#include <comm.h>

#ifdef _WIN32
#include <windef.h>
#include <ws2tcpip.h>
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
    memcpy(buffer, in_buffer, blen);
    int bwrite = 0;
    int write_len = (int)blen;
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
    memcpy(buffer, in_buffer, blen);
    int bwrite = -1;
    int write_len = (int)blen;
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
    bytesRead = xs_recv(sock, buffer, bufflen, 0);
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
    //struct sockaddr_in serv_addr;
	//struct hostent *server;
    if (comm_check_port(port) != 0)
    {
        return xs_ERROR;
    }

    //
	char port_s[16];
	snprintf(port_s, 15, "%d", port);

    struct addrinfo hints;
	struct addrinfo *result, *rp;
	int s;
	xs_SOCKET sock = xs_ERROR;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = 0;
	hints.ai_protocol = 0;
    clog_enable();

	s = getaddrinfo(hostname, (const char *)port_s, &hints, &result);
	if (s != 0) {
		clog_e(_COMM, "getaddrinfo: %s\n", gai_strerror(s));
		return xs_ERROR;
	}

	for (rp = result; rp != NULL; rp = rp->ai_next) {
		sock = socket(rp->ai_family, rp->ai_socktype,
			rp->ai_protocol);
		if (sock == xs_ERROR) {
			continue;
		}
		if (connect(sock, rp->ai_addr, (int)rp->ai_addrlen) != xs_ERROR) {
            clog_i(_COMM, "connection established successfully to %s on port %d", hostname, port);
			break;
		}
		xs_close(sock);
	}
	
	freeaddrinfo(result);
    
    if(rp  == NULL) {
        clog_i(_COMM, "Connection failed");
    }
    
    return sock;
}
	
xs_SOCKET comm_start_server(int port)
{
    static int cont;
    static xs_SOCKET servfd;

    struct sockaddr_in server, client;
    socklen_t cli_size;

    if (comm_check_port(port) < 0)
    {
        return xs_ERROR;
    }
    //Prepare the sockaddr_in structure
    server.sin_family = AF_UNSPEC;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons((u_short)port);
    cli_size = sizeof(struct sockaddr_in);

    if (cont == port)
    {
        xs_SOCKET clifd = xs_accept(servfd, (struct sockaddr *)&client, &cli_size);
        if (clifd == xs_ERROR)
        {
            clog_i(_COMM, "Accept failed");
            return xs_ERROR;
        }
        clog_i(_COMM, "Connection accepted");
        return clifd;
    }
    if (cont == 0)
        cont = port;
    //Create socket
    servfd = xs_socket(AF_UNSPEC, SOCK_STREAM, 0);
    if (servfd == xs_ERROR)
    {
        clog_e(_COMM, "could not create socket");
        return xs_ERROR;
    }
    //Bind
    if (bind(servfd, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        clog_e(_COMM, "bind failed");
        return xs_ERROR;
    }
    //Listen
    listen(servfd, COMM_SERV_BACKLOG);
    return servfd;
}
