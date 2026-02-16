cmake_minimum_required(VERSION 3.16)
project(Bifide LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (WIN32 AND EXISTS "C:/msys64/ucrt64/bin/wx-config")
	set(wxWidgets_CONFIG_EXECUTABLE "C:/msys64/ucrt64/bin/wx-config")
endif()

find_package(wxWidgets REQUIRED COMPONENTS core base)
include(${wxWidgets_USE_FILE})

add_executable(bifide bifide.cpp)
target_link_libraries(bifide ${wxWidgets_LIBRARIES})
