#pragma once

namespace Offsets {
	enum class Functions {
		GetPing = 0x320610,
		OnProcessSpell = 0x4f8840,
		OnNewPath = 0x27e4c0,
		WorldToScreen = 0x971F30
	};
	enum class Data {
		NetClient = 0x30E223C,
		LocalPlayer = 0x30DA914,
		GameTime = 0x30d2c58,
	};
}