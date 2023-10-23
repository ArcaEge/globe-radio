#include <stdio.h>
#include <stdbool.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/apps/http_client.h"

#define HTTPC_DEBUG

#ifdef HTTPC_DEBUG
#define printf_DEBUG printf
#endif

struct HttpRequest {
    char *header;
    char *body;
    bool complete;
    httpc_result_t result;
    u32_t serverResponse;
    err_t err;
};

int main();

// struct HttpRequest* createHttpRequest(const char *serverName, const char* uri);

err_t httpClientHeadersDone(httpc_state_t* connection, struct HttpRequest* request, struct pbuf* hdr, u16_t hdr_len, u32_t content_len);
err_t httpClientReceive(struct HttpRequest* request, struct tcp_pcb* tpcb, struct pbuf* p, err_t err);
void httpClientResult(struct HttpRequest* request, httpc_result_t httpc_result, u32_t rx_content_len, u32_t srv_res, err_t err);