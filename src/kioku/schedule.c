#include "kioku/schedule.h"
#include "kioku/log.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
  if (time.hour >= 24)
  {
    goto end;
  }
  if (time.minute >= 60)
  {
    goto end;
  }
  /* Conversion */
  struct tm stdtime = { .tm_year=time.year - srsTIME_YEAR_OFFSET,
                        .tm_mon=time.month - srsTIME_MONTH_OFFSET,
                        .tm_mday=time.day,
                        .tm_hour=time.hour,
                        .tm_min=time.minute,
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
  unsigned int hour = 0;
  unsigned int minute = 0;
  int found = sscanf(string, srsTIME_SCAN_FORMAT, &year, &month, &day, &hour, &minute);
  result = (found == 5);
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
  result = ((hour >= 0) && (hour < 24));
  if (!result)
  {
    goto end;
  }
  result = ((minute >= 0) && (hour < 60));
  if (!result)
  {
    goto end;
  }
  memset(time, 0, sizeof(*time));
  time->year = year;
  time->month = month;
  time->day = day;
  time->hour = hour;
  time->minute = minute;
end:
  return result;
}

static srsTIME srsTime_FromTM(struct tm *timeinfo)
{
  srsTIME kioku_time = {0};
  kioku_time.year = timeinfo->tm_year + srsTIME_YEAR_OFFSET;
  kioku_time.month = timeinfo->tm_mon + srsTIME_MONTH_OFFSET;
  kioku_time.day = timeinfo->tm_mday;
  kioku_time.hour = timeinfo->tm_hour;
  kioku_time.minute = timeinfo->tm_min;
  return kioku_time;
}

srsTIME srsTime_Now()
{
  time_t now = time(0);
  struct tm * timeinfo = localtime(&now);
  return srsTime_FromTM(timeinfo);
}

int srsTime_Compare(const srsTIME left, const srsTIME right)
{
  int result = 0;
  result = left.year - right.year;
  if (result != 0)
  {
    goto done;
  }
  result = left.month - right.month;
  if (result != 0)
  {
    goto done;
  }
  result = left.day - right.day;
  if (result != 0)
  {
    goto done;
  }
  result = left.hour - right.hour;
  if (result != 0)
  {
    goto done;
  }
  result = left.minute - right.minute;
  if (result != 0)
  {
    goto done;
  }
done:
  return result;
}
