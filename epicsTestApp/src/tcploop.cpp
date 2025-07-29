/**
 * ----------------------------------------------------------------------------
 * Company    : SLAC National Accelerator Laboratory
 * ----------------------------------------------------------------------------
 * Description: Data integrity test for TCP
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

#include <arpa/inet.h>
#include <netinet/in.h>
#include "getopt_s.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

#include "epicsExport.h"
#include "iocsh.h"

static void usage()
{
    printf("USAGE: tcploop [-s SIZE] [-c COUNT] -a ADDR [-p PORT]\n");
}

static void tcploop_CallFunc(const iocshArgBuf* args)
{
    getopt_state_t state = {};
    int opt = 0;
    int size = 4096, count = 16;
    char addr[64] = {};
    int port = 4096;
    uint32_t interval = 0;
    while ((opt = getopt_s(args[0].aval.ac, args[0].aval.av, "hs:c:a:p:i:", &state)) != -1) {
        switch (opt) {
        case 'h':
            usage();
            return;
        case 'c':
            count = atoi(state.optarg);
            break;
        case 's':
            size = atoi(state.optarg);
            break;
        case 'p':
            port = atoi(state.optarg);
            break;
        case 'a':
            strcpy(addr, state.optarg);
            break;
        case 'i':
            interval = 1000 * atof(optarg);
            printf("Sleeping for %u micros\n", interval);
            break;
        default:
            usage();
            return;
        }
    }

    if (interval > 30000000) {
        printf("max interval is 30 seconds\n");
        return;
    }

    if (!*addr) {
        usage();
        return;
    }

    if ((size % 16) != 0) {
        printf("size must be multiple of 16\n");
        return;
    }

    if (size > (1<<20)) {
        printf("size must be less than %d\n", 1<<20);
        return;
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return;
    }

    struct sockaddr_in si;
    si.sin_family = AF_INET;
    si.sin_port = htons(port);
    si.sin_addr.s_addr = inet_addr(addr);
    
    if (connect(sock, (sockaddr*)&si, sizeof(si)) < 0) {
        perror("connect");
        close(sock);
        return;
    }
    
    uint32_t* buf = (uint32_t*)calloc(size, 1);
    
    for (int i = 0; i < count; ++i) {
        const uint32_t patterns[] = {'abcd', 'xyzw', 'ABCD', 'XYZW'};
        for (int k = 0; k < size/4; ++k) {
            buf[k] = patterns[i % 4];
        }

        ssize_t toSend = size, off = 0;
        do {
            // Split between multiple TCP segments (ideally)
            if (send(sock, (uint8_t*)buf + off, toSend < 256 ? toSend : 256, 0) < 0)
                perror("send");
            off += 256;
            toSend -= 256;
            usleep(rand() % 500);
        } while(toSend > 0);

        printf("[%02d] Send %d bytes\n", i, size);

        ssize_t rem = size,guard = 100;
        off = 0;
        while (rem > 0) {
            ssize_t r = recv(sock, ((char*)buf) + off, rem, 0);
            if (r < 0) {
                perror("recv");
                break;
            }
            if (--guard < 0) // Just to avoid locking up the system if something goes wrong.
                break;
            rem -= r;
            off += r;
        }
        
        if (rem != 0) {
            printf("[%02d] Recv %ld bytes, but expected %d\n", i, rem, size);
            continue;
        }

        printf("[%02d] Recv %d bytes -- ", i, size);

        for (int k = 0; k < size/4; ++k) {
            if (buf[k] != patterns[i % 4]) {
                printf("Invalid starting at offset %d\n", k * 4);
                printf("--------- DUMP DATA ---------\n");
                char* b = (char*)buf;
                for (k = 0; k < size; k+=16) {
                    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n",
                        b[0], b[1], b[2], b[3], b[4], b[5], b[6],
                        b[7], b[8], b[9], b[10], b[11], b[12], b[13],
                        b[14], b[15]);
                }
                printf("--------- END DUMP ---------\n");
                goto done;
            }
        }

        printf("Valid\n");
    done:
        if (interval)
            usleep(interval);
        continue;
    }

    free(buf);
    close(sock);
}

/* For cexpsh; must not be static */
void tcploop(const char* arg)
{
    char buf[512];
    int argc = 0;
    char* args[32] = {};
    strcpy(buf, arg);
    for (char* s = strtok(buf, " "); s; s = strtok(NULL, " ")) {
        args[argc++] = s;
    }
    
    iocshArgBuf a;
    a.aval.ac = argc;
    a.aval.av = args;
    tcploop_CallFunc(&a);
}

void tcploop_register()
{
    static const iocshArg arg0 = {"args", iocshArgArgv};
    static const iocshArg *args[] = {&arg0};
    static const iocshFuncDef func = {"tcploop", 1, args};
    iocshRegister(&func, tcploop_CallFunc);
}

epicsExportRegistrar(tcploop_register);
