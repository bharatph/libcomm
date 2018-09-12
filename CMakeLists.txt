cmake_minimum_required(VERSION 3.0)
project(comm VERSION 0.7 DESCRIPTION "Cross-platfrom Networking library for C")
find_package(clog)
add_library(comm src/comm.c)
if (WIN32)
    target_link_libraries(comm wsock32 ws2_32)
endif ()

set_target_properties(comm PROPERTIES VERSION ${PROJECT_VERSION})

target_include_directories(comm PUBLIC include)
target_include_directories(comm PUBLIC ${CLOG_INCLUDE_DIR})
target_link_libraries(comm ${CLOG_LIBS})

add_subdirectory(test)

install(FILES include/comm.h DESTINATION include)
install(FILES comm-config.cmake DESTINATION lib/cmake/comm)
install(TARGETS comm
        RUNTIME DESTINATION lib
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        PUBLIC_HEADER DESTINATION include)
enable_testing()