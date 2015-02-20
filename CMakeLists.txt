cmake_minimum_required(VERSION 2.6)
IF( DEFINED ENV{CHAOS_BUNDLE} )
  include($ENV{CHAOS_BUNDLE}/tools/project_template/CMakeChaos.txt)
ENDIF()

INCLUDE_DIRECTORIES(vme)
LINK_DIRECTORIES(./lib)
    set (CMAKE_C_FLAGS "-m32")
    set (CMAKE_CXX_FLAGS "-m32")
    set (CMAKE_LINK_FLAGS "-m32")

SET(caen_src caen/src/caen960_lib.c )
SET(caen960_test_src caen/src/test_caen960.c )


ADD_LIBRARY(common_vme_caen SHARED ${caen_src})
ADD_EXECUTABLE(caen960_test ${caen960_test_src})
TARGET_LINK_LIBRARIES(caen960_test common_vme_caen vme)

INSTALL_TARGETS(/lib  common_vme_caen)
INSTALL_TARGETS(/bin caen960_test)
