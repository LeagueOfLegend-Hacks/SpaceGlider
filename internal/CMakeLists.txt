project(SpaceGlider)
add_library(${PROJECT_NAME} SHARED dllmain.cpp)
target_link_libraries(${PROJECT_NAME} PUBLIC ImGui)