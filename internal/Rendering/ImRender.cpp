#include "ImRender.h"
bool ImRender::LoadTextureFromFile(LPDIRECT3DDEVICE9 device, LPCSTR filename, PDIRECT3DTEXTURE9* out_texture)
{
	PDIRECT3DTEXTURE9 texture;
	HRESULT hr = D3DXCreateTextureFromFileA(device, filename, &texture);
	if (hr != S_OK)
		return false;
	*out_texture = texture;
	return true;
}
void ImRender::LoadIcons(LPDIRECT3DDEVICE9 device, std::string& path)
{
	std::string folder(path);
	WIN32_FIND_DATAA findData;
	HANDLE hFind;

	int nrFiles = std::distance(std::experimental::filesystem::directory_iterator(path), std::experimental::filesystem::directory_iterator());
	int nrFile = 0;
	hFind = FindFirstFileA((folder + "\\*.png").c_str(), &findData);
	do {
		if (hFind != INVALID_HANDLE_VALUE) {

			std::string filePath = folder + "/" + findData.cFileName;
			PDIRECT3DTEXTURE9 loadtexture;
			bool image = LoadTextureFromFile(device, filePath.c_str(), &loadtexture);
			if (image)
			{
				std::string fileName(findData.cFileName);
				fileName.erase(fileName.find(".png"), 4);
				Images[fileName] = loadtexture;
			}
		}
		nrFile++;
	} while (FindNextFileA(hFind, &findData));
}

void ImRender::Init(LPDIRECT3DDEVICE9 Device) {
	if (IsInitalized)
		return;
	if (!Device)
		return;
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(GetHwndProc());
	ImGui_ImplDX9_Init(Device);
	IsInitalized = true;
	riot_render = (D3DRenderer*)*(DWORD*)DEFINE_RVA(Offsets::Data::D3DRender);

	std::string wards = std::string("C:/SpaceGlider/Sprites/Wards");
	std::string ui = std::string("C:/SpaceGlider/Sprites/UI");
	std::string spells = std::string("C:/SpaceGlider/Sprites/Spells");

	LoadIcons(Device, wards);
	LoadIcons(Device, ui);
	LoadIcons(Device, spells);
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

	if (ImGui::Begin("SpaceGlider", nullptr))
	{
		ImGui::End();
	}

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
void ImRender::draw_text(Vector2 screen_pos, const char* text, bool outlined, ImColor color, ImFont* font)
{
	if (!IsInitalized)
		return;

	if (font == nullptr) {
		if (outlined) {
			ImGui::GetWindowDrawList()->AddText(ImVec2(screen_pos.x, screen_pos.y + 1.0f), ImColor(0, 0, 0, 255), text);
			ImGui::GetWindowDrawList()->AddText(ImVec2(screen_pos.x + 1.0f, screen_pos.y), ImColor(0, 0, 0, 255), text);
		}
		ImGui::GetWindowDrawList()->AddText(ImVec2(screen_pos.x, screen_pos.y), color, text);
	} else {
		if (outlined)
		{
			ImGui::GetWindowDrawList()->AddText(font, font->FontSize, ImVec2(screen_pos.x, screen_pos.y + 1.0f), ImColor(0, 0, 0, 255), text);
			ImGui::GetWindowDrawList()->AddText(font, font->FontSize, ImVec2(screen_pos.x + 1.0f, screen_pos.y), ImColor(0, 0, 0, 255), text);
		}

		ImGui::GetWindowDrawList()->AddText(font, font->FontSize, ImVec2(screen_pos.x, screen_pos.y), color, text);
	}
}
void ImRender::draw_line(Vector2 start_pos, Vector2 end_pos, ImColor color, float thickness)
{
	ImGui::GetWindowDrawList()->AddLine(ImVec2(start_pos.x, start_pos.y), ImVec2(end_pos.x, end_pos.y), color, thickness);
}
void ImRender::draw_line(Vector3 start_pos, Vector3 end_pos, ImColor color, float thickness)
{
	Vector2 startWorldPos = riot_render->WorldToScreen(start_pos);
	Vector2 endWorldPos = riot_render->WorldToScreen(end_pos);
	ImGui::GetWindowDrawList()->AddLine(ImVec2(startWorldPos.x, startWorldPos.y), ImVec2(endWorldPos.x, endWorldPos.y), color, thickness);
}
void ImRender::draw_line3D(Vector3 start_pos, Vector3 end_pos, ImColor color, float thickness)
{
	Vector2 startWorldPos = riot_render->WorldToScreen(start_pos);
	Vector2 endWorldPos = riot_render->WorldToScreen(end_pos);

	ImGui::GetWindowDrawList()->AddLine(ImVec2(startWorldPos.x, endWorldPos.y), ImVec2(endWorldPos.x, endWorldPos.y), color, thickness);
}

void ImRender::rect(Vector3 start, Vector3 end, float radius, float width, ImColor color) {
	Vector3 vector = (end - start).Normalized();
	Vector3 value = vector.Perpendicular();
	Vector3 point2 = start + value * radius;
	Vector3 point3 = start - value * radius;
	Vector3 point4 = end + value * radius;
	Vector3 point5 = end - value * radius;

	draw_line(point2, point4, color, width);
	draw_line(point3, point5, color, width);
	draw_line(point2, point3, color, width);
	draw_line(point5, point4, color, width);
}

void ImRender::polygon(Geometry::Polygon poly, ImColor color, float tick)
{
	ImVec2 points[200];
	int i = 0;
	for (auto& point : poly.Points)
	{
		Vector2 screenSpace = riot_render->WorldToScreen(point);
		points[i].x = screenSpace.x;
		points[i].y = screenSpace.y;
		i++;
	}

	ImGui::GetWindowDrawList()->AddPolyline(points, i, color, true, tick);
}

/*void ImRender::draw_rect(Vector4 screen_pos, ImColor color, DrawType type, float rounding, unsigned short points, float thickness)
{
	switch (type)
	{
	case DrawType::Normal:
		ImGui::GetWindowDrawList()->AddRect(ImVec2(screen_pos.x, screen_pos.y), ImVec2(screen_pos.x + screen_pos.w, screen_pos.y + screen_pos.z), color, rounding, points, thickness);
		break;
	case DrawType::Filled:
		ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(screen_pos.x, screen_pos.y), ImVec2(screen_pos.x + screen_pos.w, screen_pos.y + screen_pos.z), color, rounding, points);
		break;
	default:
		break;
	}
}
*/
void ImRender::draw_triangle(Vector2 pos_one, Vector2 pos_two, Vector2 pos_three, ImColor color, DrawType type, float thickness)
{
	if (type == DrawType::Normal)
		ImGui::GetWindowDrawList()->AddTriangle(ImVec2(pos_one.x, pos_one.y), ImVec2(pos_two.x, pos_two.y), ImVec2(pos_three.x, pos_three.y), color, thickness);
	else
		ImGui::GetWindowDrawList()->AddTriangleFilled(ImVec2(pos_one.x, pos_one.y), ImVec2(pos_two.x, pos_two.y), ImVec2(pos_three.x, pos_three.y), color);
}
void ImRender::draw_circle(Vector2 center_pos, float radius, ImColor color, DrawType type, unsigned short points, float thickness)
{
	switch (type) {
	case DrawType::Normal:
	{
		ImGui::GetWindowDrawList()->AddCircle(ImVec2(center_pos.x, center_pos.y), radius, color, points, thickness);
		break;
	}
	case DrawType::Filled:
	{
		ImGui::GetWindowDrawList()->AddCircleFilled(ImVec2(center_pos.x, center_pos.y), radius, color, points);
		break;
	}
	}
}
void ImRender::draw_circle(Vector3 screen_pos, float radius, ImColor color, DrawType type, float thickness)
{
	if ((color & IM_COL32_A_MASK) == 0)
		return;
	Vector3 world_pos(screen_pos.x, screen_pos.y, screen_pos.z);
	for (auto i = 0; i <= 99; i++) {
		auto angle = (float)i * IM_PI * 1.98f / 98.0f;
		world_pos.x = screen_pos.x + ImCos(angle) * radius;
		world_pos.z = screen_pos.z + ImSin(angle) * radius;
		Vector2 pos = riot_render->WorldToScreen(world_pos);
		ImGui::GetWindowDrawList()->PathLineTo(ImVec2(pos.x, pos.y));
	}
	if (type == DrawType::Normal)
		ImGui::GetWindowDrawList()->PathStroke(color, 0, thickness);
	else
		ImGui::GetWindowDrawList()->PathFillConvex(color);
}

void ImRender::draw_image(const char* img, Vector3 pos, Vector2 body, ImColor color)
{
	Vector2 worldPos = riot_render->WorldToScreen(pos);
	ImVec2 zero = ImVec2(0.f, 0.f);
    ImVec2 one = ImVec2(1.f, 1.f);

	auto it = Images.find(std::string(img));
	if (it == Images.end())
		return;
	ImGui::GetWindowDrawList()->AddImage(it->second, ImVec2(worldPos.x, worldPos.y), ImVec2(body.x + worldPos.x, body.y + worldPos.y), zero, one, color);
}


void ImRender::draw_image(const char* img, Vector2 pos, Vector2 body, ImColor color)
{
	ImVec2 zero = ImVec2(0.f, 0.f);
	ImVec2 one = ImVec2(1.f, 1.f);

	auto it = Images.find(std::string(img));
	if (it == Images.end())
		return;
	ImGui::GetWindowDrawList()->AddImage(it->second, ImVec2(pos.x, pos.y), ImVec2(body.x + pos.x, body.y + pos.y), zero, one, color);
}