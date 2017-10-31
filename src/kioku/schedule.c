#include "kioku/schedule.h"
#include "kioku/log.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#define srsTIME_YEAR_OFFSET 1900
#define srsTIME_MONTH_OFFSET 1
bool srsTime_ToString(const srsTIME time, srsTIME_STRING string)
{
  bool result = false;
  /* Bounds checks */
  if (string == NULL)
  {
    goto end;
  }
  if (time.month + srsTIME_MONTH_OFFSET > 12)
  {
    goto end;
  }
  if (time.day < 1 || time.day > 31)
  {
    goto end;
  }
  /* Conversion */
  struct tm stdtime = { .tm_year=time.year - srsTIME_YEAR_OFFSET,
                        .tm_mon=time.month - srsTIME_MONTH_OFFSET,
                        .tm_mday=time.day,
                        .tm_hour=0,
                        .tm_min=0,
                        .tm_sec=0 /* NOTE: if we implement this, account for pre-C99 range defect (0-61 allowing for 2 leap seconds) */
  };
  size_t length = strftime(string, srsTIME_STRING_SIZE, srsTIME_DATE_FORMAT, &stdtime);
  result = (length != 0);
  /* Return result */
end:
  return result;
}

bool srsTime_FromString(const srsTIME_STRING string, srsTIME *time)
{
  bool result = false;
  if (string == NULL)
  {
    goto end;
  }
  if (time == NULL)
  {
    goto end;
  }
  unsigned int year = 0;
  unsigned int month = 0;
  unsigned int day = 0;
  int found = sscanf(string, srsTIME_SCAN_FORMAT, &year, &month, &day);
  result = (found == 3);
  if (!result)
  {
    goto end;
  }
  result = (year >= 1900);
  if (!result)
  {
    goto end;
  }
  result = (month <= 12);
  if (!result)
  {
    goto end;
  }
  result = ((day >= 1) && (day <= 31));
  if (!result)
  {
    goto end;
  }
  memset(time, 0, sizeof(*time));
  time->year = year;
  time->month = month;
  time->day = day;
end:
  return result;
}
