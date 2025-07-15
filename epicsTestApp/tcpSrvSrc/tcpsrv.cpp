/**
 * ----------------------------------------------------------------------------
 * Company    : SLAC National Accelerator Laboratory
 * ----------------------------------------------------------------------------
 * Description: Simple TCP server running on the host. Loops data back to the
 *  target system.
 * ----------------------------------------------------------------------------
 * This file is part of 'atlas-ioc'. It is subject to the license terms in the
 * LICENSE.txt file found in the top-level directory of this distribution,
 * and at:
 *    https://confluence.slac.stanford.edu/display/ppareg/LICENSE.html.
 * No part of 'atlas-ioc', including this file, may be copied, modified,
 * propagated, or distributed except according to the terms contained in the
 * LICENSE.txt file.
 * ----------------------------------------------------------------------------
 **/
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>
#include <string.h>
#include <pthread.h>
#include <stdint.h>
#include <signal.h>

static void*
worker(void* param)
{
  sigset_t ss;
  sigemptyset(&ss);
  sigaddset(&ss, SIGPIPE);
  pthread_sigmask(SIG_BLOCK, &ss, NULL);
  
  int ns = (int)(uintptr_t)param;
  const int BUF_SZ = 1<<20;
  char* buf = (char*)malloc(BUF_SZ);
  memset(buf, 0, BUF_SZ);

  ssize_t r = 0;
  while ((r = recv(ns, buf, BUF_SZ, 0)) >= 0) {
    usleep(rand() % 500);
    if (send(ns, buf, r, 0) < 0) {
      perror("sendto");
      break;
    }
  }

  if (r < 0) {
    perror("recvfrom");
  }
  close(ns);
  return 0;
}

int
main(int argc, char** argv)
{
  int opt = -1, port = 4096;
  char addr[64] = "0.0.0.0";
  while ((opt = getopt(argc, argv, "p:a:")) != -1) {
    switch(opt) {
    case 'p':
      port = atoi(optarg);
      break;
    case 'a':
      strcpy(addr, optarg);
      break;
    }
  }

  printf("listening on %s:%d\n", addr, port);

  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    perror("socket");
    return -1;
  }

  struct sockaddr_in myaddr = {0};
  myaddr.sin_addr.s_addr = inet_addr(addr);
  myaddr.sin_family = AF_INET;
  myaddr.sin_port = htons(port);

  if (bind(sock, (struct sockaddr*)&myaddr, sizeof(myaddr)) < 0) {
    perror("bind");
    close(sock);
    return -1;
  }

  if (listen(sock, 0) < 0) {
    perror("listen");
    close(sock);
    return -1;
  }
  
  int ns = 0;
  struct sockaddr_in input = {0};
  socklen_t sl = sizeof(input);
  while ((ns = accept(sock, (struct sockaddr*)&input, &sl))) {
    pthread_attr_t att;
    pthread_attr_init(&att);
    pthread_t thr;
    pthread_create(&thr, &att, worker, (void*)(uintptr_t)ns);
  }

  close(sock);
  return 0;
}