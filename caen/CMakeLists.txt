cmake_minimum_required(VERSION 2.8)
project(common_vme_caen)

SET(src CaenBase.cpp Caen775.cpp caen513_drv.c caen775_drv.c caen792_drv.c caen965_drv.c)
ADD_LIBRARY(${PROJECT_NAME} STATIC ${src})
ADD_EXECUTABLE(test_qdc965 test/test_qdc_caen.c)
ADD_EXECUTABLE(test_tdc775 test/test_tdc_caen.c)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} common_vme common_debug)
TARGET_LINK_LIBRARIES(test_qdc965 ${PROJECT_NAME} common_vme vme common_debug ${FrameworkLib})
TARGET_LINK_LIBRARIES(test_tdc775 ${PROJECT_NAME} common_vme vme common_debug ${FrameworkLib})

INSTALL_TARGETS(/lib ${PROJECT_NAME})
INSTALL_TARGETS(/bin test_qdc965)
INSTALL_TARGETS(/bin test_tdc775)

FILE(GLOB src *.h)
INSTALL(FILES ${src} DESTINATION include/common/vme/caen)
