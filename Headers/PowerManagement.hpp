#pragma once

#include <avr/sleep.h>

#define DEFAULT_SLEEPMODE Powersave
#define MODULE_COUNT 4

class PowerManagement
{
	public:
		enum SleepMode
		{
			Powersave = 0,
			Idle = 1
		};
		static void Config();
		static void SetMaximumSleepMode(char id, SleepMode mode);
		static void Sleep();

	private:
		static const char SleepModeMap[];
		static SleepMode MaximumSleepMode[MODULE_COUNT];
};
