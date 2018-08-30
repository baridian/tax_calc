cmake_minimum_required(VERSION 3.10)
project(tax_calc)

set(CMAKE_CXX_STANDARD 11)

add_executable(tax_calc src/main.cpp src/hashTable.cpp)