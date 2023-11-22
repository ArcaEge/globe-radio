#include "stream.h"

// // Opens socket to host and port, returning descriptor (socket number, when called multiple times will return 
// // previous descriptor + 1 if successful). Returns -1 if unsuccessful.
// //
// // ~~Stolen~~ Borrowed from:
// // https://github.com/AdrianBesciak/InternetRadioReceiver/blob/master/Radio/io/stream/network/TcpStream.cpp#L21
// // with slight changes to work with C.
// int openSocket(const char* host, uint16_t port) {
//     ip4_addr_t hostAddress = getAddressFromHost(host);
//     char addressString[16];
//     strcpy(addressString, ip4addr_ntoa(&hostAddress));

//     struct sockaddr_in socket = {};
//     socket.sin_family = AF_INET;
//     socket.sin_port = lwip_htons(port);
//     lwip_inet_pton(socket.sin_family, addressString, &(socket.sin_addr));

//     int descriptor = lwip_socket(socket.sin_family, SOCK_STREAM, 0);
//     if (lwip_connect(descriptor, (struct sockaddr*)&socket, sizeof(socket)) != 0) {
//         printf("Error contacting remote host '%s'", host);
//         return -1;
//     }

//     struct timeval sendTimeout = { 2, 0 };
//     struct timeval receiveTimeout = { 1, 0 };
//     lwip_setsockopt(descriptor, SOL_SOCKET, SO_SNDTIMEO, &sendTimeout, sizeof(receiveTimeout));
//     lwip_setsockopt(descriptor, SOL_SOCKET, SO_RCVTIMEO, &receiveTimeout, sizeof(sendTimeout));

//     return descriptor;
// }

static volatile uint8_t audio_is_playing = 0;

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

void decode_mp3_stream(int socket_fd) {
    char mp3_buf[MP3_BUF_SIZE];
    HMP3Decoder hMP3Decoder;
    MP3FrameInfo mp3FrameInfo;

    // Initialize the Helix MP3 decoder
    hMP3Decoder = MP3InitDecoder();

    while (1) {
        // Read data from the socket into the buffer
        int bytes_read = lwip_recv(socket_fd, mp3_buf, MP3_BUF_SIZE, 0);
        if (bytes_read <= 0) {
            // Handle error or end of stream
            break;
        }

        // Decode the MP3 data
        short pcm_buf[1152 * 2];
        int offset = 0;
        unsigned char* mp3_buf_ptr = mp3_buf + offset;
        while (offset < bytes_read) {
            int err = MP3Decode(hMP3Decoder, &mp3_buf_ptr, &bytes_read, (short*)pcm_buf, 0);
            if (err) {
                // Handle decoding error
                printf("Error decoding: %d\n", err);
                break;
            }

            // Get info about the decoded frame
            MP3GetLastFrameInfo(hMP3Decoder, &mp3FrameInfo);

            // Calculate time per frame
            float milliseconds_per_frame = ((float)mp3FrameInfo.outputSamps / (float)mp3FrameInfo.samprate) * 1000.0f;

            vTaskDelay(milliseconds_per_frame);

            // Output the decoded audio data
            printf("Decoded %d samples\n", mp3FrameInfo.outputSamps);
        }
    }

    // Free the Helix MP3 decoder
    MP3FreeDecoder(hMP3Decoder);
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
    unsigned char hdrBuffer[1024];

    // Read headers from stream
    while (true) {
        int len = recv(stream->descriptor, hdrBuffer, sizeof(hdrBuffer) - 1, 0);

        if (len < 0) {
            printf("Error reading response\n");
            return;
        }

        if (len == 0) {
            printf("End of response\n");
            break;
        }

        hdrBuffer[len] = 0;
        printf("%s", hdrBuffer);

        if (strstr(hdrBuffer, "\r\n\r\n") != NULL) {
            printf("End of headers\n");
            decode_mp3_stream(stream->descriptor);
            break;
        }
        
    }

    // struct mp3_decoder* decoder;
    // decoder = mp3_decoder_create();

    // if (decoder != NULL) {
    //     decoder->fetch_data = readFromStream;
    //     decoder->fetch_parameter = (void*)&stream;
    //     decoder->output_cb = mp3_callback;

    //     //while (mp3_decoder_run(decoder) != -1);
    //     while (mp3_decoder_run(decoder) != -1);

    //     /* delete decoder object */
    //     mp3_decoder_delete(decoder);
    // }

    closeSocket(stream->descriptor);
}


// Writes data to HttpStream
void writeToStream(struct HttpStream* stream, const char* data) {
    lwip_write(stream->descriptor, data, strlen(data));
}

// Reads data from HttpStream into buffer
int readFromStream(void* stream, char* buffer, uint count) {
    return lwip_recv(((struct HttpStream*)stream)->descriptor, buffer, count, 0);
}

static uint32_t mp3_callback(MP3FrameInfo *header, int16_t *buffer, uint32_t length) {
    while (audio_is_playing == 1);

    audio_is_playing = 1;
    // Process audio data

    return 0;
}