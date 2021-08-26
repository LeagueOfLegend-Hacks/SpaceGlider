#include "ImRender.h"
void ImRender::Init(LPDIRECT3DDEVICE9 Device) {
	if (IsInitalized)
		return;
	if (!Device)
		return;
	WorldToScreen = (fnWorldToScreen)(DEFINE_RVA(Offsets::Functions::WorldToScreen));
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(GetHwndProc());
	ImGui_ImplDX9_Init(Device);
	IsInitalized = true;
}
void ImRender::Free() {
	if (!IsInitalized)
		return;
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
void ImRender::begin_draw() {
		if (!IsInitalized)
		return;
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();

	ImGui::NewFrame();

	const auto flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs;

	ImGui::GetStyle().AntiAliasedFill = true;
	ImGui::GetStyle().AntiAliasedLines = true;

	ImGui::Begin("##overlay", nullptr, flags);

	ImGui::SetWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Always);
	ImGui::SetWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y), ImGuiCond_Always);

	m_width = ImGui::GetIO().DisplaySize.x;
	m_height = ImGui::GetIO().DisplaySize.y;
}
void ImRender::end_draw(void)
{
	if (!IsInitalized)
		return;
	ImGui::GetForegroundDrawList()->PushClipRectFullScreen();

	ImGui::End();

	ImGui::EndFrame();

	ImGui::Render();

	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}
void ImRender::draw_text(ImVec2 screen_pos, const char* text, bool outlined, ImColor color, ImFont* font)
{
	if (!IsInitalized)
		return;

	if (font == nullptr) {
		if (outlined) {
			ImGui::GetWindowDrawList()->AddText(ImVec2(screen_pos.x, screen_pos.y + 1.0f), ImColor(0, 0, 0, 255), text);
			ImGui::GetWindowDrawList()->AddText(ImVec2(screen_pos.x + 1.0f, screen_pos.y), ImColor(0, 0, 0, 255), text);
		}
		ImGui::GetWindowDrawList()->AddText(screen_pos, color, text);
	} else {
		if (outlined)
		{
			ImGui::GetWindowDrawList()->AddText(font, font->FontSize, ImVec2(screen_pos.x, screen_pos.y + 1.0f), ImColor(0, 0, 0, 255), text);
			ImGui::GetWindowDrawList()->AddText(font, font->FontSize, ImVec2(screen_pos.x + 1.0f, screen_pos.y), ImColor(0, 0, 0, 255), text);
		}

		ImGui::GetWindowDrawList()->AddText(font, font->FontSize, screen_pos, color, text);
	}
}
void ImRender::draw_line(ImVec2 start_pos, ImVec2 end_pos, ImColor color, float thickness)
{
	ImGui::GetWindowDrawList()->AddLine(start_pos, end_pos, color, thickness);
}
void ImRender::draw_rect(ImRect screen_pos, ImColor color, DrawType type, float rounding, unsigned short points, float thickness)
{
	switch (type)
	{
	case DrawType::Normal:
		ImGui::GetWindowDrawList()->AddRect(ImVec2(screen_pos.x, screen_pos.y), ImVec2(screen_pos.x + screen_pos.width, screen_pos.y + screen_pos.height), color, rounding, points, thickness);
		break;
	case DrawType::Filled:
		ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(screen_pos.x, screen_pos.y), ImVec2(screen_pos.x + screen_pos.width, screen_pos.y + screen_pos.height), color, rounding, points);
		break;
	default:
		break;
	}
}
void ImRender::draw_triangle(ImVec2 pos_one, ImVec2 pos_two, ImVec2 pos_three, ImColor color, DrawType type, float thickness)
{
	if (type == DrawType::Normal)
		ImGui::GetWindowDrawList()->AddTriangle(pos_one, pos_two, pos_three, color, thickness);
	else
		ImGui::GetWindowDrawList()->AddTriangleFilled(pos_one, pos_two, pos_three, color);
}
void ImRender::draw_circle(ImVec2 center_pos, float radius, ImColor color, DrawType type, unsigned short points, float thickness)
{
	switch (type) {
	case DrawType::Normal:
	{
		ImGui::GetWindowDrawList()->AddCircle(center_pos, radius, color, points, thickness);
		break;
	}
	case DrawType::Filled:
	{
		ImGui::GetWindowDrawList()->AddCircleFilled(center_pos, radius, color, points);
		break;
	}
	}
}
void ImRender::draw_circle(ImVec3 screen_pos, float radius, ImColor color, DrawType type, float thickness)
{
	if ((color & IM_COL32_A_MASK) == 0)
		return;
	ImVec3 world_pos = screen_pos;
	ImVec3 pos;
	for (auto i = 0; i <= 99; i++) {
		auto angle = (float)i * IM_PI * 1.98f / 98.0f;
		world_pos.x = screen_pos.x + ImCos(angle) * radius;
		world_pos.z = screen_pos.z + ImSin(angle) * radius;
		WorldToScreen(&world_pos, &pos);
		ImGui::GetWindowDrawList()->PathLineTo(ImVec2(pos.x, pos.y));
	}
	if (type == DrawType::Normal)
		ImGui::GetWindowDrawList()->PathStroke(color, 0, thickness);
	else
		ImGui::GetWindowDrawList()->PathFillConvex(color);
}