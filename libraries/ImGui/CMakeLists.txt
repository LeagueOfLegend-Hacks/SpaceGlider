project(ImGui)

SET(imgui_sources
	imconfig.h
	imgui.cpp
	imgui.h
	imgui_draw.cpp
	imgui_impl_dx9.cpp
	imgui_impl_dx9.h
	imgui_impl_win32.cpp
	imgui_impl_win32.h
	imgui_internal.h
	imgui_tables.cpp
	imgui_widgets.cpp
	imstb_rectpack.h
	imstb_textedit.h
	imstb_truetype.h
)

add_library(${PROJECT_NAME} STATIC ${imgui_sources})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})