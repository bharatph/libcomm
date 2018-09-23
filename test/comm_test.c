#include <stdio.h>
#include <comm.h>
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#include <pthread.h>
#endif

#define _D_PORT 3500

static const char *TAG = "TEST";

comm_socket sock = -1;


int test_server(int port){
    sock = comm_start_server(port);
    if(sock == SOCKET_ERROR){
		comm_close_socket(sock);
        return -1;
    } else return 0;
}

int test_server_loop(int port) {
	int loop = 3;
	while (loop --> 0) {
		sock = comm_start_server(port);
		if (sock == SOCKET_ERROR) {
			comm_close_socket(sock);
			return -1;
		}
	}
	return 0;
}

void *start_server(void *port){
  //test_server((*(int *)port));
}

int test_connection(){
      //pthread_t thread_id;
      //int port = 7500;
    //pthread_create(&thread_id, NULL, start_server, (void *)&port);
  sock = comm_connect_server("localhost", _D_PORT);
  if(sock < 0){
    return -1;
  }
      //pthread_join(thread_id, NULL);
  return 0;
}

int test_write(){
    if(test_connection() < 0)return -1;
    if( comm_write_text(sock, "hello") < 0){
        return -1;
    }
	return 0;
}

int test_read(){
    if(test_server(_D_PORT) < 0)return -1;
    char buffer[8];
    if(comm_read_text(sock, buffer, 7) < 0){
      printf("Test: Read Error\n");
    }
    if(buffer == NULL)return -1;
    printf("%s\n", buffer);
    return 0;
}

int test_read_lines(){
  if(test_server(_D_PORT) < 0)return -1;
  char buffer[100];
  if(comm_read_text(sock, buffer, 10) < 0){
    printf("Test: Read error");
  }
  if(buffer == NULL)return -1;
  int *line_len = (int *)calloc(sizeof(int), 1);
  char **lines = read_line(line_len, buffer);
  if(*line_len == 0){
    return -1;
  }
  int i = 0;
  for(i = 0; i < *line_len; i++){
    printf("Message[%d]: %s\n", i, lines[i]);
  }
}

int main(int argc, char *argv[]){
    int status = -1;
	/*
    if(argc < 2){
        return -1;
    }
	*/
	comm_init();
    if(strcmp(argv[1], "server") == 0){
        status = test_server(_D_PORT);
	} else if (strcmp(argv[1], "server_loop") == 0) {
		status = test_server_loop(_D_PORT);
	} else if(strcmp(argv[1], "connection") == 0){
        status = test_connection();
    } else if(strcmp(argv[1], "write") == 0){
        status = test_write();
    } else if(strcmp(argv[1], "read") == 0){
        status = test_read();
    } else if(strcmp(argv[1], "read_lines") == 0){
      status = test_read_lines();
    } else {
      printf("Unknown parameter\n");
      return -1;
    }
	comm_close_socket(sock);
	comm_clean();
    return status;
}
