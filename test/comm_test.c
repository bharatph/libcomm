#include <stdio.h>
//#include<clog/clog.h>
#include <comm.h>
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

static const char *TAG = "TEST";

comm_socket sock = -1;

int test_connection(){
    sock = comm_start_server(3500);
    if(sock < 0){
        return -1;
    } else return 0;
}

int test_write(){
    if(test_connection() < 0)return -1;
    if( comm_write_text(sock, "testing") < 0){
        return -1;
    }
	return 0;
}

int test_read(){
#define _L_BUF_LEN 10
    if(test_connection() < 0)return -1;
    char *buffer;
    buffer = comm_read_text(sock);
    if(buffer == NULL)return -1;
    printf("Mesage: %s\n", buffer);
    return 0;
}


int main(int argc, char *argv[]){
    int status = -1;
    if(argc < 2){
        return -1;
    }
    if(strcmp(argv[1], "connection") == 0){
        status = test_connection();
    } else if(strcmp(argv[1], "write") == 0){
        status = test_write();
    } else if(strcmp(argv[1], "read") == 0){
        status = test_read();
    }
	comm_close_socket(sock);
    return status;
}
