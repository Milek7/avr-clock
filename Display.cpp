#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/pgmspace.h>

#include "Display.hpp"
#include "PowerManagement.hpp"
#include "FontMapping.hpp"

#define MODULE_ID 2
#define Image(index) pgm_read_byte(&(image[index]))

void Display::SendCommand(char cmd)
{
	PORTD &= ~_BV(1); //a0 low
	SPDR = cmd;
	PowerManagement::Sleep();
}

void Display::SendData(char dat)
{
	PORTD |= _BV(1); //a0 high
	SPDR = dat;
	PowerManagement::Sleep();
}

void Display::Config()
{
	DDRB |= (1<<PB5) | (1<<PB3) | (1<<PB2);	//spi pins configuration
	DDRB |= (1<<PB0); //reset
	DDRD |= (1<<PD1); //a0
	SPCR = (1<<SPE) | (1<<MSTR) | (1<<SPIE); //enable SPI, divider OSC/4, interrupt
	SPSR = (1<<SPI2X); //double speed (OSC/2)
	PowerManagement::SetMaximumSleepMode(MODULE_ID, PowerManagement::Powersave);
}

void Display::DrawText(char xpos, char ypos, char string[], const char* const* font, char invert)
{
	for (char i = 0; string[i] != 0; i++)
	{
		const char* image = (const char*)pgm_read_word(&(font[string[i]-32]));
		Display::DrawImage(xpos, ypos, image, invert);
		xpos += Image(0);
	}
}

char Display::CalcTextLen(char string[], const char* const* font)
{
	char xpos = 0;
	for (char i = 0; string[i] != 0; i++)
	{
		const char* image = (const char*)pgm_read_word(&(font[string[i]-32]));
		xpos += Image(0);
	}
	return xpos;
}

void Display::DrawImage(char xpos, char ypos, const char* image, char invert)
{
	PowerManagement::SetMaximumSleepMode(MODULE_ID, PowerManagement::Idle);

	for (char j = 0; j < Image(1); j++)
	{
		Display::SendCommand(0b10110000+(7-(ypos+j)));
		Display::SendCommand(0b00000000+(xpos & 0x0F));
		Display::SendCommand(0b00010000+(xpos >> 4));

		char end = Image(0)+2+j*Image(0);
		for (char i = end-Image(0); i < end; i++)
		{
			Display::SendData(Image(i) ^ invert);
		}
	}

	PowerManagement::SetMaximumSleepMode(MODULE_ID, PowerManagement::Powersave);
}

void Display::Clear(char data)
{
	PowerManagement::SetMaximumSleepMode(MODULE_ID, PowerManagement::Idle);

	for (char ypos = 0; ypos < 8; ypos++)
	{
		Display::SendCommand(0b10110000+(7-ypos));
		Display::SendCommand(0b00000000);
		Display::SendCommand(0b00010000);
		for (char xpos = 0; xpos < 128; xpos++)
			Display::SendData(data);
	}

	PowerManagement::SetMaximumSleepMode(MODULE_ID, PowerManagement::Powersave);
}

void Display::Initialize()
{
	PORTB &= ~_BV(0);
	asm("nop");
	PORTB |= _BV(0); //display reset

	PowerManagement::SetMaximumSleepMode(MODULE_ID, PowerManagement::Idle);

	Display::SendCommand(0b10100011);	// Bias 1/7
	Display::SendCommand(0b10100001);	// ADC reverse
	Display::SendCommand(0b11001000);	// Reverse scan
	Display::SendCommand(0b00101111);	// Power control
	Display::SendCommand(0b10000001); 	// Electronic volume
	Display::SendCommand(35);		// Electronic volume
	Display::SendCommand(0b00100011);	// Resistor ratio
	Display::SendCommand(0b10101111);	// Display ON

	PowerManagement::SetMaximumSleepMode(MODULE_ID, PowerManagement::Powersave);
}


EMPTY_INTERRUPT(SPI_STC_vect);

