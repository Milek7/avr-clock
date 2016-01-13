#pragma once

extern "C" void TIMER0_OVF_vect(void) __attribute__ ((signal));

class Melody
{
	public:
		static void Config();
		static void Play(char melody);
		static void Stop();
		static void GetMelodyTitle(char melody, char *title);

	private:
		static char TLeft; //remaining time of current played note in 10ms
		static char CurrentNote; //number of current playing note
		static const unsigned int* CurrentMelody; //pointer to array of current played melody

		static void SetFrequency(unsigned int freq);
		friend void TIMER0_OVF_vect(void);
};
