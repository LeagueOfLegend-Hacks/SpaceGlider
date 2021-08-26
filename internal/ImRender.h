#pragma once
#include "Offsets.h"
#include "Utils.h"
#include <d3d9.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_dx9.h>
#include <imgui_impl_win32.h>
#include <vector>

#pragma comment(lib, "d3d9.lib")

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class ImRender {
public:
	enum class DrawType {
		Normal,
		Filled
	};
	struct ImRect : ImVec2 {
		float width;
		float height;
	};
	struct ImVec3 : ImVec2 {
		float z;
	};
	void Init(LPDIRECT3DDEVICE9 Device);
	void Free();
	void begin_draw();
	void end_draw();
	void draw_text(ImVec2 screen_pos, const char* text, bool outlined = false, ImColor color = ImColor(255,255,255,255) , ImFont* font = nullptr);
	void draw_line(ImVec2 start_pos, ImVec2 end_pos, ImColor color = ImColor(255, 255, 255, 255), float thickness = 1.0f);
	void draw_rect(ImRect screen_pos, ImColor color = ImColor(255, 255, 255, 255), DrawType type = DrawType::Normal, float rounding = 0.0f, unsigned short points = 12, float thickness = 1.0f);
	void draw_triangle(ImVec2 pos_one, ImVec2 pos_two, ImVec2 pos_three, ImColor color = ImColor(255,255,255,255), DrawType type = DrawType::Normal, float thickness = 1.0f);
	void draw_circle(ImVec2 screen_pos, float radius, ImColor color = ImColor(255, 255, 255, 255), DrawType type = DrawType::Normal, unsigned short points = 12, float thickness= 1.0f);
	void draw_circle(ImVec3 screen_pos, float radius, ImColor color = ImColor(255, 255, 255, 255), DrawType type = DrawType::Normal, float thickness= 1.0f);
private:
	bool IsInitalized = false;
	float m_width = 0.0f;
	float m_height = 0.0f;
	typedef bool(__cdecl* fnWorldToScreen)(ImVec3* vectorIn, ImVec3* vectorOut);
	fnWorldToScreen WorldToScreen;
};