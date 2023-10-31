#include "ipUtils.h"

ip4_addr_t getAddressFromHost(const char* host) {
    ip4_addr_t hostAddress;
    assert(netconn_gethostbyname(host, &hostAddress) == ERR_OK);
    return hostAddress;
}

char* headerToString(struct Header* header, bool crlf) {
    if (crlf) {
        char* str = (char*)calloc(strlen(header->key) + strlen(header->value) + 5, sizeof(char));   // +5 for ": ", "\r\n" and null terminator
        snprintf(str, sizeof(str), "%s: %s\r\n", header->key, header->value);
        return str;
    }
    
    char* str = (char*)calloc(strlen(header->key) + strlen(header->value) + 3, sizeof(char));   // +3 for ": " and null terminator
    snprintf(str, sizeof(str), "%s: %s", header->key, header->value);
    return str;
}