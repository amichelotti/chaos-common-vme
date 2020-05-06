cmake_minimum_required(VERSION 2.8)
project(common_vme_caen)

SET(src CaenDaqBase.cpp CaenPioBase.cpp Caen775.cpp caen513_drv.cpp caen775_drv.cpp caen792_drv.cpp caen965_drv.cpp caen977_drv.cpp)
ADD_LIBRARY(${PROJECT_NAME} STATIC ${src})
ADD_EXECUTABLE(test_qdc965 test/test_qdc_caen.cpp)
ADD_EXECUTABLE(test_tdc775 test/test_tdc_caen.cpp)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} common_vme common_debug)
TARGET_LINK_LIBRARIES(test_qdc965 ${PROJECT_NAME} common_vme common_debug ${FrameworkLib})
TARGET_LINK_LIBRARIES(test_tdc775 ${PROJECT_NAME} common_vme common_debug ${FrameworkLib})

#INSTALL_TARGETS(/lib ${PROJECT_NAME})
INSTALL(TARGETS ${PROJECT_NAME}
        DESTINATION "lib"
        PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
#INSTALL_TARGETS(/bin test_qdc965)
INSTALL(TARGETS test_qdc965
        DESTINATION "bin"
        PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
#INSTALL_TARGETS(/bin test_tdc775)
INSTALL(TARGETS test_tdc775
        DESTINATION "bin"
        PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
FILE(GLOB src *.h)
INSTALL(FILES ${src} DESTINATION include/common/vme/caen)
