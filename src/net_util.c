#include<net_util.h>

/*
 * Write data to a given socket
 * @param sockfd The socket descriptor to write to
 * @param msg The message to be written to the socket
 */

int write_data(int sockfd, const char *in_buffer, int blen){
	char *buffer = (char *)malloc(sizeof(char) * blen);
	strcpy(buffer, in_buffer);
	ssize_t bwrite = -1;
	ssize_t write_len = blen;
	bwrite = write(sockfd, buffer, write_len);
	while(bwrite > 0 ){
		write_len -= bwrite;
		bwrite = write(sockfd, buffer, write_len);
	}
	if(bwrite == -1){
		printf("write failed");
	}
	else if(bwrite == 0){
	}
	return bwrite; //here it indicates error or success
}

int writeln(int sockfd, char *buf, int len){
	int bwrite = write(sockfd, buf, len);
	//log_inf("CLIENT", "written buf: %s, sent: %d", buf, bwrite);
	if(bwrite > 0){
		if(bwrite < len){
			return writeln(sockfd, buf+bwrite, len - bwrite);
		}
		if(bwrite == len){
			write(sockfd, "\n", 1);
			//log_inf("CLIENT", "write sucessful");
			return 0;
		}
	}
	else if(bwrite == 0){
		log_inf("CLIENT", "Unspecified write error");
		return -1;
	}
	else if(bwrite < 0){
		log_inf("CLIENT", "write error, exiting...");
		return -1;
	}
}

/*
 * Write a file to a socket
 * Automatically chooses the optimized way at compile time
 * @param sockfd The socket descriptor to write to
 * @param file_path The file which has to written to the socket
 */
int write_file(int sockfd, const char *file){
#if defined(__linux__) || defined(__APPLE__)
#if defined(__kernel_version_2_4)
	//sendfile(..,..,...);
#else
	FILE *fp = fopen(file, "rb");
	if(fp == NULL){
		log_err(_NET_TAG, "cannot open specified file");
		return -1;
	}
#endif
#endif
	//code to send file to socket
	return -1;
}

/** Read data from the given socket
 * @param sockfd The socket descriptor to read form
 */
int read_data(int sockfd, char *buffer, int rlen){
	ssize_t read_bytes = -1;
	int i = 0;
	while((read_bytes = read(sockfd, buffer+i, 1)) >  0){
		i++;
	}
	return read_bytes;
}

char *readln(int sockfd){
		char *buf = (char *)malloc(256);
		int ptr = 0;
		memset(buf, '\0', 256);
		int quit=0;
		for(ptr=0;quit!=1;ptr++){
			int bread = read(sockfd, buf+ptr, 1);
			if(bread > 0){
				//log_inf("SERVER", "Content read[%d]: %c", ptr, buf[ptr]);
				if(buf[ptr] == '\n'){
					buf[ptr] = '\0';
					quit = 1;
				}
			}
			else if(bread == 0){//EOF hit, client disconnected
				log_inf("SERVER", "EOF hit");
				return NULL;
			}
			else if(bread < 0){ //read error
				log_inf("SERVER", "read error exiting...");
				return NULL;
			}
		}
		
		//sleep(1);
		//this checking is used to prevent read from reading continuosly even if no data is sent in the occasion of peer disconnection
		if(buf[0] == '\0'){ //FIXME use non-char checking, such as size??
			return NULL;
		}
		
		log_inf("SERVER", "%s", buf);
		return buf;
}

/** Read data from the given socket
 * @param sockfd The socket descriptor to read form
 */
FILE *read_file(int sockfd){
#if defined(__linux__) || defined(__APPLE__)
#if defined(__kernel_version_2_4)
	//recvfile(..,..,...);
#else
	//read(..,..,..);
#endif
#endif
	return NULL;
}

int disconnect_server(int sockfd){
	if(close(sockfd) == -1){
		log_err(_NET_TAG, "Disconnection Unsuccesful");
        return -1;
	}
	else log_inf(_NET_TAG, "Disconnection Successful");
    return 0;
}

int connect_server (const char * hostname, int port){
	struct sockaddr_in serv_addr;
	struct hostent *server;
	//checking whether port is between 0 and 65536
	if (port < 0 || port > 65535){
		log_err (_NET_TAG, "invalid port number, port number should be between 0 and 65536");
		return -1;
	}
	//Create socket
	int sockfd = socket(AF_INET , SOCK_STREAM , 0);
	if (sockfd == -1){
		log_err(_NET_TAG, "Could not create socket");
		return -1;
	}
	log_inf(_NET_TAG, "Socket created");
	if((server = gethostbyname(hostname))==NULL){
		log_err(_NET_TAG, "no such host found");
		return -1;
	}
	memset((char *)&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	memcpy((char*)&serv_addr.sin_addr.s_addr, (char*)server->h_addr, server->h_length);
	serv_addr.sin_port = htons( port );
	int i = 0;
	while (connect (sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1){
		if(i++ > CON_MAX_ATTEMPTS){
			//guess other hostnames for the user
			close(sockfd);
			log_err(_NET_TAG, "cannot establish connection to %s on port %d", hostname, port);
			return -1;
		}
	}
	log_inf(_NET_TAG, "connection established successfully to %s on port %d", hostname, port);
	return sockfd;
}

#ifndef SERV_BACKLOG
#define SERV_BACKLOG 10
#endif


//TODO support multiple server options
/** Starts the server with the standard IPv4 and TCP stack
 * @param port Port number for the server to start
 * @return Socket descriptor of the started server
 */
int start_server(int port){
	static int cont;
	static int servfd;

	struct sockaddr_in server, client;
    	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons( port );
	socklen_t cli_size = sizeof(struct sockaddr_in);
	
	if(cont == port){
		log_inf(_NET_TAG, "Connection accepted");
		return accept(servfd, (struct sockaddr *)&client, &cli_size);
	}
	if(cont == 0)
		cont = port;
	//Create socket
	servfd = socket(PF_INET , SOCK_STREAM , 0);
	if (servfd == -1){
		log_err(_NET_TAG, "could not create socket");
		return -1;
	}
	//Bind
	if( bind(servfd,(struct sockaddr *)&server , sizeof(server)) < 0){
		log_err(_NET_TAG, "bind failed");
		return -1;
	}
	//Listen
	listen(servfd , SERV_BACKLOG);
	//Accept and incoming connection
	log_inf(_NET_TAG, "Waiting for incoming connections...");
	//accept connection from an incoming client
	int clifd = accept(servfd, (struct sockaddr *)&client, &cli_size);
	if (clifd < 0){
		log_inf(_NET_TAG, "Accept failed");
		return -1;
	}
	log_inf(_NET_TAG, "Connection accepted");
	return clifd;
}
