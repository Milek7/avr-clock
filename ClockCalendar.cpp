#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "ClockCalendar.hpp"
#include "PowerManagement.hpp"
#include "Display.hpp"
#define MODULE_ID 1

ClockCalendar::TimeStruct ClockCalendar::Time;
ClockCalendar::DateStruct ClockCalendar::Date;
void (*ClockCalendar::EventHandlers[])(void);

void ClockCalendar::Config()
{
	TCCR2B |= (1<<CS22) | (1<<CS20) /*| (1<<CS21)*/; //timer2 config
	ASSR |= (1<<AS2); //asynchronus mode
	TIMSK2 |= (1<<TOIE2); //timer2 overflow interrupt
	PowerManagement::SetMaximumSleepMode(MODULE_ID, PowerManagement::Powersave);

	Date = DateStruct(2000, 1, 1);
	Time = TimeStruct(12, 0, 0);
}

char* ClockCalendar::TimeStruct::ToString()
{
	static char buffer[9];
	if (Hour < 10)
	{
		buffer[0] = '0';
		itoa(Hour, &buffer[1], 10);
	}
	else
		itoa(Hour, &buffer[0], 10);

	if (Minute < 10)
	{
		buffer[3] = '0';
		itoa(Minute, &buffer[4], 10);
	}
	else
		itoa(Minute, &buffer[3], 10);

	if (Second < 10)
	{
		buffer[6] = '0';
		itoa(Second, &buffer[7], 10);
	}
	else
		itoa(Second, &buffer[6], 10);

	buffer[2] = ':';
	buffer[5] = ':';
	return buffer;
}

char* ClockCalendar::DateStruct::ToString()
{
	static char buffer[10];

	if (Day < 10)
	{
		buffer[0] = '0';
		itoa(Day, &buffer[1], 10);
	}
	else
		itoa(Day, &buffer[0], 10);

	if (Month < 10)
	{
		buffer[3] = '0';
		itoa(Month, &buffer[4], 10);
	}
	else
		itoa(Month, &buffer[3], 10);

	itoa(Year, &buffer[6], 10);

	buffer[2] = '-';
	buffer[5] = '-';

	return buffer;
}

bool ClockCalendar::AddEventHandler(void (*handler)(void))
{
	char i = 0;
	while(EventHandlers[i++] != 0);
	if (i > MAX_HANDLERS_COUNT)
		return false;
	EventHandlers[i - 1] = handler;
	return true;
}

void ClockCalendar::RemoveEventHandler(void (*handler)(void))
{
	char i = 0;
	while(EventHandlers[i++] != handler);
	if (i > MAX_HANDLERS_COUNT)
		return;
	EventHandlers[i - 1] = 0;
}

ISR(TIMER2_OVF_vect) //timer2 overflow interrupt - every 8s
{
	ClockCalendar::TimeStruct* Time = &ClockCalendar::Time;
	ClockCalendar::DateStruct* Date = &ClockCalendar::Date;

	Time->Second += 1;

	if (Time->Second > 59)
	{
		Time->Minute++;
		Time->Second -= 60;
	
		if (Time->Minute == 60)
		{
			Time->Hour++;
			Time->Minute = 0;
		
			if (Time->Hour == 24)
			{
				Date->Day++;
				Time->Hour = 0;

				if (Date->Day > Date->DaysInMonth())
				{
					Date->Day = 1;
					Date->Month++;
					if (Date->Month > 12)
					{
						Date->Month = 1;
						Date->Year++;
					}
				}
			}
		}
	}

	char i = 0;
	while(ClockCalendar::EventHandlers[i++] != 0)
	{
		if (i > MAX_HANDLERS_COUNT)
			break;
		ClockCalendar::EventHandlers[i - 1]();
	}
}

ClockCalendar::DateStruct::DateStruct()
{
}

ClockCalendar::DateStruct::DateStruct(unsigned int year, char month, char day)
{
	Year = year;
	Month = month;
	Day = day;
}

ClockCalendar::TimeStruct::TimeStruct()
{
}

ClockCalendar::TimeStruct::TimeStruct(char hour, char minute, char second)
{
	Hour = hour;
	Minute = minute;
	Second = second;
}

bool ClockCalendar::DateStruct::IsLeapYear()
{
	bool leap = false;

	if (Year % 4 == 0)
	{
		leap = true;
		if (Year % 100 == 0 && Year % 400 != 0)
			leap = false;
	}

	return leap;
}

char ClockCalendar::DateStruct::DayOfYear()
{
	char dayofyear = 0;

	DateStruct date = DateStruct(Year, 0, 0);
	for (int i = 1; i < Month; i++)
	{
		date.Month = i;
		dayofyear += date.DaysInMonth();
	}

	dayofyear += Day;

	return dayofyear;
}

char ClockCalendar::DateStruct::DaysInMonth()
{
	char daysinmonth = 31;

	if (Month == 2)
	{
		if (IsLeapYear())
			daysinmonth = 29;
		else
			daysinmonth = 28;
	}
	else if (Month > 7)
		daysinmonth -= (Month - 2) % 2;
	else
		daysinmonth -= (Month - 1) % 2;
	
	return daysinmonth;
}

char ClockCalendar::DateStruct::DayOfWeek()
{
	char dayofweek = (Day+=Month<3?Year--:Year-2,23*Month/9+Day+4+Year/4-Year/100+Year/400)%7;
	if (dayofweek == 0)
		dayofweek = 7;
	return dayofweek;
}
