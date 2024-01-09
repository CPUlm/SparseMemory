cmake_minimum_required(VERSION 3.16)

project(SparseMemory)

add_library(SparseMemory
    memory.c
    memory.h)

target_include_directories(SparseMemory PUBLIC .)
