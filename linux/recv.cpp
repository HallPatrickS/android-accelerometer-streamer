#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <iostream>
#include <fstream>

#define PORT "5005"

#define MAXBUFLEN 512 

typedef struct {
    float x, y, z;
    char time[26];
} accel_data;


void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

float ntohf(uint32_t p) {
    float f = ((p>>16)&0x7fff); // whole part
    f += (p&0xffff) / 65536.0f; // fraction

    if (((p>>31)&0x1) == 0x1) { f = -f; } // sign bit set
    return f;
}


int main(int argc, char **argv) {
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;
    struct sockaddr_storage their_addr;
    char buf[MAXBUFLEN];
    socklen_t addr_len;
    char s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // set AF_INET to force IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; //use my ip

    rv = getaddrinfo(NULL, PORT, &hints, &servinfo);
    if (rv != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                        p->ai_protocol)) == -1) {
            perror("listner: socket");
            continue;
        }
        int b = bind(sockfd, p->ai_addr, p->ai_addrlen);
        if (b == -1) {
            close(sockfd);
            perror("listner: bind");
            continue;
        }
        break;
    }

    if (p == NULL) {
        fprintf(stderr, "listner: failed to bind socket\n");
        return 2;
    }

    freeaddrinfo(servinfo);

    printf("listner: waiting to recvfrom...\n");

    accel_data xyz_data;

    std::ofstream log("accel_data.txt");
    while (1) {
        memset(&xyz_data, 0, sizeof(xyz_data));
        addr_len = sizeof their_addr;
        if ((numbytes = recvfrom(sockfd, &xyz_data, MAXBUFLEN-1, 0,
                        (struct sockaddr *)&their_addr, &addr_len)) == -1) {
            perror("recvfrom");
            exit(1);
        }

        // std::cout << "Date: " << xyz_data.time << std::endl;
        printf("Date: %s\n", xyz_data.time);
        std::cout << "x: " << xyz_data.x << std::endl;
        std::cout << "y: " << xyz_data.y << std::endl;
        std::cout << "z: " << xyz_data.z << std::endl;

        log << xyz_data.x << ", " 
            << xyz_data.y << ", " 
            << xyz_data.z << ", "
            << xyz_data.time << "\n";

        printf("listner: got packet from %s\n",
                inet_ntop(their_addr.ss_family,
                    get_in_addr((struct sockaddr *)&their_addr),
                    s, sizeof s));
        printf("listner: packet is %d bytes long\n", numbytes);
        //buf[numbytes] = '\0';
        //printf("listern: packet contains \"%s\"\n", buf);

        memset(buf, 0, MAXBUFLEN);
        flush(log);
    }
    close(sockfd);

    return 0;
}
