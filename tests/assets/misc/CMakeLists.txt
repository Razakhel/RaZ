cmake_minimum_required(VERSION 3.11)
project(PluginTest)

add_library(PluginTest SHARED PluginTest.cpp)
set_target_properties(PluginTest PROPERTIES OUTPUT_NAME "PlûgïnTést")
