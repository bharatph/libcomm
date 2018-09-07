#include <stdio.h>
#include <clog/clog.h>
#include <comm.h>
#include <unistd.h>

static const char *TAG = "TEST";

int sockfd = -1;

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
}

int test_read(){
    int buf_len = 10;
    char buffer[buf_len];
    recv(sockfd, buffer, buf_len, 0);
    buffer[buf_len - 1] = '\0';
    log_inf(TAG, buffer);
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
    close(sockfd);
    return status;
}