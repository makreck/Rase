/*
 * ==============================================================================
 *
 *  PROJECT:     "Rase" Radio Sensor Project,    Measuring and Config Application
 *  COPYRIGHT:   (C)2025-2026 KKS-Elektronik,  M. Kreck, <makreck@googlemail.com>
 *
 *  This program is free software: you can redistribute it and/or modify it under
 *  the terms of the GNU General Public License as published by the Free Software
 *  Foundation, either version 3 of the License, or (at your option) any later
 *  version.
 *
 *  This program is distributed in the hope that it will be useful,   but WITHOUT
 *  ANY WARRANTY, without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE, see the GNU General Public License for details.
 *
 *  You should have received a copy of the  GNU General Public License along with
 *  this program. If not, see <https://www.gnu.org/licenses/>.
 *
 * ==============================================================================
 */

#include "includes.h"

const int monMaxDays[] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
const int yearDays[] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };
const char* intrinsic_date_month_names[] = { "not-a-month", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec", };

const char *weekDayName[] = {
    "monday",
    "tuesday",
    "wednesday",
    "thursday",
    "friday",
    "saturday",
    "sunday",
};

const char *yearMonthName[] = {
    "january",
    "february",
    "march",
    "april",
    "may",
    "june",
    "july",
    "august",
    "september",
    "october",
    "november",
    "december",
};

Times::Times(long unixTimeMillis) {
    set_from_unix_time(unixTimeMillis);
}

Times::Times(double timeDateFactor) {
    set_timecode(timeDateFactor);
}

Times::Times(Times& givenSource) {
    set(givenSource);
}

Times::Times(int gpsWeek, int gpsTimeOfWeek) {
    set_gps_time(gpsWeek, gpsTimeOfWeek);
}

Times::Times(TimeInitializer initType) {
    switch (initType) {
        case TimeInitializer::today: {
            set_now_utc();
            wHour = 0;
            wMinute = 0;
            wSecond = 0;
            wMilliseconds = 0;
        } break;

        case TimeInitializer::month: {
            set_now_utc();
            wDay = 1;
            wHour = 0;
            wMinute = 0;
            wSecond = 0;
            wMilliseconds = 0;
        } break;

        case TimeInitializer::year: {
            set_now_utc();
            wMonth = 1;
            wDay = 1;
            wHour = 0;
            wMinute = 0;
            wSecond = 0;
            wMilliseconds = 0;
        } break;

        case TimeInitializer::UTC: {
            set_now_utc();
        } break;

        case TimeInitializer::now:
        default: {
            set_now_local();
        } break;
    }
}

uint64_t Times::get_current_time_millis(void) {
    timespec ts{ 0 };
    clock_gettime(CLOCK_REALTIME, &ts);
    return (((int64_t)ts.tv_sec * (int64_t)TC_MILLISECONDS_PER_SECOND) + (int64_t)(((long double)ts.tv_nsec / (long double)TC_NANOSECOND_PER_MILLISECONDS) + 0.0005));
}

void Times::set(Times& givenSource) {
    wYear         = givenSource.wYear;
    wMonth        = givenSource.wMonth;
    wDayOfWeek    = givenSource.wDayOfWeek;
    wDay          = givenSource.wDay;
    wHour         = givenSource.wHour;
    wMinute       = givenSource.wMinute;
    wSecond       = givenSource.wSecond;
    wMilliseconds = givenSource.wMilliseconds;
}

void Times::set_date(int givenYear, int givenMonth, int givenDay) {
    wYear         = (int16_t)(givenYear);
    wMonth        = (int16_t)(givenMonth);
    wDay          = (int16_t)(givenDay);
    wDayOfWeek    = get_day_from_timecode(get_timecode());
}

void Times::set_time(int givenHour, int givenMinute, int givenSecond) {
    wHour         = (int16_t)(givenHour);
    wMinute       = (int16_t)(givenMinute);
    wSecond       = (int16_t)(givenSecond);
    wMilliseconds = 0;
}

void Times::clear_time(void) {
    wHour         = 0;
    wMinute       = 0;
    wSecond       = 0;
    wMilliseconds = 0;
}

void Times::set_time(int givenHour, int givenMinute, int givenSecond, int givenMillis) {
    wHour         = (int16_t)(givenHour);
    wMinute       = (int16_t)(givenMinute);
    wSecond       = (int16_t)(givenSecond);
    wMilliseconds = (int16_t)(givenMillis);
}

void Times::set_time(long unixTimeMillis) {
    set_from_unix_time(unixTimeMillis);
}

void Times::clear(void) {
    wYear         = 0;
    wMonth        = 0;
    wDayOfWeek    = 0;
    wDay          = 0;
    wHour         = 0;
    wMinute       = 0;
    wSecond       = 0;
    wMilliseconds = 0;
}

void Times::set_unix_time_begin(void) {
    wYear         = 1970;
    wMonth        = 1;
    wDay          = 1;
    wHour         = 0;
    wMinute       = 0;
    wSecond       = 0;
    wMilliseconds = 0;
}

void Times::set_gps_time(int gpsWeek, int gpsTimeOfWeek) {
    set_date(1980, 1, 5);
    set_time(0, 0, 0);
    set_timecode(get_factor_internal() + ((double)(gpsWeek) * 7.0));

    double time = (double)(gpsTimeOfWeek) / 1000.0;
    int nDays = (int)(time / 86400.0);
    wDay += (int16_t)(nDays);
    time = time - (double)(nDays * 86400);
    wHour = (int16_t)(time / 3600.0);
    time = time - (double)(wHour * 3600);
    wMinute = (int16_t)(time / 60.0);
    time = time - (double)(wMinute * 60);
    wSecond = (int16_t)(time);
    time = frac(time);
    wMilliseconds = (int16_t)((time + 0.0005) * 1000.0);

    normalize();
}

bool Times::is_leap_year(double factor) {
    Times t(factor);
    return (is_leap_year(t.wYear));
}

bool Times::is_leap_year(int year) {
    bool result = false;
    if ((year % 4)   == 0) { result = true;  }
    if ((year % 100) == 0) { result = false; }
    if ((year % 400) == 0) { result = true;  }
    return (result);
}

int Times::get_daycount_of_year(int year) {
    return (is_leap_year(year) ? 366 : 365);
}

int Times::get_daycount_of_month(int year, int month) {
    int monDays = 0;
    if ((year >= 1582) && (month >= 1) && (month <= 12)) {
        if (month == 2) {
            if (is_leap_year(year)) {
                monDays = 29;
            } else {
                monDays = 28;
            }
        } else {
            monDays = monMaxDays[month - 1];
        }
    }
    return (monDays);
}

int16_t Times::get_day_from_timecode(double factor) {
    long intFactor = (long)(factor);
    long day = intFactor - ((intFactor / 7) * 7) - 1;
    if (day < 0) { day += 7; }
    if (day > 6) { day -= 7; }
    return ((int16_t)(day));
}

void Times::normalize(void) {
    bool flag = false;

    int year  = (int)(wYear);
    int month = (int)(wMonth);
    int day   = (int)(wDay);

    if ((year > 1582) && (month > 0) && (day > 0)) {
        flag = true;
    }

    int hour         = (int)(wHour);
    int minute       = (int)(wMinute);
    int second       = (int)(wSecond);
    int milliseconds = (int)(wMilliseconds);

    if (milliseconds < 0) {
        while (milliseconds < 0) {
            milliseconds += 1000;
            second -= 1;
        }
    } else {
        while (milliseconds > 999) {
            milliseconds -= 1000;
            second += 1;
        }
    }

    if (second < 0) {
        while (second < 0) {
            second += 60;
            minute -= 1;
        }
    } else {
        while (second > 59) {
            second -= 60;
            minute += 1;
        }
    }

    if (minute < 0) {
        while (minute < 0) {
            minute += 60;
            hour -= 1;
        }
    } else {
        while (minute > 59) {
            minute -= 60;
            hour += 1;
        }
    }

    if (hour < 0) {
        while (hour < 0) {
            hour += 24;
            if (flag) {
                day -= 1;
            }
        }
    } else {
        while (hour > 23) {
            hour -= 24;
            if (flag) {
                day += 1;
            }
        }
    }

    if (flag) {
        while (month > 12) {
            year++;
            month -= 12;
        }

        int dom = get_daycount_of_month(year, month);
        if (day < 1) {
            while (day < 1) {
                day += dom;
                month -= 1;

                if (month < 1) {
                    month = 12;
                    year -= 1;
                }

                dom = get_daycount_of_month(year, month);
            }
        } else {
            while (day > dom) {
                day -= dom;
                month += 1;

                if (month > 12) {
                    month = 1;
                    year += 1;
                }

                dom = get_daycount_of_month(year, month);
            }
        }
    }

    wHour          = (int16_t)(hour);
    wMinute        = (int16_t)(minute);
    wSecond        = (int16_t)(second);
    wMilliseconds  = (int16_t)(milliseconds);

    if (flag) {
        wYear      = (int16_t)(year);
        wMonth     = (int16_t)(month);
        wDay       = (int16_t)(day);
        wDayOfWeek = get_day_from_timecode(get_factor_internal());
    }
}

double Times::get_day_time(void) {
    long double time    = ((long double)wHour   * (long double)TC_MILLISECONDS_PER_HOUR) 
                        + ((long double)wMinute * (long double)TC_MILLISECONDS_PER_MINUTE) 
                        + ((long double)wSecond * (long double)TC_MILLISECONDS_PER_SECOND)
                        + ((long double)wMilliseconds);
    return ((double)(time / (long double)TC_MILLISECONDS_PER_DAY));
}

double Times::get_timecode(void) {
    if (wMonth == 0) {
        wMonth = (int16_t)(1);
    }
    if (wDay == 0) {
        wDay = (int16_t)(1);
    }
    wYear = (int16_t)(std::max(1582, (int)(wYear)));
    normalize();
    return (get_factor_internal());
}

void Times::set_timecode(double f) {
    int64_t intFactor = (int64_t)(f);
    long double factor = (long double)(intFactor);
    wYear = (int16_t)(factor * TC_DAYTOYEAR);
    int64_t day = std::max(0L, std::min(366L, (long)factor - (long)((double)wYear * TC_YEARTODAY)));
    if (is_leap_year(wYear) && (day > 60)) {
        day--;
    }        
    for (wMonth = 1; wMonth < 12; wMonth++) {
        if (yearDays[wMonth] >= (long)day) {
            break;
        }
    }
    wDay = (int16_t)((day - (int64_t)(yearDays[wMonth - 1])) & 0x0000ffffL);
    wDayOfWeek = get_day_from_timecode(factor);

    int64_t lPart = (int64_t)((((long double)f - (long double)intFactor) * (long double)TC_MILLISECONDS_PER_DAY));
    wHour = (int16_t)(lPart / (int64_t)TC_MILLISECONDS_PER_HOUR);
    lPart %= (int64_t)TC_MILLISECONDS_PER_HOUR;
    wMinute = (int16_t)(lPart / (int64_t)TC_MILLISECONDS_PER_MINUTE);
    lPart %= (int64_t)TC_MILLISECONDS_PER_MINUTE;
    wSecond = (int16_t)(lPart / (int64_t)TC_MILLISECONDS_PER_SECOND);
    lPart %= (int64_t)TC_MILLISECONDS_PER_SECOND;
    wMilliseconds = (int16_t)lPart;
}

int Times::week(void) {
    return (get_week_from_timecode(get_today()));
}

int Times::get_week(void) {
    return (get_week_from_timecode(get_timecode()));
}

std::string Times::to_date(void) {
    char s[64]{0};
    snprintf(s, sizeof(s), "%04d-%02d-%02d", wYear, wMonth, wDay);
    return (std::string(s));
}

std::string Times::to_time_hms(void) {
    char s[64]{0};
    snprintf(s, sizeof(s), "%02d:%02d:%02d", wHour, wMinute, wSecond);
    return (std::string(s));
}

std::string Times::to_date_and_time_hms(const char* gap) {
    if (gap == nullptr) {
        gap = " ";
    }
    char s[256]{0};
    snprintf(s, sizeof(s), "%04d-%02d-%02d%s%02d:%02d:%02d", wYear, wMonth, wDay, gap, wHour, wMinute, wSecond);
    return (std::string(s));
}

std::string Times::to_time(void) {
    char s[64]{0};
    snprintf(s, sizeof(s), "%02d:%02d:%02d.%03d", wHour, wMinute, wSecond, wMilliseconds);
    return (std::string(s));
}

std::string Times::to_short_time(void) {
    char s[64]{0};
    snprintf(s, sizeof(s), "%02d:%02d", wHour, wMinute);
    return (std::string(s));
}

std::string Times::to_string() {
    return (to_date() + " - " + to_time());
}

double Times::get_timecode_of_unix_base_date() {
    return (719528.0);
}

double Times::unix_time_to_timecode(long unixTime_ms) {
    return (get_timecode_of_unix_base_date() + ((double)(unixTime_ms) / (long double)TC_MILLISECONDS_PER_DAY));
}

void Times::set_from_unix_time(long unixTime_ms) {
    set_timecode(unix_time_to_timecode(unixTime_ms));
}

bool Times::is_later_as(Times& thisOne) {
    return (get_timecode() > thisOne.get_timecode());
}

double Times::get_factor_internal(void) {
    int64_t date    = (int64_t)((long double)wYear * TC_YEARTODAY) 
                    + (int64_t)yearDays[wMonth - 1] 
                    + (((wMonth > 2) && is_leap_year(wYear)) ? 1LL : 0LL) 
                    + (int64_t)wDay;
    return ((double)(date) + get_day_time());
}

double Times::frac(double a) {
    return (a - ((double)((long)(a))));
}

void Times::set_now_local(void) {
    set_from_unix_time(get_unix_time_utc_ms() + get_local_timezone_offset_ms() + get_local_summertime_offset_ms());
}

void Times::set_now_utc(void) {
    set_from_unix_time(get_unix_time_utc_ms());
}

double Times::get_begin_of_year(double timeDateFactor) {
    Times t((double)((int)timeDateFactor));
    t.wMonth = 1;
    t.wDay = 1;
    t.clear_time();
    return (t.get_timecode());
}

int Times::get_week_from_timecode(double timeDateFactor) {
    double baseOfYear = get_begin_of_year(timeDateFactor);
    double offsetInYear = (double)((int)timeDateFactor) - baseOfYear;
    return ((int)(offsetInYear / 7.0) + 1);
}

double Times::get_begin_of_week(double timeDateFactor) {
    double baseOfYear = get_begin_of_year(timeDateFactor);
    double offsetInYear = (double)((int)timeDateFactor) - baseOfYear;
    int weekBaseNull = (int)(offsetInYear / 7.0);
    return (((double)weekBaseNull * 7.0) + baseOfYear + 1.0);
}

void Times::set_to_begin_of_week(void) {
    clear_time();
    set_timecode(get_begin_of_week(get_timecode()));
}

double Times::get_now(void) {
    Times t(TimeInitializer::now);
    return (t.get_timecode());
}

double Times::get_today(void) {
    Times t(TimeInitializer::today);
    return (t.get_timecode());
}

double Times::get_current_week_begin(void) {
    return (get_current_year_begin() + (get_week_from_timecode(get_today()) * 7.0));
}

double Times::get_current_month_begin(void) {
    Times t(TimeInitializer::month);
    return (t.get_timecode());
}

double Times::get_current_year_begin(void) {
    Times t(TimeInitializer::year);
    return (t.get_timecode());
}

std::string Times::name_of_day(int dayOfWeek) {
    if ((dayOfWeek >= 0) && (dayOfWeek < SIZEOFARRAY(weekDayName))) {
        return (weekDayName[dayOfWeek]);
    }
    return ("");
}

std::string Times::name_of_month(int month) {
    int i = std::max(0, std::min((int)SIZEOFARRAY(yearMonthName), (int)(month - 1)));
    return (yearMonthName[i]);
}

double Times::get_timestamp_utc(void) {
    return (unix_time_to_timecode(get_unix_time_utc_ms()));
}

double Times::get_timestamp_local(TimeStampFormat format) {
    Times t(get_unix_time_utc_ms() + get_local_timezone_offset_ms() + get_local_summertime_offset_ms());
    switch (format) {
        case TimeStampFormat::lockYear: {
            t.clear_time();
            t.wMonth = 0;
            t.wDay = 0;
        } break;

        case TimeStampFormat::lockMonth: {
            t.clear_time();
            t.wDay = 0;
        } break;

        case TimeStampFormat::lockWeek: {
            t.set_to_begin_of_week();
        } break;

        case TimeStampFormat::lockDay: {
            t.clear_time();
        } break;

        case TimeStampFormat::lockHour: {
            t.wMinute = 0;
            t.wSecond = 0;
            t.wMilliseconds = 0;
        } break;

        case TimeStampFormat::lockMinute: {
            t.wSecond = 0;
            t.wMilliseconds = 0;
        } break;

        case TimeStampFormat::lockSecond: {
            t.wMilliseconds = 0;
        } break;

        case TimeStampFormat::full:
        default: {
        } break;
    }
    return (t.get_timecode());
}

double Times::get_timestamp_local(void) {
    return (get_timestamp_local(TimeStampFormat::full));
}

std::string Times::timestamp_utc() {
    Times t(TimeInitializer::UTC);
    return (t.format(InFormat::dateAndTime));
}

std::string Times::timestamp_local() {
    Times t(TimeInitializer::now);
    return (t.format(InFormat::dateAndTime));
}

double Times::get_timecode_offset_of(int hour, int minute, int seconds, int millis) {
    return (((((double)std::min(23, std::max(0, hour)) * 3600.0) + ((double)std::min(59, std::max(0, minute)) * 60.0) + (double)std::min(59, std::max(0, seconds))) + (std::min(999, std::max(0, millis)) / 1000.0)) / TC_SECONDS_PER_DAY);
}

double Times::get_date_offset_when_no_leap_year(int month, int day, int hour, int minute, int seconds, int millis) {
    if ((month < 1) || (month > 12) || (day < 1))
        return (0.0);
    if (day > monMaxDays[month - 1])
        return (0.0);
    double dayOffset = (double)(day - 1);
    for (int i = 0; i < (month - 1); i++)
    {
        dayOffset += (double)monMaxDays[i];
    }
    double offsetInDay = get_timecode_offset_of(hour, minute, seconds, millis);
    return (dayOffset + offsetInDay);
}

double Times::get_utc_timeshift(void) {
    return ((double)(get_local_timezone_offset_ms() + get_local_summertime_offset_ms()) / (long double)TC_MILLISECONDS_PER_DAY);
}

int64_t Times::get_unix_time_utc_ms(void) {
    return (get_current_time_millis());
}

// Function returns time zone offset including DST (daylight saving time) offset.
int64_t Times::get_local_timezone_offset_ms() {
    time_t t = time(nullptr);
    tm localtime{ 0 };
    localtime_r(&t, &localtime);
    return ((int64_t)localtime.tm_gmtoff * (int64_t)TC_MILLISECONDS_PER_SECOND);
}

int64_t Times::get_local_summertime_offset_ms(void) {
    // ocaltime.tm_gmtoff contains DST also. So, this function returns always 0 only.
    return (0);
}

int8_t Times::get_utc_timeshift_byte(void) {
    double timeShift = Times::get_utc_timeshift() * TC_UTC_TIME_SHIFT_FACTOR;
    if (timeShift > 0.0) {
        timeShift += 0.5;
    } else if (timeShift < 0.0) {
        timeShift -= 0.5;
    }
    return ((int8_t)(timeShift));
}

double Times::get_timecode_offset_from_utc_shift_byte(int8_t utcTimeShiftByte) {
    return ((double)((int)utcTimeShiftByte) / TC_UTC_TIME_SHIFT_FACTOR);
}

app_err_t Times::format_timecode_date_week_time_ms(double timeCode, char* pszDate, size_t lengthDate, char* pszTime, size_t lengthTime, bool useLocalTime) {
    double tc;

    if (timeCode == 0.0) {
        Times t(TimeInitializer::UTC);
        tc = t.get_timecode();
    } else {
        tc = timeCode;
    }
    
    if (useLocalTime) {
        tc += get_utc_timeshift();
    }
    
    Times sysTime(tc);
    int weekNumber = get_week_from_timecode(tc);

    app_err_t result = AppState::failed;

    if ((pszDate != nullptr) && (lengthDate > 1)) {
        snprintf(pszDate, lengthDate, "%4.4d-%2.2d-%2.2d W%d", sysTime.wYear, sysTime.wMonth, sysTime.wDay, weekNumber);
        result = AppState::ok;
    }

    if ((pszTime != nullptr) && (lengthTime > 1)) {
        snprintf(pszTime, lengthTime, "%2.2d:%2.2d:%2.2d.%3.3d", sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds);
        result = AppState::ok;
    }

    return (result);
}

std::string Times::format(InFormat fmt) {
    switch (fmt) {
        case InFormat::time:          return (to_time());
        case InFormat::time_hms:      return (to_time_hms());
        case InFormat::shortTime:     return (to_short_time());
        case InFormat::date:          return (to_date());
        case InFormat::dateAndTime:   return (to_string());
        case InFormat::day:           return (name_of_day(wDayOfWeek));
        case InFormat::week:          return ("" + get_week_from_timecode(get_week()));
        case InFormat::month:         return (name_of_month(wMonth));

        case InFormat::factor: {
            char s[32]{ 0 };
            snprintf(s, sizeof (s), "%f", get_timecode());
            return (std::string(s));
        };

        case InFormat::year: {
            char s[32]{ 0 };
            snprintf(s, sizeof (s), "%d", (int)wYear);
            return (std::string(s));
        };

        default: {
        } break;
    }
    return (to_string());
}

std::string Times::format(InFormat fmt, double factor) {
    Times t(factor);
    return (t.format(fmt));
}

std::string Times::format_timespan(double range) {
    range = fabs(range);
    double span = range;
    char string[256]{ 0 };
    std::string s;

    if (span > 1.0) {               // multiple days
        int days = (int)(span);
        span = span - (double)days;
        snprintf(string, sizeof (string), "%dd ", days);
        s.append(string);
    }

    if (span > TC_HOUR) {           // multiple hours
        int hours = (int)(span / TC_HOUR);
        span = span - ((double)hours * TC_HOUR);
        snprintf(string, sizeof (string), "%dh ", hours);
        s.append(string);
    }

    if (span > TC_MINUTE) {  // multiple minutes
        int minutes = (int)(span / TC_MINUTE);
        span = span - ((double)minutes * TC_MINUTE);
        int seconds = (int)((span / TC_SECOND) + 0.5);
        if (seconds >= 60) {
            minutes++;
            seconds -= 60;
        }
        snprintf(string, sizeof (string), "%dmin ", minutes);
        s.append(string);
        if (seconds > 0) {
            snprintf(string, sizeof(string), "%ds ", seconds);
            s.append(string);
        }
    } else {
        if (span > TC_SECOND) {  // multiple seconds
            snprintf(string, sizeof (string), "%.3fs ", span / TC_SECOND);
            s.append(string);
        } else {
            snprintf(string, sizeof (string), "%dms ", (int)(span / TC_MILLISEC));
            s.append(string);
        }
    }

    return (s);
}

uint64_t Times::get_tick_count64(void) {
    return (Times::get_current_time_millis());
}

void Times::delay_ms(uint64_t ms) {
    usleep((__useconds_t)(ms * TC_MICROSECONDS_PER_MILLISECOND));
}

void Times::set_build_date(void) {
    const char* p = __DATE__;
    int month = 0;
    for (month = 0; month < 13; month++) {
        if (strstr(p, intrinsic_date_month_names[month]) != nullptr) {
            break;
        }
    }
    if ((month >= 1) && (month <= 12)) {
        this->wMonth = (int16_t)month; 
        this->wDay   = (int16_t)atoi(&p[4]);
        this->wYear  = (int16_t)atoi(&p[(this->wDay < 10) ? 6 : 7]);
    }
}

const char* Times::get_build_date(void) {
    return (__DATE__);
}
