/**
 * ----------------------------------------------------------------------------
 * Company    : SLAC National Accelerator Laboratory
 * ----------------------------------------------------------------------------
 * Description: Simple performance test 
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
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <limits.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

#include <epicsExport.h>
#include <iocsh.h>
#include <epicsThread.h>

#define DATA_TO_WRITE (50 * 1000000)
#define SMALL_FILES 4096

#define MIN(x, y) ((x) < (y) ? (x) : (y))

static double
time_now()
{
  struct timespec tp;
  clock_gettime(CLOCK_MONOTONIC, &tp);
  return (double)(tp.tv_sec) + tp.tv_nsec / 1e9;
}

/**
 * Run simple read/write fs perf test
 */
static void
fp_run_io_test(const char* path, size_t blockSize)
{
  char file[PATH_MAX];
  snprintf(file, sizeof(file), "%s/testfile0.txt", path);
  remove(file);

  int fd = open(file, O_CREAT | O_RDWR, 0777);
  if (fd < 0) {
    perror("fp_run_io_test: open");
    return;
  }

  /* create scratch buffer and fill it with random junk */
  char* buf = malloc(blockSize);
  char num[0xF] = "123456789ABCDEF";
  for (int i = 0; i < blockSize; ++i)
    buf[i] = num[rand() & 0xF];

  double start = time_now();

  /* measure write perf */
  ssize_t rem = DATA_TO_WRITE;
  while (rem > 0) {
    ssize_t written = write(fd, buf, MIN(blockSize,rem));
    if (written < 0) {
      perror("write");
      goto error;
    }
    rem -= written;
  }

  double end = time_now();

  const double write_time = end-start;

  printf(
    "[WritePerf][blockSize=%lld] write done in %.2f seconds -- %.2f MiB/s, %.2f bytes/s\n",
    (long long)blockSize,
    write_time,
    (DATA_TO_WRITE / write_time) / (1024. * 1024.),
    DATA_TO_WRITE / write_time
  );

  start = time_now();

  /* measure read perf */
  rem = 0;
  lseek(fd, 0, SEEK_SET);
  while (rem < DATA_TO_WRITE) {
    ssize_t nr = read(fd, buf, blockSize);
    if (nr < 0) {
      perror("read");
      goto error;
    }
    else if (nr == 0)
      break;
    rem += nr;
  }

  if (rem != DATA_TO_WRITE) {
    printf("%lld != %d!\n", (long long)rem, DATA_TO_WRITE);
  }

  end = time_now();
  const double read_time = end - start;

  printf(
    "[WritePerf][blockSize=%lld] read  done in %.2f seconds -- %.2f MiB/s, %.2f bytes/s\n",
    (long long)blockSize,
    read_time,
    (DATA_TO_WRITE / read_time) / (1024. * 1024.),
    DATA_TO_WRITE / read_time
  );

  /* remove the file */
  close(fd);
  remove(file);

  return;

error:
  close(fd);
}

/**
 * Small file access in directory
 */
static void
fp_run_small_files(const char* dir)
{
  /* remove everything first */
  for (int i = 0; i < SMALL_FILES; ++i) {
    char file[PATH_MAX];
    snprintf(file, sizeof(file), "%s/smallfile%05d", dir, i);
    remove(file);
  }

  /* create a bunch of misc files in the dir */
  double start = time_now();
  for (int i = 0; i < SMALL_FILES; ++i) {
    char file[PATH_MAX];
    snprintf(file, sizeof(file), "%s/smallfile%05d", dir, i);
    int fd;
    if ((fd = creat(file, 0777)) < 0) {
      perror("create");
      goto error;
    }
    close(fd);
  }
  double end = time_now();

  const double create_time = end-start;
  printf(
    "[SmallFiles] Created %d files in %.2f seconds -- %.2f files/sec\n",
    SMALL_FILES,
    create_time,
    SMALL_FILES / create_time
  );

  /* read all directory entries */
  size_t numRead = 0;
  start = time_now();
  DIR* d = opendir(dir);
  if (!d) {
    perror("opendir");
    goto error;
  }

  struct dirent* de = NULL;
  while ((de = readdir(d)))
    numRead++;

  closedir(d);

  end = time_now();
  const double readdir_time = end - start;

  printf(
    "[SmallFiles] Read %lld dirents in %.2f seconds -- %.2f files/sec\n",
    (long long)numRead,
    readdir_time,
    numRead / readdir_time
  );

  /* stat all files */
  start = time_now();
  for (int i = 0; i < SMALL_FILES; ++i) {
    char file[PATH_MAX];
    snprintf(file, sizeof(file), "%s/smallfile%05d", dir, i);
    struct stat st;
    if (lstat(file, &st) < 0) {
      perror("lstat");
      goto error;
    }
  }
  end = time_now();
  const double stat_time = end-start;

  printf(
    "[SmallFiles] Stat'ed %d files in %.2f seconds -- %.2f files/sec\n",
    SMALL_FILES,
    stat_time,
    SMALL_FILES / stat_time
  );

  /* remove all files */
  start = time_now();
  for (int i = 0; i < SMALL_FILES; ++i) {
    char file[PATH_MAX];
    snprintf(file, sizeof(file), "%s/smallfile%05d", dir, i);
    remove(file);
  }

  const double remove_time = time_now() - start;

  printf(
    "[SmallFiles] Removed %d files in %.2f seconds -- %.2f files/sec\n",
    SMALL_FILES,
    remove_time,
    SMALL_FILES / remove_time
  );

  return;
error:
  printf("Test failed\n");
}

void
fp_run_tests(const char* path)
{
  fp_run_io_test(path, 1024);
  fp_run_io_test(path, 2048);
  fp_run_io_test(path, 4096);
  fp_run_io_test(path, 8192);

  fp_run_small_files(path);
}

static epicsThreadId threadId;

static void
fsperf_thread_proc(void* arg)
{
  char* p = arg;
  fp_run_tests(p);
  free(p);
  threadId = 0;
}


static void
fsperf_run(const iocshArgBuf* args)
{
  if (!args || !args[0].sval) {
    printf("USAGE: fsperf_run PATH\n");
    return;
  }

  if (threadId != 0) {
    printf("Thread already running!\n");
    return;
  }

  threadId = epicsThreadCreate(
    "FSPERF",
    epicsThreadPriorityHigh,
    epicsThreadStackBig,
    fsperf_thread_proc,
    strdup(args[0].sval)
  );
}

void
fsperf_register()
{
  static const iocshArg arg0 = {"path", iocshArgString};
  static const iocshArg* args[] = {&arg0};
  static const iocshFuncDef func = {"fsperf_run", 1, args};
  iocshRegister(&func, fsperf_run);
}

epicsExportRegistrar(fsperf_register);

// vim: sw=2 et
