#pragma once
#include <string.h>
#include <lwip/api.h>
#include "lwip/apps/http_client.h"

struct HttpUrl {
    char* host;
    char* path;
};

// A single HTTP header
struct Header {
    char* key;
    char* value;
};

struct HttpRequest {
    char* header;
    char* body;
    bool complete;
    httpc_result_t result;
    u32_t serverResponse;
    err_t err;
};

// Returns IP address from host name
ip4_addr_t getAddressFromHost(const char *host);

// Converts header to string with optional carriage return
char* headerToString(struct Header* header, bool crlf);