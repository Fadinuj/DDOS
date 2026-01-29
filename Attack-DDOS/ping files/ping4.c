#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <math.h>
#include <signal.h>

#define TARGET_IP "192.168.1.55" 
#define PING_INTERVAL 5           
#define MAX_PINGS 10000         
#define ICMP_ECHO 8
#define ICMP_CODE 0

double rtt_times[MAX_PINGS];
int count = 0;
int running = 1;


void handle_sigint(int sig) {
    running = 0;
}


unsigned short checksum(void *b, int len) {
    unsigned short *buf = b;
    unsigned int sum = 0;
    unsigned short result;

   
    for (sum = 0; len > 1; len -= 2) {
        sum += *buf++;
    }
    if (len == 1) {
        sum += *(unsigned char *)buf; 
    }
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

int main() {
    int sockfd;
    struct sockaddr_in addr;
    struct icmp icmp_hdr;
    char recv_buf[1024];
    struct timeval start_time, end_time;
    double rtt, total_rtt = 0.0, average_rtt, std_dev = 0.0;

    signal(SIGINT, handle_sigint);

    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        perror("Socket creation failed");
        return 1;
    }

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(TARGET_IP);

    printf("Starting real ping monitor... (Press Ctrl+C to stop)\n");

    while (running) {
        memset(&icmp_hdr, 0, sizeof(icmp_hdr));
        icmp_hdr.icmp_type = ICMP_ECHO;
        icmp_hdr.icmp_code = ICMP_CODE;
        icmp_hdr.icmp_id = getpid() & 0xFFFF;
        icmp_hdr.icmp_seq = count;
        icmp_hdr.icmp_cksum = checksum(&icmp_hdr, sizeof(icmp_hdr));

        gettimeofday(&start_time, NULL);

        if (sendto(sockfd, &icmp_hdr, sizeof(icmp_hdr), 0,
                   (struct sockaddr *)&addr, sizeof(addr)) <= 0) {
            perror("Sendto error");
        }

        socklen_t addr_len = sizeof(addr);
        if (recvfrom(sockfd, recv_buf, sizeof(recv_buf), 0,
                     (struct sockaddr *)&addr, &addr_len) <= 0) {
            perror("Recvfrom error");
        }

        gettimeofday(&end_time, NULL);

        rtt = (end_time.tv_sec - start_time.tv_sec) * 1000.0; 
        rtt += (end_time.tv_usec - start_time.tv_usec) / 1000.0; 

        if (count < MAX_PINGS) {
            rtt_times[count] = rtt;
            total_rtt += rtt;
            printf("Ping %d: %.2f ms\n", count + 1, rtt);
            count++;
        }

        sleep(PING_INTERVAL);
    }

    if (count == 0) {
        printf("No pings recorded.\n");
        close(sockfd);
        return 1;
    }

    average_rtt = total_rtt / count;

    for (int i = 0; i < count; i++) {
        std_dev += pow(rtt_times[i] - average_rtt, 2);
    }
    std_dev = sqrt(std_dev / count);

    FILE *output = fopen("pings_results1_c.txt", "w");
    if (output == NULL) {
        perror("File open failed");
        close(sockfd);
        return 1;
    }

    for (int i = 0; i < count; i++) {
        fprintf(output, "Ping %d: %f ms\n", i + 1, rtt_times[i]);
    }
    fprintf(output, "\nAverage RTT: %f ms\n", average_rtt);
    fprintf(output, "Standard Deviation: %f ms\n", std_dev);

    fclose(output);
    close(sockfd);

    printf("\nDone. Total pings: %d\n", count);
    printf("Average RTT: %.2f ms\n", average_rtt);
    printf("Standard Deviation: %.2f ms\n", std_dev);

    return 0;
}
