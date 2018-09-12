#include <stdio.h>
//#include<clog/clog.h>
#include <comm.h>
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

static const char *TAG = "TEST";

SOCKET sockfd = -1;

int test_connection(){
    sockfd = comm_start_server(3500);
    if(sockfd < 0){
        return -1;
    } else return 0;
}

int test_write(){
    if( send(sockfd, "testing", 7, 0) < 0){
        return -1;
    }
	return -1;
}

int test_read(){
#define _L_BUF_LEN 10
	char buffer[_L_BUF_LEN];
    recv(sockfd, buffer, _L_BUF_LEN, 0);
    buffer[_L_BUF_LEN - 1] = '\0';
    //log_inf(TAG, buffer);
    return 0;
}


int main(int argc, char *argv[]){
    int status = -1;
    if(argc < 2){
        return -1;
    }
    if(strcmp(argv[1], "connection") == 0){
        status = test_connection();
    } else if(strcmp(argv[2], "write")){
        status = test_write();
    } else if(strcmp(argv[3], "read")){
        status = test_read();
    }
	comm_close_socket(sockfd);
    return status;
}