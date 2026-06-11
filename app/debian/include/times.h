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

#pragma once

#define TC_MIN_YEAR                     (1582.0)             // beginning year of gregorian calendar
#define TC_DAYTOYEAR                    (400.0L / 146097.0L) // calculation factor for calendar day into year
#define TC_YEARTODAY                    (146097.0L / 400.0L) // calculation factor for calendar year into day

#define TC_MIN_DAYS_PER_MONTH           (28.0)
#define TC_AVERAGE_DAYS_PER_MONTH       (30.0)
#define TC_MAX_DAYS_PER_MONTH           (31.0)
#define TC_DAYS_PER_WEEK                (7.0)
#define TC_FULL_YEAR                    (366.9999884259259L)

#define TC_MILLISECONDS_PER_DAY         (86400000)
#define TC_ZENTISECONDS_PER_DAY         (8640000)
#define TC_DEZISECONDS_PER_DAY          (864000)
#define TC_SECONDS_PER_DAY              (86400)
#define TC_MINUTES_PER_DAY              (1440)
#define TC_HOURS_PER_DAY                (24)
#define TC_SECONDS_PER_HOUR             (3600)
#define TC_MILLISECONDS_PER_HOUR        (3600000)
#define TC_SECONDS_PER_MINUTE           (60)
#define TC_MILLISECONDS_PER_MINUTE      (60000)
#define TC_MILLISECONDS_PER_SECOND      (1000)
#define TC_MICROSECONDS_PER_MILLISECOND (1000)
#define TC_NANOSECOND_PER_MILLISECONDS  (1000000)
#define TC_UTC_TIME_SHIFT_FACTOR        (96)

#define TC_DAY          (1.0)
#define TC_HOUR         (TC_DAY / TC_HOURS_PER_DAY)
#define TC_MINUTE       (TC_DAY / TC_MINUTES_PER_DAY)
#define TC_SECOND       (TC_DAY / TC_SECONDS_PER_DAY)
#define TC_DEZI_SECOND  (TC_DAY / TC_DEZISECONDS_PER_DAY)
#define TC_ZENTI_SECOND (TC_DAY / TC_ZENTISECONDS_PER_DAY)
#define TC_MILLISEC     (TC_DAY / TC_MILLISECONDS_PER_DAY)

#define STR_TIME_DATE(_timecode)   (Times::format(InFormat::dateAndTime, (double)(_timecode)).c_str())
#define STR_TIME(_timecode)        (Times::format(InFormat::time, (double)(_timecode)).c_str())
#define STR_TIME_HMS(_timecode)    (Times::format(InFormat::time_hms, (double)(_timecode)).c_str())

enum class InFormat {
    dateAndTime = 0,
    time        = 1,
    time_hms    = 2,
    shortTime   = 3,
    date        = 4,
    factor      = 5,
    day         = 6,
    week        = 7,
    month       = 8,
    year        = 9,
}; 

enum class TimeStampFormat {
    full        = 0,
    lockSecond  = 1,
    lockMinute  = 2,
    lockHour    = 3,
    lockDay     = 4,

    lockWeek    = 5,
    lockMonth   = 6,
    lockYear    = 7,
};

enum class TimeInitializer {
    now = 0,
    UTC = 1,
    today = 2,
    month = 3,
    year = 4,
};

class Times {
    private:
        double get_factor_internal(void);

    public:
        int16_t wYear;
        int16_t wMonth;
        int16_t wDayOfWeek;
        int16_t wDay;
        int16_t wHour;
        int16_t wMinute;
        int16_t wSecond;
        int16_t wMilliseconds;

        Times(TimeInitializer initType = TimeInitializer::now);
        Times(long unixTimeMillis);
        Times(double timeDateFactor);
        Times(Times& givenSource);
        Times(int gpsWeek, int gpsTimeOfWeek);

        static std::string name_of_day(int dayOfWeek);
        static std::string name_of_month(int month);
        static std::string timestamp_utc();
        static std::string timestamp_local();
        static std::string format(InFormat fmt, double factor);
        static std::string format_timespan(double span);

        static void delay_ms(uint64_t ms);
        static bool is_leap_year(double factor);
        static bool is_leap_year(int year);
        static int get_daycount_of_year(int year);
        static int get_daycount_of_month(int year, int month);
        static int get_week_from_timecode(double timeDateFactor);
        static int week(void);
        static int8_t get_utc_timeshift_byte(void);
        static int16_t get_day_from_timecode(double factor);
        static int64_t get_local_timezone_offset_ms();
        static int64_t get_local_summertime_offset_ms(void);
        static int64_t get_unix_time_utc_ms(void);
        static uint64_t get_current_time_millis(void);
        static uint64_t get_tick_count64(void);
        static double get_timecode_of_unix_base_date();
        static double unix_time_to_timecode(long unixTime_ms);
        static double get_timecode_offset_from_utc_shift_byte(int8_t utcTimeShiftByte);
        static double get_begin_of_year(double timeDateFactor);
        static double get_begin_of_week(double timeDateFactor);
        static double get_now(void);
        static double get_today(void);
        static double get_current_week_begin(void);
        static double get_current_month_begin(void);
        static double get_current_year_begin(void);
        static double get_timestamp_utc(void);
        static double get_timestamp_local(TimeStampFormat format);
        static double get_timestamp_local(void);
        static double get_timecode_offset_of(int hour, int minute, int seconds, int millis);
        static double get_date_offset_when_no_leap_year(int month, int day, int hour, int minute, int seconds, int millis);
        static double get_utc_timeshift(void);
        static app_err_t format_timecode_date_week_time_ms(double timeCode, char* pszDate, size_t lengthDate, char* pszTime, size_t lengthTime, bool useLocalTime);
        static const char* get_build_date(void);

        std::string to_date(void);
        std::string to_time_hms(void);
        std::string to_time(void);
        std::string to_date_and_time_hms(const char* gap = " - ");
        std::string to_short_time(void);
        std::string to_string();
        std::string format(InFormat fmt);

        void set(Times& givenSource);
        void set_date(int givenYear, int givenMonth, int givenDay);
        void set_time(int givenHour, int givenMinute, int givenSecond);
        void set_build_date(void);
        void clear_time(void);
        void set_time(int givenHour, int givenMinute, int givenSecond, int givenMillis);
        void set_time(long unixTimeMillis);
        void clear(void);
        void set_unix_time_begin(void);
        void set_gps_time(int gpsWeek, int gpsTimeOfWeek);
        void normalize(void);
        double get_day_time(void);
        double get_timecode(void);
        void set_timecode(double factor);
        void set_from_unix_time(long unixTime_ms);
        bool is_later_as(Times& thisOne);
        double frac(double a);
        void set_now_local(void);
        void set_now_utc(void);
        int get_week(void);
        void set_to_begin_of_week(void);
};

