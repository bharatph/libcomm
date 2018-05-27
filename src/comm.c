#include <comm.h>

#include <clog/clog.h>

int comm_check_port(int port)
{
    if (port < 0 || port > 65535)
    {
        log_err(_COMM, "invalid port number, port number should be between 0 and 65536");
        return -1;
    }
    return 0;
}

int comm_write_text(SOCKET sockfd, const char *in_buffer)
{
    int blen = strlen(in_buffer);
    char *buffer = (char *)malloc(sizeof(char) * blen);
    strcpy(buffer, in_buffer);
    ssize_t bwrite = -1;
    ssize_t write_len = blen;
    bwrite = send(sockfd, buffer, write_len, 0);
    while (bwrite > 0)
    {
        write_len -= bwrite;
        bwrite = send(sockfd, buffer, write_len, 0);
    }
    if (bwrite == -1)
    {
        log_err(_COMM, "Write failed");
    }
    else if (bwrite == 0)
    {
    }
    return bwrite; //here it indicates error or success
}

int comm_write_binary(SOCKET sockfd, const void *in_buffer)
{
    int blen = strlen(in_buffer);
    char *buffer = (char *)malloc(blen);
    strcpy(buffer, in_buffer);
    ssize_t bwrite = -1;
    ssize_t write_len = blen;
    bwrite = send(sockfd, buffer, write_len, 0);
    while (bwrite > 0)
    {
        write_len -= bwrite;
        bwrite = send(sockfd, buffer, write_len, 0);
    }
    if (bwrite == -1)
    {
        log_err(_COMM, "Write failed");
    }
    else if (bwrite == 0)
    {
    }
    return bwrite; //here it indicates error or success
}

char *comm_read_text(SOCKET sockfd)
{
    char *buf = (char *)malloc(256);
    int ptr = 0;
    memset(buf, '\0', 256);
    int quit = 0;
    for (ptr = 0; quit != 1; ptr++)
    {
        int bread = recv(sockfd, buf + ptr, 1, 0);
        if (bread > 0)
        {
            // log_inf("SERVER", "Content read[%d]: %c", ptr, buf[ptr]);
            if (buf[ptr] == '\n' || buf[ptr] == '\r')
            {
                buf[ptr] = '\0';
                return buf;
            }
        }
        else if (bread == 0)
        { // EOF hit, client disconnected
            log_inf("SERVER", "EOF hit");
            return NULL;
        }
        else if (bread < 0)
        { // read error
            log_inf("SERVER", "read error exiting...");
            return NULL;
        }
    }
    return NULL;
}

void *comm_read_binary(SOCKET sockfd)
{
    ssize_t bytes_read = -1;
    int buffer_size = COMM_BUFFER_SIZE;
    void *buffer = (void *)malloc(COMM_BUFFER_SIZE);
    int i = 0;
    memset(buffer, '\0', COMM_BUFFER_SIZE);
    while ((bytes_read = recv(sockfd, buffer + i, buffer_size - i, 0)) > 0)
    {
        i += bytes_read;
        if (i >= buffer_size)
        {
            buffer_size += COMM_BUFFER_SIZE;
            void *new_buffer = realloc(buffer, buffer_size);
            if (new_buffer)
            {
                buffer = new_buffer;
            }
        }
    }
    return buffer;
}

int comm_disconnect_server(SOCKET sockfd)
{
#if defined(_WIN32)
    if (closesocket(sockfd) == -1)
#else
    if (close(sockfd) == -1)
#endif
    {
        log_err(_COMM, "Disconnection Unsuccesful");
        return -1;
    }
    else
        log_inf(_COMM, "Disconnection Successful");
    return 0;
}

//checking whether port is between 0 and 65536
int comm_connect_server(const char *hostname, int port)
{
    struct sockaddr_in serv_addr;
    struct hostent *server;
    if (comm_check_port(port) != 0)
    {
        return -1;
    }
    //Create socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        log_err(_COMM, "Could not create socket");
        return -1;
    }
    log_inf(_COMM, "Socket created");
    if ((server = gethostbyname(hostname)) == NULL)
    {
        log_err(_COMM, "no such host found");
        return -1;
    }
    memset((char *)&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy((char *)&serv_addr.sin_addr.s_addr, (char *)server->h_addr, server->h_length);
    serv_addr.sin_port = htons(port);
    int i = 0;
    while (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
    {
        if (i++ > COMM_CON_MAX_ATTEMPTS)
        {
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

int comm_start_server(int port)
{
    static int cont;
    static int servfd;

    struct sockaddr_in server, client;
    socklen_t cli_size;

    if (comm_check_port(port) != 0)
    {
        return -1;
    }
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);
    cli_size = sizeof(struct sockaddr_in);

    if (cont == port)
    {
        log_inf(_COMM, "Connection accepted");
        return accept(servfd, (struct sockaddr *)&client, &cli_size);
    }
    if (cont == 0)
        cont = port;
    //Create socket
    servfd = socket(PF_INET, SOCK_STREAM, 0);
    if (servfd == -1)
    {
        log_err(_COMM, "could not create socket");
        return -1;
    }
    //Bind
    if (bind(servfd, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        log_err(_COMM, "bind failed");
        return -1;
    }
    //Listen
    listen(servfd, COMM_SERV_BACKLOG);
    //Accept and incoming connection
    log_inf(_COMM, "Waiting for incoming connections...");
    //accept connection from an incoming client
    int clifd = accept(servfd, (struct sockaddr *)&client, &cli_size);
    if (clifd < 0)
    {
        log_inf(_COMM, "Accept failed");
        return -1;
    }
    log_inf(_COMM, "Connection accepted");
    return clifd;
}
