#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

#include<arpa/inet.h>
#include<sys/socket.h>

#include<linux/if_packet.h>
#include<net/ethernet.h>

#include<netinet/ip.h>
#include<netinet/tcp.h>
#include<netinet/udp.h>
#include<netinet/ip_icmp.h>

#define BUFFER_SIZE 65536

void printMAC(unsigned char *mac)
{
    printf("%02X:%02X:%02X:%02X:%02X:%02X",
           mac[0],mac[1],mac[2],
           mac[3],mac[4],mac[5]);
}

int main()
{
    int sockfd;
    unsigned char *buffer;
    int data_size;

    struct sockaddr saddr;
    socklen_t saddr_len;

    buffer=(unsigned char *)malloc(BUFFER_SIZE);

    sockfd=socket(AF_PACKET,SOCK_RAW,htons(ETH_P_ALL));

    if(sockfd<0)
    {
        perror("Socket");
        return 1;
    }

    printf("\n==============================================\n");
    printf("        RAW SOCKET PACKET CAPTURE\n");
    printf("==============================================\n");
    printf("Press Ctrl+C to stop...\n\n");

    while(1)
    {
        saddr_len=sizeof(saddr);

        data_size=recvfrom(sockfd,
                           buffer,
                           BUFFER_SIZE,
                           0,
                           &saddr,
                           &saddr_len);

        if(data_size<0)
        {
            perror("recvfrom");
            break;
        }

        struct ethhdr *eth =
        (struct ethhdr *)buffer;

        printf("\n==============================================\n");
        printf("Packet Length : %d Bytes\n",data_size);

        printf("\nEthernet Header\n");
        printf("-----------------------------\n");

        printf("Destination MAC : ");
        printMAC(eth->h_dest);

        printf("\nSource MAC      : ");
        printMAC(eth->h_source);

        printf("\nProtocol        : 0x%04X\n",
               ntohs(eth->h_proto));

        if(ntohs(eth->h_proto)==ETH_P_IP)
        {
            struct iphdr *iph=
            (struct iphdr *)(buffer+
            sizeof(struct ethhdr));

            struct sockaddr_in src,dst;

            memset(&src,0,sizeof(src));
            memset(&dst,0,sizeof(dst));

            src.sin_addr.s_addr=iph->saddr;
            dst.sin_addr.s_addr=iph->daddr;

            printf("\nIP Header\n");
            printf("-----------------------------\n");

            printf("Source IP      : %s\n",
                   inet_ntoa(src.sin_addr));

            printf("Destination IP : %s\n",
                   inet_ntoa(dst.sin_addr));

            printf("TTL            : %d\n",
                   iph->ttl);

            printf("Protocol       : %d\n",
                   iph->protocol);

            if(iph->protocol==IPPROTO_TCP)
            {
                struct tcphdr *tcp=
                (struct tcphdr*)
                (buffer+
                sizeof(struct ethhdr)+
                iph->ihl*4);

                printf("\nTCP Header\n");
                printf("-----------------------------\n");

                printf("Source Port      : %u\n",
                       ntohs(tcp->source));

                printf("Destination Port : %u\n",
                       ntohs(tcp->dest));
            }

            else if(iph->protocol==IPPROTO_UDP)
            {
                struct udphdr *udp=
                (struct udphdr*)
                (buffer+
                sizeof(struct ethhdr)+
                iph->ihl*4);

                printf("\nUDP Header\n");
                printf("-----------------------------\n");

                printf("Source Port      : %u\n",
                       ntohs(udp->source));

                printf("Destination Port : %u\n",
                       ntohs(udp->dest));
            }

            else if(iph->protocol==IPPROTO_ICMP)
            {
                struct icmphdr *icmp=
                (struct icmphdr*)
                (buffer+
                sizeof(struct ethhdr)+
                iph->ihl*4);

                printf("\nICMP Header\n");
                printf("-----------------------------\n");

                printf("Type : %d\n",
                       icmp->type);

                printf("Code : %d\n",
                       icmp->code);
            }

            else
            {
                printf("\nOther Protocol\n");
            }
        }
    }

    close(sockfd);

    free(buffer);

    return 0;
}
