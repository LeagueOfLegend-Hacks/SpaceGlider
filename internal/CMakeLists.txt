project(SpaceGlider)
add_library(${PROJECT_NAME} SHARED dllmain.cpp Utils.h)
target_link_libraries(${PROJECT_NAME} PUBLIC ImGui MSDetours)