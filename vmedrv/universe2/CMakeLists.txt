cmake_minimum_required(VERSION 2.8)
project(vme)


ADD_LIBRARY(${PROJECT_NAME} STATIC vmelib.cpp)

INSTALL_TARGETS(/lib ${PROJECT_NAME})
