cmake_minimum_required(VERSION 3.18)

project(UIDesignTester LANGUAGES CXX)

# Check for dependencies
find_package(RmlUi CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(PNG REQUIRED)
find_package(CURL CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

add_subdirectory(src)
