cmake_minimum_required(VERSION 3.25)

project(RogueGlitchTools CXX)

find_package(Catch2 3 CONFIG REQUIRED)
find_package(cryptopp CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

option(WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)
option(ENABLE_CPPCHECK "Enable static analysis with cppcheck" OFF)
option(ENABLE_CLANG_TIDY "Enable static analysis with clang-tidy" OFF)
option(ENABLE_INCLUDE_WHAT_YOU_USE "Enable static analysis with include-what-you-use" OFF)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/CompilerWarnings.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/ProjectSettings.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/StaticAnalyzers.cmake)

if(EMSCRIPTEN)
	add_library(rgt_wasm INTERFACE)
	target_compile_options(rgt_wasm INTERFACE -sNO_DISABLE_EXCEPTION_CATCHING)
	target_link_options(rgt_wasm INTERFACE -sSTACK_SIZE=2MB --no-entry -lembind)
	set_target_properties(rgt_wasm PROPERTIES SUFFIX ".wasm")
endif()

add_library(rgt_common src/file.cpp src/dependency_info.cpp)
target_include_directories(rgt_common PUBLIC inc)
target_compile_features(rgt_common PUBLIC cxx_std_17)

add_library(rgt_crypto src/crypto.cpp)
target_include_directories(rgt_crypto PUBLIC inc)
target_compile_features(rgt_crypto PUBLIC cxx_std_17)
target_link_libraries(rgt_crypto PRIVATE cryptopp::cryptopp)

add_library(rgt_convert src/convert.cpp)
target_include_directories(rgt_convert PUBLIC inc)
target_compile_features(rgt_convert PUBLIC cxx_std_17)
target_link_libraries(rgt_convert PRIVATE nlohmann_json::nlohmann_json)

if(EMSCRIPTEN)
	add_executable(rgt_webui src/webui.cpp)
	target_include_directories(rgt_webui PUBLIC inc)
	target_link_libraries(rgt_webui PRIVATE rgt_wasm rgt_crypto rgt_convert)
	add_custom_target(rgt_webui.html)
	add_custom_command(TARGET rgt_webui.html COMMAND ${CMAKE_COMMAND}
			-E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/src/webui.html $<TARGET_FILE_DIR:rgt_webui>
	)
	add_dependencies(rgt_webui rgt_webui.html)
endif()

if(NOT EMSCRIPTEN)
	add_executable(rgt_converter src/converter.cpp)
	target_include_directories(rgt_converter PUBLIC inc)
	target_compile_features(rgt_converter PUBLIC cxx_std_17)
	target_link_libraries(rgt_converter PRIVATE rgt_common rgt_crypto rgt_convert)
endif()

enable_testing()
include(Catch)

add_executable(rgt_test test/crypto.cpp test/convert.cpp)
target_link_libraries(rgt_test PRIVATE rgt_crypto rgt_common rgt_convert Catch2::Catch2WithMain)
if(NOT EMSCRIPTEN)
	catch_discover_tests(rgt_test)
	add_custom_target(rgt_test_data)
	add_custom_command(TARGET rgt_test_data COMMAND ${CMAKE_COMMAND} -E copy_if_different
			${CMAKE_CURRENT_SOURCE_DIR}/test/data/SaveData.glitch
			${CMAKE_CURRENT_SOURCE_DIR}/test/data/SaveData.used.glitch
			${CMAKE_CURRENT_SOURCE_DIR}/test/data/SaveData__Deluxe.glitch
			$<TARGET_FILE_DIR:rgt_test>
	)
	add_dependencies(rgt_test rgt_test_data)
endif()
