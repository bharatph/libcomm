cmake_minimum_required(VERSION 3.0)
project(net_util VERSION 0.5 DESCRIPTION "Cross-platfrom Networking library for C")
file(GLOB_RECURSE net_util_HDRS "include/*.h")
file(GLOB_RECURSE net_util_SRCS "src/*.c")

add_library(net_util SHARED ${net_util_HDRS} ${net_util_SRCS})
target_link_libraries(net_util clog)
set_target_properties(net_util PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(net_util PROPERTIES SOVERSION 1)
set_target_properties(net_util PROPERTIES PUBLIC_HEADER ${net_util_HDRS})

target_include_directories(net_util PUBLIC include)
target_include_directories(net_util PUBLIC src)

include(GNUInstallDirs)

install(TARGETS net_util 
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
