#include "stream.h"

// Opens socket to host and port, returning descriptor (socket number, when called multiple times will return 
// previous descriptor + 1 if successful). Returns -1 if unsuccessful.
//
// ~~Stolen~~ Borrowed from:
// https://github.com/AdrianBesciak/InternetRadioReceiver/blob/master/Radio/io/stream/network/TcpStream.cpp#L21
// with slight changes to work with C.
int openSocket(const char* host, uint16_t port) {
    ip4_addr_t hostAddress = getAddressFromHost(host);
    char addressString[16];
    strcpy(addressString, ip4addr_ntoa(&hostAddress));

    struct sockaddr_in socket = {};
    socket.sin_family = AF_INET;
    socket.sin_port = lwip_htons(port);
    lwip_inet_pton(socket.sin_family, addressString, &(socket.sin_addr));

    int descriptor = lwip_socket(socket.sin_family, SOCK_STREAM, 0);
    if (lwip_connect(descriptor, (struct sockaddr*)&socket, sizeof(socket)) != 0) {
        printf("Error contacting remote host '%s'", host);
        return -1;
    }

    struct timeval sendTimeout = { 2, 0 };
    struct timeval receiveTimeout = { 1, 0 };
    lwip_setsockopt(descriptor, SOL_SOCKET, SO_SNDTIMEO, &sendTimeout, sizeof(receiveTimeout));
    lwip_setsockopt(descriptor, SOL_SOCKET, SO_RCVTIMEO, &receiveTimeout, sizeof(sendTimeout));

    return descriptor;
}

// Closes socket with descriptor
void closeSocket(int descriptor) {
    // lwip_shutdown(descriptor, SHUT_RDWR);
    lwip_close(descriptor);
}

static void send_message(int socket, char* msg) {
    int len = strlen(msg);
    int done = 0;
    while (done < len) {
        int done_now = send(socket, msg + done, len - done, 0);
        if (done_now <= 0)
            return;

        done += done_now;
    }
}

// Initialises HttpStream with host and path
void initHttpStream(struct HttpStream* stream, const char* host, const char* path) {
    // Copy host and path to HttpStream
    stream->url.host = (char*)calloc(strlen(host) + 1, sizeof(char));
    strcpy(stream->url.host, host);
    stream->url.path = (char*)calloc(strlen(path) + 1, sizeof(char));
    strcpy(stream->url.path, path);

    // stream->descriptor = openSocket(host, 80);
    ip4_addr_t hostAddress = getAddressFromHost(host);
    char addressString[16] = "192.168.1.123";
    strcpy(addressString, ip4addr_ntoa(&hostAddress));

    stream->descriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    struct sockaddr_in listen_addr = {
        .sin_len = sizeof(struct sockaddr_in),
        .sin_family = AF_INET,
        .sin_port = htons(80),
        .sin_addr = 0,
    };
    inet_pton(listen_addr.sin_family, addressString, &(listen_addr.sin_addr));

    if (stream->descriptor < 0) {
        printf("Unable to create socket: error %d", errno);
        return;
    }

    // if (bind(stream->descriptor, (struct sockaddr*)&listen_addr, sizeof(listen_addr)) < 0) {
    //     printf("Unable to bind socket: error %d\n", errno);
    //     return;
    // }

    // if (listen(stream->descriptor, kConnectionThreadCount * 2) < 0) {
    //     printf("Unable to listen on socket: error %d\n", errno);
    //     return;
    // }

    printf("Connecting to server at %s on port %u\n", addressString, ntohs(listen_addr.sin_port));
    if (connect(stream->descriptor, (struct sockaddr*)&listen_addr, sizeof(listen_addr)) != 0) {
        printf("Error contacting remote host '%s'", addressString);
        return;
    }

    writeToStream(stream, "GET ");
    writeToStream(stream, stream->url.path);
    writeToStream(stream, " HTTP/1.1\r\n");
    writeToStream(stream, "Accept: audio/*\r\n");
    writeToStream(stream, "Host: ");
    writeToStream(stream, stream->url.host);
    writeToStream(stream, "\r\n\r\n");

    char buffer[1024];

    while (true) {
        int len = recv(stream->descriptor, buffer, sizeof(buffer) - 1, 0);

        if (len < 0) {
            printf("Error reading response\n");
            return;
        }

        if (len == 0) {
            printf("End of response\n");
            break;
        }

        buffer[len] = 0;
        printf("%s", buffer);

        if (strstr(buffer, "\r\n\r\n") != NULL) {
            printf("End of headers\n");
            break;
        }
    }

    closeSocket(stream->descriptor);

    // while (true) {
    //     struct sockaddr_storage remote_addr;
    //     socklen_t len = sizeof(remote_addr);
    //     int conn_sock = accept(stream->descriptor, (struct sockaddr*)&remote_addr, &len);
    //     if (conn_sock >= 0) {
    //         handle_connection(conn_sock);
    //     }
    //     // char buffer[1];
    //     // readFromStream(stream, buffer, 1);
    //     // printf("%s", buffer);
    //     //vTaskDelay(1);
    // }

    // // TODO: read headers from stream
    // char buffer[128];
    // int done = 0;
    // // readFromStream(stream, buffer, 128);
    // while (done < sizeof(buffer)) {
    //     int done_now = lwip_recv(stream->descriptor, buffer + done, sizeof(buffer) - done, 0);
    //     done += done_now;
    // }
    // printf("%s", buffer);
}

// Writes data to HttpStream
void writeToStream(struct HttpStream* stream, const char* data) {
    lwip_write(stream->descriptor, data, strlen(data));
}

// Reads data from HttpStream into buffer
void readFromStream(struct HttpStream* stream, char* buffer, uint count) {
    lwip_recv(stream->descriptor, buffer, count, 0);
}