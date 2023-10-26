#include "httpclient.h"

// int main() {
// #include "wificonnection.h"

//     stdio_init_all();

//     connectToWifi(WIFI_SSID, WIFI_PASSWORD);

//     char* serverName = "de1.api.radio-browser.info";
//     char* uri = "/json/stations/topclick/2";

//     struct HttpRequest request;

//     request.complete = false;

//     httpc_connection_t settings = {
//         .use_proxy = 0,
//         .headers_done_fn = httpClientHeadersDone,
//         .result_fn = httpClientResult
//     };
//     httpc_state_t* connection = NULL;

//     request.err = httpc_get_file_dns(serverName, HTTP_DEFAULT_PORT, uri, &settings, httpClientReceive, &request, &connection);

//     printf("Waiting for request to complete\n");
//     while (!(request.complete)) {
//         sleep_ms(10);
//     }
//     printf("Request complete\n");

//     cyw43_arch_deinit();
//     return 0;
// }

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
