#include "httpClient.h"

err_t httpClientHeadersDone(httpc_state_t* connection, struct HttpRequest* request, struct pbuf* hdr, u16_t hdr_len, u32_t content_len) {
#ifdef HTTPC_DEBUG
    printf("in headers_done_fn\n");
    printf("content length: %d\n", content_len);
#endif

    request->header = (char*)calloc(hdr_len, sizeof(char));
    request->body = (char*)calloc(content_len, sizeof(char));

    pbuf_copy_partial(hdr, request->header, hdr_len, 0);
    return ERR_OK;
}

err_t httpClientReceive(struct HttpRequest* request, struct tcp_pcb* tpcb, struct pbuf* p, err_t err) {
    printf(">>> recv_fn >>>\n");
    if (p == NULL) {
        printf("p is NULL\n");
    } else {
        printf("p: %p\n", p);

        strcat(request->body, p->payload);

        printf("next: %p\n", p->next);
        printf("buffer len: %d\n", strlen(request->body));
        printf("payload: %p\n", p->payload);
        printf("len: %d\n", p->len);
    }
    printf("<<< recv_fn <<<\n");
    return ERR_OK;
}

// Print result
void httpClientResult(struct HttpRequest* request, httpc_result_t httpc_result, u32_t rx_content_len, u32_t srv_res, err_t err) {
    printf(">>> result_fn >>>\n");
    printf("httpc_result: %s\n",
        httpc_result == HTTPC_RESULT_OK ? "HTTPC_RESULT_OK"
        : httpc_result == HTTPC_RESULT_ERR_UNKNOWN ? "HTTPC_RESULT_ERR_UNKNOWN"
        : httpc_result == HTTPC_RESULT_ERR_CONNECT ? "HTTPC_RESULT_ERR_CONNECT"
        : httpc_result == HTTPC_RESULT_ERR_HOSTNAME ? "HTTPC_RESULT_ERR_HOSTNAME"
        : httpc_result == HTTPC_RESULT_ERR_CLOSED ? "HTTPC_RESULT_ERR_CLOSED"
        : httpc_result == HTTPC_RESULT_ERR_TIMEOUT ? "HTTPC_RESULT_ERR_TIMEOUT"
        : httpc_result == HTTPC_RESULT_ERR_SVR_RESP ? "HTTPC_RESULT_ERR_SVR_RESP"
        : httpc_result == HTTPC_RESULT_ERR_MEM ? "HTTPC_RESULT_ERR_MEM"
        : httpc_result == HTTPC_RESULT_LOCAL_ABORT ? "HTTPC_RESULT_LOCAL_ABORT"
        : httpc_result == HTTPC_RESULT_ERR_CONTENT_LEN ? "HTTPC_RESULT_ERR_CONTENT_LEN"
        : "*UNKNOWN*");
    printf("received %ld bytes\n", rx_content_len);
    printf("server response: %ld\n", srv_res);
    printf("err: %d\n", err);
    printf("\nheader:\n");
    printf(request->header);
    printf("body:\n");
    printf(request->body);
    printf("\n");
    printf("<<< result_fn <<<\n");

    (*request).complete = true;
    (*request).err = err;
    (*request).result = httpc_result;
    (*request).serverResponse = srv_res;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wincompatible-pointer-types"
extern httpc_connection_t getConnectionSettings() {
    httpc_connection_t connection_settings = {
        .use_proxy = 0,
        .headers_done_fn = httpClientHeadersDone,
        .result_fn = httpClientResult
    };
    return connection_settings;
}
#pragma GCC diagnostic pop