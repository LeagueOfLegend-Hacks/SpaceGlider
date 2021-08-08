file(GLOB external_src
     "source/*.h"
     "source/*.cpp"
)
set(CMAKE_CXX_STANDARD 17)
add_executable(external
	${external_src}
)