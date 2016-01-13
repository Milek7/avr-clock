#include <avr/sleep.h>
#include "PowerManagement.hpp"

PowerManagement::SleepMode PowerManagement::MaximumSleepMode[MODULE_COUNT] = {DEFAULT_SLEEPMODE};
const char PowerManagement::SleepModeMap[] = {SLEEP_MODE_PWR_SAVE, SLEEP_MODE_IDLE};

void PowerManagement::Config()
{
	ACSR |= (1 << ACD);
}

void PowerManagement::SetMaximumSleepMode(char id, SleepMode mode)
{
	MaximumSleepMode[id] = mode;

	SleepMode maxlvl = (SleepMode)0;
	for (char i = 0; i < MODULE_COUNT; i++)
		if (MaximumSleepMode[i] > maxlvl)
			maxlvl = MaximumSleepMode[i];

	set_sleep_mode(SleepModeMap[maxlvl]);
}

void PowerManagement::Sleep()
{
 	sleep_mode();
}
