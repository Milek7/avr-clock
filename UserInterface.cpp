#include "UserInterface.hpp"
#include "Display.hpp"
#include "ClockCalendar.hpp"
#include "FontMapping.hpp"
#include "Melody.hpp"

bool UserInterface::UpdateRequested;

void UserInterface::Config()
{
	ClockCalendar::AddEventHandler(Update);
}

void UserInterface::Initialize()
{
	Display::Initialize();
	Display::Clear(0x00);
	Update();
}

void UserInterface::Update()
{
	UpdateRequested = true;
}

void UserInterface::Tick()
{
	if (!UpdateRequested)
		return;
	UpdateRequested = false;

	Display::DrawText(32, 2, ClockCalendar::Date.ToString(), Font_Small, 0x00);
	Display::DrawText(4, 3, ClockCalendar::Time.ToString(), Font_Big, 0x00);

}
