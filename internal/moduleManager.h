#pragma once
#include <imgui.h>

class IAbstractModuleBase 
public:
    IAbstractModuleBase() {}
    virtual ~IAbstractModuleBase() {}

    virtual void onLoad()   = 0;
    virtual void onRender() = 0;
    virtual void onDraw() = 0;
    virtual void onUpdate() = 0;
};
