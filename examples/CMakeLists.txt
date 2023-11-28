 set(EXAMPLES
	# none yet
)

foreach(X IN LISTS EXAMPLES)
	add_executable(${X})
	target_setup(${X})
	target_sources(${X} PRIVATE ${X}.cpp)
	target_include_directories(${X} PUBLIC 
		$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/external/parser/src>)	
	target_link_libraries(${X} TAUs)
endforeach()
