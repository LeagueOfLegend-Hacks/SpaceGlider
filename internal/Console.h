#pragma once
#include <imgui.h>
#include <deque>
#include <string>
#include <Windows.h>

class Console {
public:
	void Render();
	void Show();
	void Hide();
	void Print(const char* fmt, ...);
private:
	bool ShowConsole = true;
	std::deque<std::string> consoleEvents;
};
extern Console console;