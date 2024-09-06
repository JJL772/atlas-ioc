
#include <epicsExport.h>
#include <epicsStdio.h>
#include <epicsThread.h>
#include <iocsh.h>
#include <epicsStdlib.h>
#include <cantProceed.h>
#include <epicsString.h>
#include <stdlib.h>
#include <memory.h>
#include <errno.h>
#include <epicsTime.h>
#include <epicsMutex.h>
#include <unistd.h>
#include <fcntl.h>

#include "testUtils.h"
#include "getopt_s.h"

static const float FILE_OP_WAIT = 5;

static void fs_start_test(const iocshArgBuf* buf);
static void fs_report_test(const iocshArgBuf* buf);
static void fs_test_thread(void*);
static void fs_stop_tests(const iocshArgBuf* buf);
static size_t parse_byte_size(const char* sz);

void fsTest_register() {
	static const iocshArg startTestArgv = {
		"size",
		iocshArgArgv
	};
	static const iocshArg* startTestArgs[] = { &startTestArgv };
    static const iocshFuncDef startTest = {
		"fsTestStart",
		1,
		startTestArgs
	};

	iocshRegister(&startTest, fs_start_test);

	static const iocshFuncDef testReport = {
		"fsTestReport",
		0,
		NULL
	};
	iocshRegister(&testReport, fs_report_test);

	static const iocshFuncDef testStop = {
		"fsTestStop",
		0,
		NULL
	};
	iocshRegister(&testStop, fs_stop_tests);
}

epicsExportRegistrar(fsTest_register);

struct test_s {
	epicsThreadId thr;
	struct test_s* next;
	int num;
	int failures;
	char* file;
	epicsMutex mutex;	// Only needed to prevent races in the fsTestReport function. a HACK!
	int run;
};
struct test_s* s_tests = NULL;

struct thread_param_s {
	char* filename;
	size_t fsz;
	struct test_s* test;
};

static struct thread_param_s* create_thread_param(struct test_s* test, size_t sz) {
	struct thread_param_s* param = new thread_param_s();

	char cwd[256];
	getcwd(cwd, sizeof(cwd));

	static int n = 0;
	char buf[512];
	(void)snprintf(buf, sizeof(buf), "%s/testfile%d.txt", cwd, n++);
	param->filename = epicsStrDup(buf);
	param->fsz = sz;
	param->test = test;
	return param;
}

static void fs_start_test(const iocshArgBuf* buf) {

	int argc = buf->aval.ac;
	char** argv = buf->aval.av;

	struct test_s* s = new test_s();

	thread_param_s* param = create_thread_param(s, argc > 1 ? parse_byte_size(argv[1]) : 16384);

	s->next = s_tests;
	s->num = 0;
	s->failures = 0;
	s->run = 1;
	s->file = epicsStrDup(param->filename); // Sucks to duplicate but I dont want to cause memory bugs with my mistakes
	s->thr = epicsThreadCreate("fsTestThread", epicsThreadPriorityLow, epicsThreadStackMedium, fs_test_thread, param); // Do this last!
	s_tests = s;
}

static void fs_stop_tests(const iocshArgBuf* buf) {
	for (test_s* s = s_tests; s; s = s->next)
		s->run = 0;
}

static void fs_report_test(const iocshArgBuf* buf) {
	for (test_s* s = s_tests; s; s = s->next) {
		epicsGuard<epicsMutex> guard(s->mutex);
		epicsStdoutPrintf("%s, %d iterations and %d failures, %f%% fail rate. (%s)\n",
			s->file, s->num, s->failures, float(s->failures) / float(s->num ? s->num : 1), s->run ? "RUNNING" : (s->failures > 0 ? "FAIL" : "SUCCESS"));
	}
}

static double rand_sleep_time(double interval) {
	return interval + interval * (double(rand() / RAND_MAX) - 0.5);
}

static void fs_test_thread(void* param) {
	struct thread_param_s* p = (struct thread_param_s*)param;
	epicsThreadSleep(1); // Delay start for a second
	p->fsz = ALIGN_VALUE(p->fsz, 16); // To support altivec compares

	int fd = open(p->filename, O_CREAT | O_RDWR | O_TRUNC, 0666);
	for (int i = 0; fd < 0 && i < 10; ++i) {
		epicsStdoutPrintf("File %s failed to open: %s\n", p->filename, strerror(errno));
		epicsThreadSleep(0.1); // Stall 100ms, try again
		fd = open(p->filename, O_CREAT | O_RDWR | O_TRUNC, 0666);
	}

	if (fd < 0) {
		epicsStdoutPrintf("File %s failed to open after 10 retries, giving up!\n", p->filename);
		return;
	}

	const ssize_t workBufSize = 2048;

	epicsUInt8* buf = (epicsUInt8*)aligned_alloc_p(16, workBufSize);
	if (!buf) {
		epicsStdoutPrintf("Failed to malloc working buf, giving up!\n");
		return;
	}

	epicsStdoutPrintf("Opened %s, starting test. requested size is %zu bytes\n", p->filename, p->fsz);

	char tb[128];

	// Generate "random" indices to validate
	const int NUM_RANDOMS = 16;
	short randNums[NUM_RANDOMS];
	for (int i = 0; i < NUM_RANDOMS; ++i)
		randNums[i] = rand() % workBufSize;

	while(p->test->run) {

		// Write out test data
		{
			epicsGuard<epicsMutex> guard(p->test->mutex);

			// Fill working buffer with the test number
			memset(buf, p->test->num, workBufSize);

			// Rewind to the start of the file...
			if (lseek(fd, 0, SEEK_SET) != 0) {
				epicsStdoutPrintf("%s lseek failed (%s): %s\n", get_time(tb, sizeof(tb)), p->filename, strerror(errno));
				++p->test->failures;
				epicsThreadSleep(0.05);
				continue;
			}

			// Write in a loop, our working buffer will almost always be smaller than the desired file size
			ssize_t toWrite = p->fsz;
			while (toWrite > 0) {
				ssize_t ret = write(fd, buf, workBufSize > toWrite ? toWrite : workBufSize);
				if (ret < 0)
					break;
				toWrite -= ret;
			}

			// Error case, incomplete write
			if (toWrite > 0) {
				++p->test->failures;
				epicsStdoutPrintf("%s write failed (%s): %s\n", get_time(tb, sizeof(tb)), p->filename, strerror(errno));
				epicsThreadSleep(0.05);
				continue;
			}

			// Finally, flush
			if (fsync(fd) > 0) {
				epicsStdoutPrintf("%s fsync failed (%s): %s\n", get_time(tb, sizeof(tb)), p->filename, strerror(errno));
				++p->test->failures;
				epicsThreadSleep(0.05);
				continue;
			}
		}

		// Read and validate data
		{
			epicsThreadSleep(rand_sleep_time(FILE_OP_WAIT));

			epicsGuard<epicsMutex> guard(p->test->mutex);

			// Rewind to the start of the file...
			if (lseek(fd, 0, SEEK_SET) != 0) {
				epicsStdoutPrintf("%s lseek failed (%s): %s\n", get_time(tb, sizeof(tb)), p->filename, strerror(errno));
				++p->test->failures;
				epicsThreadSleep(0.05);
				continue;
			}

			ssize_t toRead = p->fsz;
			while(toRead > 0) {
				ssize_t ret = read(fd, buf, workBufSize > toRead ? toRead : workBufSize);
				if (ret < 0) {
					epicsStdoutPrintf("%s read failed (%s): %s\n", get_time(tb, sizeof(tb)), p->filename, strerror(errno));
					++p->test->failures;
					epicsThreadSleep(0.05);
					break;
				}
				toRead -= ret;

				if (!memcheck_fast(buf, p->test->num, ret)) {
					epicsStdoutPrintf("%s data failed to validate %d expected, but got something else (%s)\n", get_time(tb,sizeof(tb)), p->test->num,
						p->filename);
					++p->test->failures;
					epicsThreadSleep(0.05);
				}
			}

			++p->test->num;
		}

		epicsThreadSleep(rand_sleep_time(FILE_OP_WAIT));
	}

	epicsGuard<epicsMutex> guard(p->test->mutex);
	epicsStdoutPrintf("Test %s completed with %d iterations and %d failures, cleaning up...\n", p->filename, p->test->num, p->test->failures);
	
	free(buf);
	free(p->filename);
	free(param);
	close(fd);
}

static size_t parse_byte_size(const char* sz) {
	size_t l = strlen(sz);
	size_t s = atol(sz);
	switch(sz[l-1]) {
	case 'K':
	case 'k':
		s *= 1000;
		break;
	case 'M':
	case 'm':
		s *= 1000 * 1000;
		break;
	}
	return s;
}
