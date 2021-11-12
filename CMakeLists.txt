project(common_vme)
option(UNIVERSE2 "open source universe2 driver" ON)
# find_package(Boost MODULE REQUIRED REQUIRED COMPONENTS program_options system thread log)
link_directories(${CMAKE_INSTALL_PREFIX}/lib)
ADD_DEFINITIONS(-fPIC)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS} core . ../.. )
MESSAGE(STATUS "enable universe2 driver")
ADD_DEFINITIONS(-DUNIVERSE2)  
# ADD_SUBDIRECTORY(vmedrv/universe2)


SET(src core/vmewrap.cpp core/  core/vmewrap_universe2.cpp core/vmewrap_caenvme.cpp core/vmewrap_uservme.cpp vmedrv/universe2/vmelib.cpp core/vmewrap_sis3153_eth.cpp vmedrv/sis3153/sis3153ETH_vme_class.cpp) 
find_package(chaos QUIET)
IF(chaos_FOUND)	
ADD_DEFINITIONS(-DCHAOS)
SET(src ${src} core/VmeBase.cpp core/RTVme.cpp)
ADD_SUBDIRECTORY(caen)
ADD_SUBDIRECTORY(sis)
ADD_EXECUTABLE(test_pio test/test_pio.cpp)
TARGET_LINK_LIBRARIES(test_pio ${PROJECT_NAME} ${vmelibs} common_debug ${FrameworkLib} )
INSTALL(TARGETS test_pio
        DESTINATION "bin"
        PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
SET(ADDLIB common_debug boost_log boost_system boost_thread pthread ${FrameworkLib})
ELSE()
SET(ADDLIB)
ENDIF()

ADD_LIBRARY(${PROJECT_NAME} ${src})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}  ${ADDLIB} dl)
SET(test_btf_daq_src test/test_btf_daq.cpp )


# ADD_EXECUTABLE(test_btf_daq ${test_btf_daq_src})
#ADD_EXECUTABLE(test_btf_daq2 test/test_btf_daq2.cpp)
ADD_EXECUTABLE(vme_read test/vme_read.cpp)
ADD_EXECUTABLE(vme_write test/vme_write.cpp)
SET(vmelibs common_vme_caen common_vme_sis common_vme )
#TARGET_LINK_LIBRARIES(test_btf_daq2 ${PROJECT_NAME} ${vmelibs} common_debug ${FrameworkLib} )
#TARGET_LINK_LIBRARIES(test_btf_daq ${PROJECT_NAME} ${vmelibs} common_debug ${FrameworkLib})
TARGET_LINK_LIBRARIES(vme_read ${PROJECT_NAME}  ${ADDLIB} )
TARGET_LINK_LIBRARIES(vme_write ${PROJECT_NAME}  ${ADDLIB})

#INSTALL_TARGETS(/lib ${PROJECT_NAME})
INSTALL(TARGETS ${PROJECT_NAME}
        DESTINATION "lib"
        PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
##INSTALL_TARGETS(/bin/vme test_btf_daq)
#INSTALL_TARGETS(/bin/vme test_pio)

#INSTALL_TARGETS(/bin/vme vme_read)
INSTALL(TARGETS vme_read
        DESTINATION "bin"
        PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
#INSTALL_TARGETS(/bin/vme vme_write)
INSTALL(TARGETS vme_write
        DESTINATION "bin"
        PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
FILE(GLOB src sis/*.h)
INSTALL(FILES ${src} DESTINATION include/common/vme/sis)
FILE(GLOB src core/*.h)
INSTALL(FILES ${src} DESTINATION include/common/vme/core/)
