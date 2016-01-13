#pragma once

#define MAX_HANDLERS_COUNT 1

extern "C" void TIMER2_OVF_vect(void) __attribute__ ((signal));

class ClockCalendar
{
	public:
		struct TimeStruct
		{
			volatile char Hour;
			volatile char Minute;
			volatile char Second;

			TimeStruct();
			TimeStruct(char hour, char minute, char second);
			char* ToString();
		};

		struct DateStruct
		{
			volatile unsigned int Year;
			volatile char Month;
			volatile char Day;

			DateStruct();
			DateStruct(unsigned int year, char month, char day);
			bool IsLeapYear();
			char DayOfYear();
			char DayOfWeek();
			char DaysInMonth();

			char* ToString();
		};

		static TimeStruct Time;
		static DateStruct Date;

		static void Config();
		static bool AddEventHandler(void (*handler)(void));
		static void RemoveEventHandler(void (*handler)(void));

	private:
		friend void TIMER2_OVF_vect(void);
		static void (*EventHandlers[MAX_HANDLERS_COUNT])(void);
};
