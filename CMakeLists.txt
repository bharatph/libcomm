cmake_minimum_required(VERSION 3.0)
project(comm VERSION 0.5 DESCRIPTION "Cross-platfrom Networking library for C")
file(GLOB_RECURSE comm_HDRS "include/*.h")
file(GLOB_RECURSE comm_SRCS "src/*.c")

add_library(comm ${comm_HDRS} ${comm_SRCS})
if (WIN32)
    target_link_libraries(comm wsock32 ws2_32)
endif ()

set_target_properties(comm PROPERTIES VERSION ${PROJECT_VERSION})

target_include_directories(comm PRIVATE include)

target_link_libraries(comm -lclog)

install(FILES ${comm_HDRS} DESTINATION include/comm)
install(TARGETS comm
        RUNTIME DESTINATION lib
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        PUBLIC_HEADER DESTINATION include)
