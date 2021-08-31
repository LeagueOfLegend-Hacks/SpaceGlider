project(ProcessDumper)
add_definitions(-DUNICODE -D_UNICODE)
SET(process_dumper_src
	DynArray.h
	close_watcher.cpp
	close_watcher.h
	dirent.h
	dump_process.cpp
	dump_process.h
	export_list.cpp
	export_list.h
	hash.cpp
	hash.h
	module_list.cpp
	module_list.h
	pd.cpp
	pe_exports.cpp
	pe_exports.h
	pe_hash_database.cpp
	pe_hash_database.h
	pe_header.cpp
	pe_header.h
	pe_imports.cpp
	pe_imports.h
	simple.cpp
	simple.h
	stdafx.cpp
	stdafx.h
	stream_wrapper.h
	targetver.h
	terminate_monitor_hook.cpp
	terminate_monitor_hook.h
	utils.h
	work_queue.h
)
add_library(${PROJECT_NAME} SHARED ${process_dumper_src})
target_link_libraries(${PROJECT_NAME} PUBLIC Shlwapi.lib Kernel32.lib)