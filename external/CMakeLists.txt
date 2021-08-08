INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )
add_executable(external
	main.cpp
)
TARGET_LINK_LIBRARIES( external LINK_PUBLIC ${Boost_LIBRARIES} )