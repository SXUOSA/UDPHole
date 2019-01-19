#include "main.h"
#include "log.h"
#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static const char *optString = "dp:P:S:s:h?";

int main(int argc, char *argv[])
{
    global_args.daemon = false;
    global_args.port_openvpn = 59900;
    global_args.port_detector = 59901;
    global_args.host_echoserver = "114.114.114.114";
    global_args.port_echoserver = 53;
    global_args.delay_heartbeat = 5;
    int opt = getopt(argc, argv, optString);
    while (opt != -1)
    {
        switch (opt)
        {
        case 'd':
            global_args.daemon = true;
            break;
        case 'P':
            global_args.port_openvpn = atoi(optarg);
            break;
        case 'p':
            global_args.port_detector = atoi(optarg);
            break;
        case 'S':
            global_args.host_echoserver = optarg;
            break;
        case 's':
            global_args.port_echoserver = atoi(optarg);
            break;
        case 't':
            global_args.delay_heartbeat = atoi(optarg);
            break;
        case 'h':
        case '?':
        {
            printf("UDPHole: OpenVPN CERNET-Internet IPv4 NAT Passthrough Tool\r\n");
            printf("Usage: %s [OPTIONS]\r\n", argv[0]);
            printf("\t -d Running in daemon mode.\r\n");
            printf("\t -P [PORT] OpenVPN Server port. Default: 59900\r\n");
            printf("\t -p [PORT] Symmetric NAT detector port. Default: 59901\r\n");
            printf("\t -S [HOST] Echo server address. Default: 114.114.114.114\r\n");
            printf("\t -s [PORT] Echo server port. Default: 53\r\n");
            printf("\t -t [SECOND] Heartbeat packet interval. Default: 5\r\n");
            exit(EXIT_FAILURE);
        }
        break;
        }
        opt = getopt(argc, argv, optString);
    }
    if (global_args.daemon)
    {
        fork_daemon();
        use_syslog(&stdout);
        use_syslog(&stderr);
    }
    server_init();
}

void fork_daemon()
{
    int pid;
    pid = fork();
    if (pid)
    {
        exit(0);
    }
    else
    {
        if (pid < 0)
        {
            exit(1);
        }
    }
    setsid();
    pid = fork();
    if (pid)
    {
        exit(0);
    }
    else
    {
        if (pid < 0)
        {
            exit(1);
        }
    }
    for (uint32_t i = 0; i < 3; ++i)
    {
        close(i);
    }
}
