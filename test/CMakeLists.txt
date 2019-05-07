add_executable(comm_test comm_test.c)
find_package(Threads REQUIRED)

target_link_libraries(comm_test comm)
target_link_libraries(comm_test ${CMAKE_THREAD_LIBS_INIT})

add_test(NAME comm_server_test COMMAND comm_test server)
add_test(NAME comm_connection_test COMMAND comm_test connection)
add_test(NAME comm_write_test COMMAND comm_test write)
add_test(NAME comm_read_test COMMAND comm_test read)

enable_testing()
