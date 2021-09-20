#pragma once
#include "imgui.h"
#include <string>
#include <map>
#include <list>

class IMenuElement
{
public:
	virtual std::string GetKey() = 0;
	virtual std::string GetDisplayName() = 0;
	virtual bool GetVisibilty() = 0;

	virtual bool GetBool() = 0;
	virtual int GetInt() = 0;
	virtual uint32_t GetColor() = 0;
	virtual float GetFloat() = 0;

	virtual void SetDisplayName(std::string const& display_name) = 0;
	virtual void SetVisibilty(bool const& _value) = 0;

	virtual void SetBool(bool const& _value) = 0;
	virtual void SetInt(int const& _value) = 0;
	virtual void SetColor(uint32_t const& _value) = 0;
	virtual void SetFloat(float const _value) = 0;

};

class IMenu
{
public:
	virtual void Remove() = 0;

	virtual IMenuElement* AddSlider(std::string const& display_name, std::string const& key, int default_value, int min_value, int max_value, bool visible_in_menu = true) = 0;
	virtual IMenuElement* AddSliderF(std::string const& display_name, std::string const& key, float default_value, float min_value, float max_value, const char* format = nullptr, bool visible_in_menu = true) = 0;
	virtual IMenuElement* AddCheckBox(std::string const& display_name, std::string const& key, bool default_value, bool visible_in_menu = true) = 0;
	//virtual IMenuElement* AddKeybind(std::string const& display_name, std::string const& key, int vkey, bool default_value, KeybindType type, bool visible_in_menu = true) = 0;
	virtual IMenuElement* AddComboBox(std::string const& display_name, std::string const& key, std::list<std::string> const& names, int default_value, bool visible_in_menu = true) = 0;
	virtual IMenuElement* AddColorPicker(std::string const& display_name, std::string const& key, float r, float g, float b, float a, bool visible_in_menu = true) = 0;
	virtual IMenuElement* AddLabel(std::string const& display_name, std::string const& key, bool visible_in_menu = true) = 0;

	virtual IMenuElement* GetElement(std::string const& key) = 0;
	virtual IMenuElement* operator[](std::string const& key) = 0;

	virtual IMenu* AddSubMenu(std::string const& display_name, std::string const& key) = 0;
	virtual IMenu* GetSubMenu(std::string const& key) = 0;

	virtual std::list<IMenuElement*> GetElements() = 0;
	virtual bool ElementExists(std::string const& key) = 0;
};

class IMainMenu
{
public:
	virtual IMenu* CreateMenu(std::string const& name, std::string const& key) = 0;
	virtual IMenu* GetMenu(std::string const& key) = 0;
	virtual IMenu* operator[](std::string const& key) = 0;

	virtual std::list<IMenu*> GetMainMenus() = 0;
};