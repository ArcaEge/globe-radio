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

    printf("Connecting to server at %s on port %u\n", addressString, ntohs(listen_addr.sin_port));
    if (connect(stream->descriptor, (struct sockaddr*)&listen_addr, sizeof(listen_addr)) != 0) {
        printf("Error contacting remote host '%s'", addressString);
        return;
    }

    // Write headers to stream
    writeToStream(stream, "GET ");
    writeToStream(stream, stream->url.path);
    writeToStream(stream, " HTTP/1.1\r\n");
    writeToStream(stream, "Accept: audio/*\r\n");
    writeToStream(stream, "Host: ");
    writeToStream(stream, stream->url.host);
    writeToStream(stream, "\r\n\r\n");

    // Header buffer
    unsigned char buffer[2048];

    // Read headers from stream
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

    HMP3Decoder decoder = MP3InitDecoder();
    int buffer_len = 0;
    // int len = 0;

    // while ((len = lwip_recv(stream->descriptor, buffer, 2048, 0)) > 0) {
    //     short samples[2048];
    //     unsigned char* mp3Buffer = (unsigned char*)buffer;
    //     int num_samples = MP3Decode(decoder, &mp3Buffer, &len, samples, 0);
    // }

    for (int i = 0; i < 20; i++) {
        printf("\nReading frame\n");
        int len = recv(stream->descriptor, buffer, sizeof(buffer) - 1, 0);

        if (len < 0) {
            printf("Error reading response\n");
            return;
        }

        if (len == 0) {
            printf("End of response\n");
            break;
        }

        short samples[2048];
        unsigned char* mp3Buffer = buffer;
        int bytesLeft = len;
        int result;

        do {
            result = MP3Decode(decoder, &mp3Buffer, &bytesLeft, samples, 0);
            printf("Decoded %d samples\n", result);

            if (result == ERR_MP3_MAINDATA_UNDERFLOW) {
                printf("Underflow\n");
                // Fetch more data from the stream
                int len = recv(stream->descriptor, buffer, sizeof(buffer) - 1, 0);
                if (len <= 0) {
                    break;
                }
                mp3Buffer = buffer;
                bytesLeft = len;
            } else if (result != ERR_MP3_NONE) {
                // Handle other errors
                printf("Error: %d\n", result);
                break;
            }
        } while (result != ERR_MP3_NONE);
        printf("End of frame\n");
    }


    closeSocket(stream->descriptor);
}

// Writes data to HttpStream
void writeToStream(struct HttpStream* stream, const char* data) {
    lwip_write(stream->descriptor, data, strlen(data));
}

// Reads data from HttpStream into buffer
int readFromStream(struct HttpStream* stream, char* buffer, uint count) {
    return recv(stream->descriptor, buffer, count, 0);
}