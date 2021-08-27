#pragma once

namespace Offsets {
	enum class Functions {
		GetPing = 0x320610,
		OnProcessSpell = 0x4f8840,
		OnNewPath = 0x27e4c0,
		WorldToScreen = 0x971F30,
		GetFirstObject = 0x278690,
		GetNextObject = 0x279090,
		IsAlive = 0x194180,
		IsHero = 0x1A7080,
		IsMissile = 0x1a70e0,
		IsMinion = 0x1a70c0,
		IsInhib = 0x1a6f00,
		IsBaron = 0x165B20,
		IsNexus = 0x1a7000,
		IsTurret = 0x1a6ec0,
		GetAttackDelay = 0x276090,
		GetAttackCastDelay = 0x275f90,
		OnCreateObject = 0x27E1E0,	//0xAE
		OnDeleteObject = 0x276BF0,	//0x151
		OnFinishCast = 0x4EAB00
	};
	enum class Data {
		NetClient = 0x30E223C,
		LocalPlayer = 0x30DA914,
		GameTime = 0x30d2c58,
		ObjectManager = 0x183E1A0,
		ManagerTemplate_AIHero_ = 0x183C1A4,
		HudInstance = 0x183e1dc,
	};
}