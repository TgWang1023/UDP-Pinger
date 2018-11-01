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

    for(int i = 0; i < 10; i++) {
        char pingString[256];
        char timeString[256];
        char buffer[256];

        time_t rawtime;
        struct tm * timeinfo;
        time(&rawtime);
        timeinfo = localtime(&rawtime);

        sprintf(pingString, "PING %d ", i);
        strftime(timeString, sizeof(timeString), "%H:%M:%S", timeinfo);
        strcpy(buffer, pingString);
        strcat(buffer, timeString);
        strcat(buffer, "\n");

        n = sendto(sock, buffer, strlen(buffer), 0, (const struct sockaddr *)&server, length);
        if (n < 0) error("sendto");
        n = recvfrom(sock, buffer, 256, 0, (struct sockaddr *)&from, &length);
        if (n < 0) {
            printf("Request timeout.\n"); 
            continue;
        }
        sleep(1);

        printf("Ping Received From %s: ", argv[1]);
        printf(buffer, "%s");
        bzero(buffer, 256);
    }
    printf("--- ping statistics ---\n");
    printf("X packets transmitted, Y packets received, Z%% packet loss\n");
    printf("round-trip min/avg/max = MIN/AVG/MAX ms\n\n");

    close(sock);
    return 0;
}

void error(const char *msg)
{
    perror(msg);
    exit(0);
}