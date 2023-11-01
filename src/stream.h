#pragma once
// Mostly copied from AdrianBesciak's Internet Radio Receiver

#pragma once
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>
#include <lwip/api.h>
#include <lwip/sockets.h>
#include <string.h>
#include "ipUtils.h"

struct HttpStream {
    struct HttpUrl url;
    int descriptor;
    struct Header* headers;
    uint headerCount;
};


int openSocket(const char* host, uint16_t port);
void closeSocket(int descriptor);

void initHttpStream(struct HttpStream* stream, const char* host, const char* path);
void writeToStream(struct HttpStream* stream, const char* data);
void readFromStream(struct HttpStream* stream, char* buffer, uint count);