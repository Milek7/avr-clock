#pragma once

class Display
{
	public:
		static void Config();
		static void Initialize();
		static void Clear(char data);
		static void DrawText(char xpos, char ypos, char string[], const char* const* font, char invert);
		static char CalcTextLen(char string[], const char* const* font);
		static void DrawImage(char xpos, char ypos, const char* image, char invert);

	private:
		static void SendCommand(char cmd);
		static void SendData(char dat);
};
