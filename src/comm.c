#include <comm.h>

#ifdef _WIN32
#include <windef.h>
#endif

#include <clog/clog.h>
#include <crosssocket.h>

int comm_init()
{
    return xs_init();
}

int comm_clean()
{
    return xs_clean();
}

int comm_check_port(int port)
{
    if (port < 0 || port > 65535)
    {
        clog_e(_COMM, "invalid port number, port number should be between 0 and 65536");
        return -1;
    }
    return 0;
}

int comm_write_text(xs_SOCKET sockfd, const char *in_buffer)
{
    size_t blen = strlen(in_buffer);
    char *buffer = (char *)malloc(sizeof(char) * blen);
    strcpy(buffer, in_buffer);
    ssize_t bwrite = 0;
    size_t write_len = blen;
    bwrite = send(sockfd, buffer, write_len, 0);
    while (bwrite > 0)
    {
        write_len -= bwrite;
        bwrite = send(sockfd, buffer, write_len, 0);
    }
    if (bwrite == -1)
    {
        clog_e(_COMM, "Write failed");
    }
    else if (bwrite == 0)
    {
    }
    return bwrite; //here it indicates error or success
}

int comm_write_binary(xs_SOCKET sockfd, const void *in_buffer)
{
    size_t blen = strlen(in_buffer);
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
        clog_e(_COMM, "Write failed");
    }
    else if (bwrite == 0)
    {
    }
    return bwrite; //here it indicates error or success
}

char *comm_read_text(xs_SOCKET sockfd, int max_len)
{
    char *buffer = (char *)calloc(sizeof(char), max_len);
    int read_bytes = xs_recv(sockfd, buffer, max_len, 0);
    if (read_bytes < 0)
    {
        return NULL;
    }
    else if (read_bytes == 0)
    {
        return NULL;
    }
    if (buffer[read_bytes - 2] == '\r' && buffer[read_bytes - 1] == '\n')
    {
        buffer[read_bytes - 2] = '\0';
    }
    else if (buffer[read_bytes - 1] == '\r' || buffer[read_bytes] == '\n')
    {
        buffer[read_bytes - 1] = '\0';
    }
    return buffer;
}

int comm_recv(xs_SOCKET sock, void *buffer, int bufflen, int flags)
{
    return recv(sock, buffer, bufflen, flags);
}

int comm_read_binary(xs_SOCKET sock, char *buffer, int bufflen)
{
    if (bufflen == 0)
        return 0;
    int bytesRead = 0;
    if (buffer == NULL)
    {
        return -1;
    }
    bytesRead = read(sock, buffer, bufflen);
    if (bytesRead < 0)
    {
        clog_f(_COMM, "Read error");
        return -1;
    }
    if (bytesRead == 0)
    {
        //EOF HIT, client disconnected
        clog_i(_COMM, "EOF HIT");
        return -1;
    }
    if (bytesRead < bufflen)
    {
        //poll for data, if data exist rerun subroutine
        comm_read_binary(sock, buffer + bytesRead, bufflen - bytesRead);
    }
    return 0;
}

int comm_close_socket(xs_SOCKET sockfd)
{
  return xs_close(sockfd);
}

//checking whether port is between 0 and 65536
xs_SOCKET comm_connect_server(const char *hostname, int port)
{
    struct sockaddr_in serv_addr;
    struct hostent *server;
    if (comm_check_port(port) != 0)
    {
        return -1;
    }
    //Create xs_socket
    xs_SOCKET sockfd = xs_socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == SOCKET_ERROR)
    {
        clog_e(_COMM, "Could not create xs_socket");
        return -1;
    }
    clog_i(_COMM, "Socket created");
    if ((server = gethostbyname(hostname)) == NULL)
    {
        clog_e(_COMM, "no such host found");
        return -1;
    }
    memset((char *)&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    //inet_pton(AF_INET, (char *)server->h_addr, &serv_addr);
    memcpy((char *)&serv_addr.sin_addr.s_addr, (char *)server->h_addr, server->h_length);
    serv_addr.sin_port = htons(port);
    int i = 0;
    while (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
    {
        if (i++ > COMM_CON_MAX_ATTEMPTS)
        {
            //guess other hostnames for the user
#ifdef _WIN32
            closexs_socket(sockfd);
#else
            close(sockfd);
#endif
            clog_e(_COMM, "cannot establish connection to %s on port %d", hostname, port);
            return -1;
        }
    }
    clog_i(_COMM, "connection established successfully to %s on port %d", hostname, port);
    return sockfd;
}

xs_SOCKET comm_start_server(int port)
{
    static int cont;
    static xs_SOCKET servfd;

    struct sockaddr_in server, client;
    socklen_t cli_size;

    if (comm_check_port(port) < 0)
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
        xs_SOCKET clifd = xs_accept(servfd, (struct sockaddr *)&client, &cli_size);
        if (clifd == SOCKET_ERROR)
        {
            clog_i(_COMM, "Accept failed");
            return -1;
        }
        clog_i(_COMM, "Connection accepted");
        return clifd;
    }
    if (cont == 0)
        cont = port;
    //Create socket
    servfd = socket(AF_INET, SOCK_STREAM, 0);
    if (servfd == SOCKET_ERROR)
    {
        clog_e(_COMM, "could not create socket");
        return -1;
    }
    //Bind
    if (bind(servfd, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        clog_e(_COMM, "bind failed");
        return -1;
    }
    //Listen
    listen(servfd, COMM_SERV_BACKLOG);
    return servfd;
}
