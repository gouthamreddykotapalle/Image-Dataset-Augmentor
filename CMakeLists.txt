cmake_minimum_required(VERSION 3.14)
project(lib)


include_directories(/usr/local/include) #this is important
set(CMAKE_INCLUDE_PATH /usr/local/lib)  #this is important

#this is to add it manually
link_directories(/usr/local/lib) #this is important

#set(CMAKE_CXX_STANDARD_LIBRARIES -ljpeg)


set(CMAKE_CXX_STANDARD 17)

SET(GCC_COVERAGE_COMPILE_FLAGS "-O3 -std=c++17 -Wall -Wextra -Wpedantic -Werror") #standarg flags, just google them
#the linker flag can either be -l library or -llibrary
SET(GCC_COVERAGE_LINK_FLAGS    "-l jpeg") #original libjpeg linker flags -> points to /usr/local/lib -> depends on the library present in that path

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")


set(SOURCE_FILES main.cpp Augmentor.cpp Augmentor.h jpeg.h jpeg.cpp Operation.cpp Operation.h)

add_executable(output ${SOURCE_FILES})



