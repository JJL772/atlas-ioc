
#include <stdlib.h>
#include <stdio.h>

#include <devSup.h>
#include <longinRecord.h>
#include <aiRecord.h>
#include <epicsVersion.h>
#include <epicsExport.h>

#if EPICS_VERSION >= 7
#include <int64inRecord.h>
#endif

static long longin_init_record(void* rec);
static long longin_read(void* rec);

struct {
	long number;
	DEVSUPFUN report;
	DEVSUPFUN init;
	DEVSUPFUN init_record;
	DEVSUPFUN get_ioint_info;
	DEVSUPFUN read_longin;
	DEVSUPFUN linconv;
} devLongRandom = {
	6,
	NULL,
	NULL,
	longin_init_record,
	NULL,
	longin_read,
	NULL
};

epicsExportAddress(dset, devLongRandom);

struct longinPvt {
	int rand_max;
	int off;
};

static long longin_init_record(void* rec)
{
	longinRecord* prec = rec;
	prec->dpvt = malloc(sizeof(struct longinPvt));
	struct longinPvt* pvt = prec->dpvt;

	if (prec->inp.type == INST_IO) {
		sscanf(prec->inp.value.instio.string, "%d,%d", &pvt->rand_max, &pvt->off);
	}

	return 0;
}

static long longin_read(void* rec)
{
	longinRecord* prec = rec;
	struct longinPvt* pvt = prec->dpvt;

	prec->val = rand() % pvt->rand_max + pvt->off;

	return 0;
}

