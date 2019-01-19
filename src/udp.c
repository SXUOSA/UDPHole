#include "udp.h"
#include <string.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/udp.h>
#include <netinet/ip.h>

#define LEN 18

struct pseudo_header
{
    u_int32_t source_address;
    u_int32_t dest_address;
    u_int8_t placeholder;
    u_int8_t protocol;
    u_int16_t udp_length;
};

unsigned short csum(unsigned short *ptr, int nbytes)
{
    register long sum;
    unsigned short oddbyte;
    register short answer;

    sum = 0;
    while (nbytes > 1)
    {
        sum += *ptr++;
        nbytes -= 2;
    }
    if (nbytes == 1)
    {
        oddbyte = 0;
        *((u_char *)&oddbyte) = *(u_char *)ptr;
        sum += oddbyte;
    }

    sum = (sum >> 16) + (sum & 0xffff);
    sum = sum + (sum >> 16);
    answer = (short)~sum;

    return (answer);
}

int send_packet(int socket, int src_port, int dst_port, in_addr_t src_addr, in_addr_t dst_addr)
{
    char datagram[4096], *pseudogram;
    memset(datagram, 0, 4096);

    struct iphdr *iph = (struct iphdr *)datagram;
    struct udphdr *udph = (struct udphdr *)(datagram + sizeof(struct ip));
    char *data = datagram + sizeof(struct iphdr) + sizeof(struct udphdr);
    strcpy(data, "HELLO WORLD");

    struct pseudo_header psh;

    struct sockaddr_in sin = {0};
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = dst_addr;

    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 0;
    iph->tot_len = sizeof(struct iphdr) + sizeof(struct udphdr) + LEN;
    iph->id = htonl(1);
    iph->frag_off = 0;
    iph->ttl = 255;
    iph->protocol = IPPROTO_UDP;
    iph->check = 0;
    iph->saddr = src_addr;
    iph->daddr = sin.sin_addr.s_addr;

    iph->check = csum((unsigned short *)datagram, iph->tot_len);

    udph->source = htons(src_port);
    udph->dest = htons(dst_port);
    udph->len = htons(8 + LEN);
    udph->check = 0;

    psh.source_address = src_addr;
    psh.dest_address = sin.sin_addr.s_addr;
    psh.placeholder = 0;
    psh.protocol = IPPROTO_UDP;
    psh.udp_length = htons(sizeof(struct udphdr) + LEN);

    int psize = sizeof(struct pseudo_header) + sizeof(struct udphdr) + LEN;
    pseudogram = malloc(psize);

    memcpy(pseudogram, (char *)&psh, sizeof(struct pseudo_header));
    memcpy(pseudogram + sizeof(struct pseudo_header), udph, sizeof(struct udphdr) + LEN);

    udph->check = csum((unsigned short *)pseudogram, psize);
    int result = sendto(socket, datagram, iph->tot_len, 0, (struct sockaddr *)&sin, sizeof(sin));
    free(pseudogram);
    return result;
}
