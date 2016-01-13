#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/pgmspace.h>

#include "Melody.hpp"
#include "MelodyBank.hpp"
#include "PowerManagement.hpp"

#define MODULE_ID 3

//helper macros
#define MelodyArray(index) pgm_read_word(&(Melody::CurrentMelody[index]))
#define MelodyTitleArray(index) pgm_read_byte(&(MelodyTitle[index]))

char Melody::TLeft;
char Melody::CurrentNote;
const unsigned int* Melody::CurrentMelody;

void Melody::Config()
{
	DDRB |= (1<<PB1);
	DDRB |= (1<<PB2);

	TCCR1A = 0b10110010; //timer1 configuration: mode 14, fast pwm, oc1a - non-interverting mode, oc1b - inverting mode
	TCCR1B = 0b00011000;

	TIMSK0 |= (1<<TOIE0); //timer0 overflow interrupt

	PowerManagement::SetMaximumSleepMode(MODULE_ID, PowerManagement::Powersave);
}

void Melody::Stop()
{
	TCCR1B = 0b00011000; //stop timer1
	TCCR0B = 0b00000000; //stop timer0
	PowerManagement::SetMaximumSleepMode(MODULE_ID, PowerManagement::Powersave);
}

void Melody::Play(char melody)
{
	PowerManagement::SetMaximumSleepMode(MODULE_ID, PowerManagement::Idle);
	TCNT0 = 100; //timer0 starting value
	TCCR0B = 0b00000011; //timer0, prescaler 64
	TCCR1B = 0b00011001; //timer1, no prescaler
	CurrentMelody = MelodyBank[melody]; //save pointer to current played melody
	CurrentNote = 0;
	TLeft = MelodyArray(CurrentNote*2+1)/10;
	Melody::SetFrequency(MelodyArray(CurrentNote*2));
}

void Melody::SetFrequency(unsigned int freq)
{
	ICR1 = 1000000/freq;
	OCR1A = ICR1>>1;
	OCR1B = OCR1A;
}

void Melody::GetMelodyTitle(char melody, char *title)
{
	unsigned char i = 0;
	const char* MelodyTitle = MelodyTitles[melody];
	while ((title[i]=MelodyTitleArray(i)))
		i++;
}

ISR(TIMER0_OVF_vect) //timer0 overflow interrupt - about every 10ms
{
	TCNT0 = 100; //timer0 starting value
	Melody::TLeft -= 1;
	if (Melody::TLeft == 0)
	{
		Melody::CurrentNote++; //jump to next note
		if (MelodyArray(Melody::CurrentNote*2) == 0) //if melody ended, play from beginning
			Melody::CurrentNote = 0;
		Melody::TLeft = MelodyArray(Melody::CurrentNote*2+1)/10;
		Melody::SetFrequency(MelodyArray(Melody::CurrentNote*2));
	}
}
