
set( HCB_PLATFORM "x86")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address")
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address" )

