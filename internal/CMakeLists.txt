project(SpaceGlider)
add_definitions(-DUNICODE -D_UNICODE)
add_library(${PROJECT_NAME} SHARED dllmain.cpp Utils.h Decrypt.cpp Decrypt.h Offsets.h)
target_link_libraries(${PROJECT_NAME} PUBLIC ImGui MSDetours Dbghelp.lib)