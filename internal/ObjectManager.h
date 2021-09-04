#pragma once
#include "Structs.h"
#include "LeagueFunctions.h"
#include <list>
class ObjectManager {
public:
	static GameObject* GetFirstObject()
	{
		typedef GameObject* (__thiscall* fnGetFirst)(void*);
		return ((fnGetFirst)(DEFINE_RVA(Offsets::Functions::GetFirstObject)))(*(void**)(DEFINE_RVA(Offsets::Data::ObjectManager)));
	}
	static GameObject* GetNextObject(GameObject* object)
	{
		typedef GameObject* (__thiscall* fnGetNext)(void*, GameObject*);
		return ((fnGetNext)(DEFINE_RVA(Offsets::Functions::GetNextObject)))(*(void**)(DEFINE_RVA(Offsets::Data::ObjectManager)), object);
	}
	static GameObject* FindObjectByIndex(std::list<GameObject*> heroList, short casterIndex)
	{
		for (GameObject* a : heroList)
		{
			if (casterIndex == a->Index)
				return a;
		}
		return nullptr;
	}
	static std::list<GameObject*> MinionList() {
		std::list<GameObject*> ObjectList;
		auto Object_list = *reinterpret_cast<SEntityList<GameObject>**>(DEFINE_RVA(Offsets::Data::ManagerTemplate_Minions));
		for (size_t i = 0; i < Object_list->size; i++) {
			auto Object = Object_list->entities[i];
			ObjectList.push_back(Object);
		}
		return ObjectList;
	}
	static std::list<GameObject*> AttackableUnitList() {
		std::list<GameObject*> ObjectList;
		auto Object_list = *reinterpret_cast<SEntityList<GameObject>**>(DEFINE_RVA(Offsets::Data::ManagerTemplate_AttackableUnits));
		for (size_t i = 0; i < Object_list->size; i++) {
			auto Object = Object_list->entities[i];
			ObjectList.push_back(Object);
		}
		return ObjectList;
	}
	static std::list<GameObject*> TurretList() {
		std::list<GameObject*> ObjectList;
		auto Object_list = *reinterpret_cast<SEntityList<GameObject>**>(DEFINE_RVA(Offsets::Data::ManagerTemplate_Turrets));
		for (size_t i = 0; i < Object_list->size; i++) {
			auto Object = Object_list->entities[i];
			ObjectList.push_back(Object);
		}
		return ObjectList;
	}
	static std::list<GameObject*> ShopList() {
		std::list<GameObject*> ObjectList;
		auto Object_list = *reinterpret_cast<SEntityList<GameObject>**>(DEFINE_RVA(Offsets::Data::ManagerTemplate_Shops));
		for (size_t i = 0; i < Object_list->size; i++) {
			auto Object = Object_list->entities[i];
			ObjectList.push_back(Object);
		}
		return ObjectList;
	}
	static std::list<GameObject*> InhibitorList() {
		std::list<GameObject*> ObjectList;
		auto Object_list = *reinterpret_cast<SEntityList<GameObject>**>(DEFINE_RVA(Offsets::Data::ManagerTemplate_Inhibitors));
		for (size_t i = 0; i < Object_list->size; i++) {
			auto Object = Object_list->entities[i];
			ObjectList.push_back(Object);
		}
		return ObjectList;
	}
	static std::list<GameObject*> BuildingList() {
		std::list<GameObject*> ObjectList;
		auto Object_list = *reinterpret_cast<SEntityList<GameObject>**>(DEFINE_RVA(Offsets::Data::MangerTemplate_Buildings));
		for (size_t i = 0; i < Object_list->size; i++) {
			auto Object = Object_list->entities[i];
			ObjectList.push_back(Object);
		}
		return ObjectList;
	}
	static std::list<GameObject*> HeroList() {
		std::list<GameObject*> ObjectList;
		auto Object_list = *reinterpret_cast<SEntityList<GameObject>**>(DEFINE_RVA(Offsets::Data::ManagerTemplate_Heros));
		for (size_t i = 0; i < Object_list->size; i++) {
			auto Object = Object_list->entities[i];
			ObjectList.push_back(Object);
		}
		return ObjectList;
	}
	static std::list<GameObject*> MissileList() { // dirty way of getting all the missiles.... FIX THIS ASAP...
		std::list<GameObject*> ObjectList;
		auto Object = GetFirstObject();
		while (Object) {
			if (Functions::IsMissile(Object))
				ObjectList.push_back((GameObject*)Object);
			Object = GetNextObject(Object);
		}
		return ObjectList;
	}
	static GameObject* GetLocalPlayer() {
		return (GameObject*)*(DWORD*)DEFINE_RVA(Offsets::Data::LocalPlayer);
	}
};