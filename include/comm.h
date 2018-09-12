#ifndef _COMM
#define _COMM "COMM"
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(__linux__) || defined(__APPLE__)
#if defined(__linux__) && defined(kernel_version_2_4)
#include <sys/sendfile.h>
#endif
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
typedef int SOCKET;
#elif _WIN32
//#include <windows.h>
//#include <ws2tcpip.h>
#include <winsock2.h>
#include <Windows.h>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
	typedef short ssize_t;
	typedef int socklen_t;
#else
#error OS not supported
#endif

#define COMM_BUFFER_SIZE 256
#define COMM_CON_MAX_ATTEMPTS 5
#define COMM_SERV_BACKLOG 10

/*
 * Check if the given port is valid
 * @param port The port number to be checked
 * @return 0 is return for success and -1 indicates failure
 */
int comm_check_port(int port);

/*
 * Write binary data to a given socket
 * @param sockfd The socket descriptor to write to
 * @param buffer The binary to be written to the socket
 * @return Success value is returned mentioning a value is sent or not
 * 0 for success
 * 1 for failure
 */
int comm_write_binary(SOCKET sockfd, const void *buffer);

/*
 * Write data to a given socket
 * @param sockfd The socket descriptor to write to
 * @param buffer The message to be written to the socket
 * @return Success value is returned mentioning a value is sent or not
 * 0 for success
 * 1 for failure
 */
int comm_write_text(SOCKET sockfd, const char *buffer);

/*
 * Read text data from a socket, reads until a newline is encountered
 * @param sockfd The socket descriptor to write to
 * @return buffer data from the socket
 */
char *comm_read_text(SOCKET sockfd);

/*
 * Read binary data from a socket, read until socket is closed,
 * if the given buffer points to null the memory will be 
 * allocated with respect to bufflen
 * @param sockfd The socket descriptor to write to
 * @param buffer The buffer to store the data
 * @param The length of the buffer
 * @return buffer data from the socket
 */
void *comm_read_binary(SOCKET sockfd, void *buffer, int bufflen);

/*
 * closes the socket if it open
 * @param sockfd The socket to be closed
 */
int comm_close_socket(SOCKET sockfd);

/** Connects to the server with the standard IPv4 and TCP stack
 * @param hostname IPv4 address or hostname
 * @param port Port number for the server to start
 * @return Socket descriptor of the started server
 */
SOCKET comm_connect_server(const char *hostname, int port);

//TODO support multiple server options
/** Starts the server with the standard IPv4 and TCP stack
 * @param port Port number for the server to start
 * @return Socket descriptor of the started server
 */
SOCKET comm_start_server(int port);

#ifdef __cplusplus
}
#endif
#endif