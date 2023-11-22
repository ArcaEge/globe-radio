#pragma once
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>
#include <lwip/api.h>
#include <lwip/sockets.h>
#include <string.h>
#include "mp3dec.h"
#include "ipUtils.h"
#include "mp3.h"

#define MP3_BUF_SIZE 8192

struct HttpStream {
    struct HttpUrl url;
    int descriptor;
    struct Header* headers;
    uint headerCount;
};

static uint32_t mp3_callback(MP3FrameInfo *header, int16_t *buffer, uint32_t length);

int openSocket(const char* host, uint16_t port);
void closeSocket(int descriptor);

void initHttpStream(struct HttpStream* stream, const char* host, const char* path);
void writeToStream(struct HttpStream* stream, const char* data);
int readFromStream(void* stream, char* buffer, uint count);