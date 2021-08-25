#pragma once
#include <imgui.h>
#include <deque>
#include <string>
#include <Windows.h>

class Menu {
public:
	void Render();
	void Show();
	void Hide();
	void Print(const char* fmt, ...);
private:
	bool ShowConsole = true;
	std::deque<std::string> consoleEvents;
};