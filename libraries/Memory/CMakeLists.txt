project(MemoryReader)
SET(mem_reader_sources
	Memory.h
	Memory.cpp
)
add_library(${PROJECT_NAME} STATIC ${mem_reader_sources})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})