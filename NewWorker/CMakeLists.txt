cmake_minimum_required (VERSION 2.8)
project(NewWorker)

# check for in-source build, forbid it!
if(CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR AND NOT MSVC_IDE)
  message(FATAL_ERROR "\nIn-source build attempt detected!\n"
    "Please create a new directory (e.g. build) and run `cmake ..`.\n"
    "Also don't forget to delete the created CMakeCache.txt and CMakeFiles dir"
    )
endif()

# find ROOT
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
find_package(ROOT 5.28 REQUIRED)

link_directories(${ROOT_LIBRARY_DIR})

# use some sane default directories 
if(NOT DEFINED EXECUTABLE_OUTPUT_PATH)
       set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")
endif()

# we check for empty string here, since the variable
# is indeed defined to an empty string
if(NOT CMAKE_BUILD_TYPE)
  # this also reflects this default in the GUI
	SET(CMAKE_BUILD_TYPE Debug CACHE STRING
    "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
    FORCE)
endif()

# really no optimization in debug mode
if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
endif()

# find the header files
include_directories("source" ${ROOT_INCLUDE_DIR})

# construct the exe from source files
add_executable("NewWorker" "source/Worker.cc" "source/Worker.h")
target_link_libraries("NewWorker" ${ROOT_LIBRARIES} "-lMinuit")
#message("${ROOT_LIBRARIES}")

