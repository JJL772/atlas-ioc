/**
 * ----------------------------------------------------------------------------
 * Company    : SLAC National Accelerator Laboratory
 * ----------------------------------------------------------------------------
 * Description: Misc utilities for tests
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
#include "testUtils.h"

char*
get_time(char* buf, size_t maxsize, epicsTimeStamp* ts)
{
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

StackStr
get_time(epicsTimeStamp* ts)
{
  StackStr s;
  get_time(s.get(), s.max_size(), ts);
  return s;
}
