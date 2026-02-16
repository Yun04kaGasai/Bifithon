cmake_minimum_required(VERSION 3.16)
project(Bifide LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(wxWidgets REQUIRED COMPONENTS core base)
include(${wxWidgets_USE_FILE})

add_executable(bifide bifide.cpp)
target_link_libraries(bifide ${wxWidgets_LIBRARIES})
