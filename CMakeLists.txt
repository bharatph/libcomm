cmake_minimum_required(VERSION 3.8)
project(comm VERSION 0.3.0 LANGUAGES C CXX)
find_package(clog)
add_library(comm include/comm.h src/comm.c)

set_target_properties(comm PROPERTIES VERSION ${PROJECT_VERSION})

target_link_libraries(comm ${CLOG_LIBS})

target_include_directories(comm PRIVATE crosssguid)
target_include_directories(comm PRIVATE include)
target_include_directories(comm PRIVATE ${CLOG_INCLUDE_DIR})

add_subdirectory(test)

install(FILES include/comm.h DESTINATION include)
install(FILES comm-config.cmake DESTINATION lib/cmake/comm)
install(TARGETS comm
        RUNTIME DESTINATION lib
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        PUBLIC_HEADER DESTINATION include)
enable_testing()
