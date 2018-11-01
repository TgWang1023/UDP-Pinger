/* 
    Tiange Wang, ID: 3717659, CS 176A Homework 2
    Code Cited: http://www.linuxhowtos.org/data/6/client_udp.c, Sockets Tutorial, Example UDP Client.
*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void error(const char *);
int main(int argc, char *argv[])
{
    int sock, n;
    unsigned int length;
    struct sockaddr_in server, from;
    struct hostent *hp;
   
    if (argc != 3) { 
        printf("Usage: ./PingClient host port\n");
        exit(1);
    }
    printf("\n");

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) error("socket");
    struct timeval timeout = {1, 0};
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(struct timeval)) == -1) {
        error("setsockopt");
        exit(1);
    }

    server.sin_family = AF_INET;
    hp = gethostbyname(argv[1]);
    if (hp==0) error("Unknown host");

    bcopy((char *)hp->h_addr, (char *)&server.sin_addr, hp->h_length);
    server.sin_port = htons(atoi(argv[2]));
    length=sizeof(struct sockaddr_in);

    int loss_count = 0;
    int time_recorded = 0;
    double min = 0.0;
    double max = 0.0;
    double total = 0.0;
    for(int i = 0; i < 10; i++) {
        char pingString[256];
        char timeString[256];
        char buffer[256];

        time_t rawtime;
        struct tm * timeinfo;
        time(&rawtime);
        timeinfo = localtime(&rawtime);
        struct timespec start, stop;
        double accum;
        if(clock_gettime(CLOCK_REALTIME, &start) == -1) {
            perror("clock gettime");
            exit(EXIT_FAILURE);
        }

        sprintf(pingString, "PING %d ", i);
        strftime(timeString, sizeof(timeString), "%H:%M:%S", timeinfo);
        strcpy(buffer, pingString);
        strcat(buffer, timeString);
        strcat(buffer, "\n");

        n = sendto(sock, buffer, strlen(buffer), 0, (const struct sockaddr *)&server, length);
        if (n < 0) error("sendto");
        n = recvfrom(sock, buffer, 256, 0, (struct sockaddr *)&from, &length);
        if (n < 0) {
            loss_count++;
            printf("Request timeout.\n"); 
            continue;
        }
        if(clock_gettime(CLOCK_REALTIME, &stop) == -1) {
            perror("clock gettime");
            exit(EXIT_FAILURE);
        } 
        accum = ( stop.tv_sec - start.tv_sec ) * 1000.00 + ( stop.tv_nsec - start.tv_nsec ) / 1000000.00;
        if(time_recorded == 0) {
            max = accum;
            min = accum;
        }
        time_recorded = 1;
        if(accum > max) max = accum;
        if(accum < min) min = accum;
        total += accum;
        sleep(1);

        printf("Ping Received From %s: ", argv[1]);
        printf("seq#=%i ", i);
        printf("time=%.*f ms\n", 3, accum);
        bzero(buffer, 256);
    }

    int received = 10 - loss_count;
    double percent = (loss_count / 10.0) * 100.0;
    printf("--- ping statistics ---\n");
    printf("10 packets transmitted, %i packets received, %.*f%% packet loss\n", received, 0, percent);
    if(time_recorded == 1) {
        double avg = total / received;
        printf("round-trip min/avg/max = %.*f/%.*f/%.*f ms\n", 3, min, 3, avg, 3, max);
    }
    printf("\n");

    close(sock);
    return 0;
}

void error(const char *msg)
{
    perror(msg);
    exit(0);
}
