cmake_minimum_required(VERSION 2.8.9)

# Specify minimum version

set(PROJECT_NAME "knnocr")
project(${PROJECT_NAME} CXX)

# Include subdirectories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Check if libraries exist
find_package(OpenCV REQUIRED)
message("found ${OpenCV_LIBRARIES}")



# Include directories
include_directories(${OpenCV_INCLUDE_DIRS})

FILE(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/src/*)
FILE(GLOB HEADER_FILES ${PROJECT_SOURCE_DIR}/include/*)

# The main program
add_executable(${PROJECT_NAME} ${PROJECT_NAME}.cpp ${SRC_FILES} ${HEADER_FILES})

# Link with libraries
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBRARIES})

