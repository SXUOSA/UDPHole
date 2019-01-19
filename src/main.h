#pragma once
#include <stdbool.h>
#include <inttypes.h>

struct
{
    bool daemon;
    uint16_t port_openvpn;
    uint16_t port_detector;
    char *host_echoserver;
    uint16_t port_echoserver;
    uint8_t delay_heartbeat;
} global_args;

int main(int argc, char *argv[]);
void fork_daemon();
