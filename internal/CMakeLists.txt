project(SpaceGlider)
add_definitions(-DUNICODE -D_UNICODE)
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
	Utils.cpp
	Utils.h 
	Decrypt.cpp 
	Decrypt.h 
	Offsets.h
	Console.cpp
	Console.h
	UltimateHooks.cpp
	UltimateHooks.h
	ImRender.cpp
	ImRender.h
	OrbWalker.cpp
	OrbWalker.h
	Structs.h
	Enums.h
	Vector.h
	DelayedAction.cpp
	DelayedAction.h
	ObjectManager.cpp
	ObjectManager.h
	LeagueFunctions.h
	Hooks.cpp
	Hooks.h
	D3DRenderer.h
)
add_library(${PROJECT_NAME} SHARED ${core_sources} ${zydis_source})
target_link_libraries(${PROJECT_NAME} PUBLIC ImGui MSDetours Dbghelp.lib)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Includes)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Includes/Zydis/src)