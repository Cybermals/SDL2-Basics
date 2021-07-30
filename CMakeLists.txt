#Minimum CMake version and policy settings
cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0076 NEW)

#Project Name
project(SDL2)

#Include sub-projects
add_subdirectory(Audio)
add_subdirectory(Images)
add_subdirectory(Input)
add_subdirectory(Text)
add_subdirectory(Window)