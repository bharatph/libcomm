#ifndef _NET_TAG
#define _NET_TAG "NETWORK_UTIL"
#include <stdio.h>
#include <stdlib.h>
#include<string.h>
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
#endif

#ifdef CLOG_H
#include <clog/clog.h>
#else
#define log_inf(...)
#define log_err(...)
#define log_per(...)
#define log_fat(...)
#endif

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 256
#endif

#ifndef CON_MAX_ATTEMPTS
#define CON_MAX_ATTEMPTS 5
#endif

/*
 * Write data to a given socket
 * @param sockfd The socket descriptor to write to
 * @param msg The message to be written to the socket
 */

int write_data(int sockfd, const char *in_buffer, int blen);

int writeln(int, const char *, int);

/*
 * Write a file to a socket
 * Automatically chooses the optimized way at compile time
 * @param sockfd The socket descriptor to write to
 * @param file_path The file which has to written to the socket
 */
int write_file(int sockfd, const char *file);

/** Read data from the given socket
 * @param sockfd The socket descriptor to read form
 */
int read_data(int sockfd, char *buffer, int rlen);

const char *readln(int sockfd);

/** Read data from the given socket
 * @param sockfd The socket descriptor to read form
 */
FILE *read_file(int sockfd);

int disconnect_server(int sockfd);

int connect_server (const char * hostname, int port);

#ifndef SERV_BACKLOG
#define SERV_BACKLOG 10
#endif


//TODO support multiple server options
/** Starts the server with the standard IPv4 and TCP stack
 * @param port Port number for the server to start
 * @return Socket descriptor of the started server
 */
int start_server(int port);
#endif
