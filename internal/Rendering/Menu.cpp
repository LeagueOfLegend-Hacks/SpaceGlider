#include "Menu.h"

IMenuElement* IMenu::AddLabel(std::string const& display_name, std::string const& key, bool visible_in_menu)
{
	if (visible_in_menu)
		ImGui::Text("asdf");
	return nullptr;
}