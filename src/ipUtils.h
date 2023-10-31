#pragma once

#include <lwip/api.h>

// Returns IP address from host name
ip4_addr_t getAddressFromHost(const char *host);