#pragma once

class UserInterface
{
	public:
		static void Config();
		static void Initialize();
		static void Update();
		static void Tick();

	private:
		static bool UpdateRequested;
};
