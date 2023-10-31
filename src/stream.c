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
    lwip_shutdown(descriptor, SHUT_RDWR);
    lwip_close(descriptor);
}

// Initialises HttpStream with host and path
void initHttpStream(struct HttpStream* stream, const char* host, const char* path) {
    // Copy host and path to HttpStream
    stream->url.host = (char*)calloc(strlen(host) + 1, sizeof(char));
    strcpy(stream->url.host, host);
    stream->url.path = (char*)calloc(strlen(path) + 1, sizeof(char));
    strcpy(stream->url.path, path);

    stream->descriptor = openSocket(host, 80);

    // Write headers to stream
    writeToStream(stream, "GET ");
    writeToStream(stream, stream->url.path);
    writeToStream(stream, " HTTP/1.1\r\n");
    writeToStream(stream, "Accept: audio/*\r\n");
    writeToStream(stream, "Host: ");
    writeToStream(stream, stream->url.host);
    writeToStream(stream, "\r\n");

    // TODO: read headers from stream
}

// Writes data to HttpStream
void writeToStream(struct HttpStream* stream, const char* data) {
    lwip_write(stream->descriptor, data, strlen(data));
}