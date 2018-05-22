#include<comm.h>

#include <clog/clog.h>

int write_data(int sockfd, const char *in_buffer, int blen) {
    char *buffer = (char *) malloc(sizeof(char) * blen);
    strcpy(buffer, in_buffer);
    ssize_t bwrite = -1;
    ssize_t write_len = blen;
    bwrite = send(sockfd, buffer, write_len, 0);
    while (bwrite > 0) {
        write_len -= bwrite;
        bwrite = send(sockfd, buffer, write_len, 0);
    }
    if (bwrite == -1) {
        printf("write failed");
    } else if (bwrite == 0) {
    }
    return bwrite; //here it indicates error or success
}

void * read_data(int sockfd) {
    ssize_t read_bytes = -1;
    void *buffer = (void *)malloc(sizeof(char) * 256);
    int i = 0;
    while ((read_bytes = recv(sockfd, buffer + i, 1, 0)) > 0) {
        i++;
    }
    return buffer;
}

int disconnect_server(int sockfd) {
#if defined(_WIN32)
    if (closesocket(sockfd) == -1)
#else
    if(close(sockfd) == -1)
#endif
    {
        log_err(_COMM, "Disconnection Unsuccesful");
        return -1;
    } else log_inf(_COMM, "Disconnection Successful");
    return 0;
}

int connect_server(const char *hostname, int port) {
    struct sockaddr_in serv_addr;
    struct hostent *server;
    //checking whether port is between 0 and 65536
    if (port < 0 || port > 65535) {
        log_err (_COMM, "invalid port number, port number should be between 0 and 65536");
        return -1;
    }
    //Create socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        log_err(_COMM, "Could not create socket");
        return -1;
    }
    log_inf(_COMM, "Socket created");
    if ((server = gethostbyname(hostname)) == NULL) {
        log_err(_COMM, "no such host found");
        return -1;
    }
    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy((char *) &serv_addr.sin_addr.s_addr, (char *) server->h_addr, server->h_length);
    serv_addr.sin_port = htons(port);
    int i = 0;
    while (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1) {
        if (i++ > CON_MAX_ATTEMPTS) {
            //guess other hostnames for the user
#ifdef _WIN32
            closesocket(sockfd);
#else
            close(sockfd);
#endif
            log_err(_COMM, "cannot establish connection to %s on port %d", hostname, port);
            return -1;
        }
    }
    log_inf(_COMM, "connection established successfully to %s on port %d", hostname, port);
    return sockfd;
}

int start_server(int port) {
    static int cont;
    static int servfd;

    struct sockaddr_in server, client;
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);
    socklen_t cli_size = sizeof(struct sockaddr_in);

    if (cont == port) {
        log_inf(_COMM, "Connection accepted");
        return accept(servfd, (struct sockaddr *) &client, &cli_size);
    }
    if (cont == 0)
        cont = port;
    //Create socket
    servfd = socket(PF_INET, SOCK_STREAM, 0);
    if (servfd == -1) {
        log_err(_COMM, "could not create socket");
        return -1;
    }
    //Bind
    if (bind(servfd, (struct sockaddr *) &server, sizeof(server)) < 0) {
        log_err(_COMM, "bind failed");
        return -1;
    }
    //Listen
    listen(servfd, SERV_BACKLOG);
    //Accept and incoming connection
    log_inf(_COMM, "Waiting for incoming connections...");
    //accept connection from an incoming client
    int clifd = accept(servfd, (struct sockaddr *) &client, &cli_size);
    if (clifd < 0) {
        log_inf(_COMM, "Accept failed");
        return -1;
    }
    log_inf(_COMM, "Connection accepted");
    return clifd;
}
