project(SpaceGlider)
add_definitions(-DUNICODE -D_UNICODE)
add_definitions(-D_SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING -D_SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING)
set(zydis_source
	Includes/ZycoreExportConfig.h
	Includes/ZydisExportConfig.h
	Includes/Zydis/src/Decoder.c
	Includes/Zydis/src/DecoderData.c
	Includes/Zydis/src/Formatter.c
	Includes/Zydis/src/FormatterATT.c
	Includes/Zydis/src/FormatterBase.c
	Includes/Zydis/src/FormatterBuffer.c
	Includes/Zydis/src/FormatterIntel.c
	Includes/Zydis/src/MetaInfo.c
	Includes/Zydis/src/Mnemonic.c
	Includes/Zydis/src/Register.c
	Includes/Zydis/src/SharedData.c
	Includes/Zydis/src/String.c
	Includes/Zydis/src/Utils.c
	Includes/Zydis/src/Zydis.c
)
SET(core_sources
	dllmain.cpp 
	Utils/Utils.cpp
	Utils/Utils.h 
	Decrypt/Decrypt.cpp 
	Decrypt/Decrypt.h 
	Patchables/Offsets.h
	Console/Console.cpp
	Console/Console.h
	Hooks/UltimateHooks.cpp
	Hooks/UltimateHooks.h
	Rendering/ImRender.cpp
	Rendering/ImRender.h
	OrbWalker/OrbWalker.cpp
	OrbWalker/OrbWalker.h
	Patchables/Structs.h
	Enums.h
	Vector.h
	DelayedAction/DelayedAction.cpp
	DelayedAction/DelayedAction.h
	ObjectManager/ObjectManager.cpp
	ObjectManager/ObjectManager.h
	LeagueFunctions/LeagueFunctions.cpp
	LeagueFunctions/LeagueFunctions.h
	Hooks/Hooks.cpp
	Hooks/Hooks.h
	Rendering/D3DRenderer.h
	EventManager/EventManager.cpp
	EventManager/EventManager.h
	Evade/Evade.cpp
	Evade/Evade.h
	Geometry/clipper.h
	Geometry/Geometry.h
	XorStr.h
	TargetSelector/TargetSelector.h
	TargetSelector/TargetSelector.cpp
	Plugins/PluginLoader.h
	Plugins/Utilities/WaypointTracker.h
	BuffManager/BuffManager.h
	BuffManager/BuffManager.cpp
	SpellInstance/Spellbase.h
	SpellInstance/Spellbase.cpp
	ConfigManager/Config.cpp
	ConfigManager/Config.h
	ConfigManager/json.hpp
)
add_library(${PROJECT_NAME} SHARED ${core_sources} ${zydis_source})
include_directories($ENV{DXSDK_DIR}/Include)
target_link_directories(${PROJECT_NAME} PUBLIC "$ENV{DXSDK_DIR}/Lib/x86") 
target_link_libraries(${PROJECT_NAME} PUBLIC ImGui MSDetours Dbghelp.lib d3d9.lib d3dx9.lib)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Includes)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Includes/Zydis/src)

foreach(_source IN ITEMS ${core_sources} ${zydis_source})
    get_filename_component(_source_path "${_source}" PATH)
    string(REPLACE "${CMAKE_SOURCE_DIR}" "" _group_path "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_group_path}")
    source_group("${_group_path}" FILES "${_source}")
endforeach()
