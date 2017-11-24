cmake_minimum_required(VERSION 2.8)
project(common_vme_sis)

file(GLOB src *.c)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${src})
ADD_EXECUTABLE(test_sis test/test_sis.c)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} common_vme common_debug)

TARGET_LINK_LIBRARIES(test_sis ${PROJECT_NAME} common_vme common_debug ${FrameworkLib})
FILE(GLOB src *.h)
INSTALL(FILES ${src} DESTINATION include/common/vme/sis)
INSTALL_TARGETS(/lib ${PROJECT_NAME})
INSTALL_TARGETS(/bin test_sis)
