cmake_minimum_required(VERSION 3.5.1)
project(comm)

find_package(clog)
find_package(crosssocket)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

# Set the build type if not set
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

add_library(comm ${CMAKE_CURRENT_SOURCE_DIR}/src/comm.c)
target_include_directories(comm PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)
#dependencies
target_link_libraries(comm clog)
target_include_directories(comm PRIVATE clog)

target_include_directories(comm PRIVATE crosssocket)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(WARNINGS "-Werror" "-Wall")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(WARNINGS "-Werror" "-Wall")
elseif(MSVC)
    set(WARNINGS "/WX" "/W4")
endif()
target_compile_options(comm PRIVATE ${WARNINGS})

if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
	include(GNUInstallDirs)

	set(COMM_INC_INSTALL_DIR "${CMAKE_INSTALL_INCLUDEDIR}")
	set(COMM_RUNTIME_INSTALL_DIR "${CMAKE_INSTALL_BINDIR}")
	set(COMM_LIBRARY_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}")
	set(COMM_ARCHIVE_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}")
	set(COMM_FRAMEWORK_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}")

	set(COMM_CMAKE_CONFIG_INSTALL_DIR "${CMAKE_INSTALL_DATADIR}/comm/cmake")
	set(COMM_ADDITIONAL_FILES_INSTALL_DIR "${CMAKE_INSTALL_DATADIR}/comm")

	# Install target
	install(TARGETS comm EXPORT commTargets
		RUNTIME       DESTINATION ${COMM_RUNTIME_INSTALL_DIR}
		LIBRARY       DESTINATION ${COMM_LIBRARY_INSTALL_DIR}
		ARCHIVE       DESTINATION ${COMM_ARCHIVE_INSTALL_DIR}
		FRAMEWORK     DESTINATION ${COMM_FRAMEWORK_INSTALL_DIR})

	# Install headers
	install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/"
		DESTINATION ${COMM_INC_INSTALL_DIR})

	# Make cmake config files for all targets
	install(EXPORT  commTargets
		DESTINATION ${COMM_CMAKE_CONFIG_INSTALL_DIR}
		FILE comm-config.cmake)

	# Install readme and license
	install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE" "${CMAKE_CURRENT_SOURCE_DIR}/README.md"
		DESTINATION ${COMM_ADDITIONAL_FILES_INSTALL_DIR})
endif()

if (COMM_TESTS)
  add_subdirectory(test)
endif()
