set(SRC ${SRC} simu.cpp targets/simu/simpgmspace.cpp)
foreach(FILE ${SRC})
  set(SIMU_SRC ${SIMU_SRC} ../../${FILE})
endforeach()

add_definitions(-DSIMU -g)
add_executable(simu WIN32 ${SIMU_SRC} )
add_dependencies(simu ${FIRMWARE_DEPENDENCIES})

target_include_directories(simu PUBLIC /usr/local/include/fox-1.6 PUBLIC /usr/include/fox-1.6 /opt/local/include/fox-1.6)
target_link_libraries(simu FOX-1.6 pthread)