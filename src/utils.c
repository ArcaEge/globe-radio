#include "utils.h"

ip4_addr_t getAddressFromHost(const char *host) {
    ip4_addr_t hostAddress;
    assert(netconn_gethostbyname(host, &hostAddress) == ERR_OK);
    return hostAddress;
}