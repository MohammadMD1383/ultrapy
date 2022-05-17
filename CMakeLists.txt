cmake_minimum_required(VERSION 3.22)
project(ultrapy)

set(CMAKE_CXX_STANDARD 20)

include_directories("/Application/C++ Tools/UltraLight/ultralight-sdk-1.2.1-linux-x64/include")
link_directories("/Application/C++ Tools/UltraLight/ultralight-sdk-1.2.1-linux-x64/bin")

link_libraries(UltralightCore AppCore Ultralight WebCore)

add_library(ultrapy SHARED ultrapy.cpp)

set_target_properties(
        ultrapy
        PROPERTIES
        PREFIX ""
)
