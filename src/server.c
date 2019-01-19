#include "server.h"
#include "main.h"
#include "udp.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

int raw_socket;
int detector_socket;

void server_init()
{
    raw_socket = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (raw_socket < 0)
    {
        perror("socket()");
        exit(1);
    }
    printf("[NAT Detector] Starting Symmetric NAT detector at UDP Prot: %d\n", global_args.port_detector);
    detector_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    struct sockaddr_in bind_addr = {0};
    bind_addr.sin_addr.s_addr = INADDR_ANY;
    bind_addr.sin_family = AF_INET;
    bind_addr.sin_port = htons(global_args.port_detector);
    if (bind(detector_socket, (struct sockaddr *)&bind_addr, sizeof(struct sockaddr_in)) == -1)
    {
        perror("bind INADDR_ANY");
        exit(EXIT_FAILURE);
    }
    pthread_t heartbeat;
    pthread_create(&heartbeat, NULL, heartbeat_thread, NULL);
    pthread_detach(heartbeat);
    struct sockaddr_in recv_addr;
    socklen_t size_recv_addr;
    while (1)
    {
        char buffer[32];
        size_recv_addr = sizeof(struct sockaddr_in);
        recvfrom(detector_socket, buffer, sizeof(buffer), 0, (struct sockaddr *)&recv_addr, &size_recv_addr);
        printf("[NAT Detector]Get packet from %s:%d\n", inet_ntoa(recv_addr.sin_addr), ntohs(recv_addr.sin_port));
        struct sockaddr_in *dst = calloc(1, sizeof(struct sockaddr_in));
        *dst = recv_addr;
        pthread_t shootport;
        pthread_create(&shootport, NULL, shootport_thread, dst);
        pthread_detach(shootport);
    }
}

void *shootport_thread(void *argv)
{
    int s = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    in_addr_t src = inet_addr("0.0.0.0");
    struct sockaddr_in *dst = (struct sockaddr_in *)argv;
    for (int i = 1000; i < 65535; i++)
    {
        send_packet(s, global_args.port_openvpn, i, src, (*dst).sin_addr.s_addr);
    }
    printf("[NAT Detector]Finish knocking port range on %s\n", inet_ntoa((*dst).sin_addr));
    close(s);
    free(argv);
    return NULL;
}

void *heartbeat_thread()
{
    printf("[HeartBeat] Starting heartbeat thread...\n");
    in_addr_t src = inet_addr("0.0.0.0");
    in_addr_t dst = inet_addr(global_args.host_echoserver);
    while (1)
    {
        send_packet(raw_socket, global_args.port_openvpn, global_args.port_echoserver, src, dst);
        printf("[HeartBeat] Sending UDP packet from %d to %s:%d\n", global_args.port_openvpn, global_args.host_echoserver, global_args.port_echoserver);
        sleep(global_args.delay_heartbeat);
    }
    return NULL;
}
