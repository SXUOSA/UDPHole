#pragma once
#include <arpa/inet.h>
int send_packet(int socket, int src_port, int dst_port, in_addr_t src_addr, in_addr_t dst_addr);