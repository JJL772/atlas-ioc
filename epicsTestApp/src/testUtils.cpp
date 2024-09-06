
#include "testUtils.h"

char* get_time(char* buf, size_t maxsize, epicsTimeStamp* ts) {
	epicsTimeStamp now;
    if (!ts) {
        ts = &now;
	    epicsTimeGetCurrent(ts);
    }
	struct tm tm;
	unsigned long ns;
	epicsTimeToTM(&tm, &ns, ts);
	(void)strftime(buf, maxsize, "[%F %T]", &tm);
	return buf;
}

StackStr get_time(epicsTimeStamp* ts) {
    StackStr s;
    get_time(s.get(), s.max_size(), ts);
    return s;
}
