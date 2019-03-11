cmake_minimum_required (VERSION 2.8.11)
#set( CMAKE_VERBOSE_MAKEFILE on )
project (HCB)
message(STATUS "CMAKE platform: " ${HCB_PLATFORM})

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)

if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

if("${HCB_PLATFORM}" STREQUAL "x86")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
	SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -m32" )
endif()


# non working option: -fno-omit-frame-pointer
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -pthread" )


#Executable directory
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin-${HCB_PLATFORM})


set (SYSROOT ${CMAKE_CURRENT_SOURCE_DIR}/libraries/sysroot-${HCB_PLATFORM})
message(STATUS "Sysroot: " ${SYSROOT})
link_directories(${SYSROOT}/lib)

message(STATUS "Gcc version: " ${CMAKE_CXX_COMPILER_VERSION})

set(BOOST_ROOT ${SYSROOT})
set(Boost_USE_STATIC_LIBS ON)

#Libraries
add_subdirectory (./Lib)
add_subdirectory (./Test)
add_subdirectory (./App)


