cmake_minimum_required(VERSION 2.8)

INCLUDE_DIRECTORIES(SYSTEM ${SYSROOT}/include)
include_directories( ./export )
include_directories( ./src )

file(GLOB lib_SRC
    "./src/*.h"
    "./src/*.cpp"
)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Weffc++")

add_library (Lib STATIC ${lib_SRC})

target_include_directories(Lib PUBLIC ./export)
target_include_directories(Lib PUBLIC ${SYSROOT}/include)
target_link_libraries (Lib rt)
