#pragma once
#include <Windows.h>
#include "../Vector.h"
#include "../Utils/Utils.h"
#include "../Patchables/Offsets.h"

class D3DRenderer {
public:
	int GetWidth();
	int GetHeight();
	float* GetViewMatrix();
	float* GetProjectionMatrix();
	void MultiplyMatrices(float* out, float* a, float* b);
	Vector2 WorldToScreen(const Vector3& pos);
};
extern D3DRenderer* riot_render;