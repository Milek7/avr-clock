#include <avr/io.h>
#include <avr/interrupt.h>

#include "Display.hpp"
#include "Melody.hpp"
#include "ClockCalendar.hpp"
#include "PowerManagement.hpp"
#include "UserInterface.hpp"

#define MODULE_ID 0

int main()
{
	PowerManagement::Config();
	Display::Config();
	Melody::Config();
	ClockCalendar::Config();
	UserInterface::Config();

	sei();

	PowerManagement::SetMaximumSleepMode(MODULE_ID, PowerManagement::Powersave);

	UserInterface::Initialize();

	while(true)
	{
		UserInterface::Tick();
		PowerManagement::Sleep();
	}
}
