# Minimum required CMake version
cmake_minimum_required(VERSION 3.16)

# Project name and C++ standard
project(OllamaCpp)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories for nlohmann/json.hpp
include_directories(${PROJECT_SOURCE_DIR}/include)

# Find libcurl (required for HTTP requests)
find_package(CURL REQUIRED)

# Add executable target
add_executable(ollama_cpp ollama_cpp.cpp)

# Link libcurl and pthread
target_link_libraries(ollama_cpp PRIVATE ${CURL_LIBRARIES} pthread)
