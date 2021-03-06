#include "stdafx.h"
#include <stdio.h>
#include <fstream>
#include <winsock2.h>
#include <iostream>
#include <sstream>

#pragma comment(lib,"ws2_32.lib") //Winsock lib

#define BUFLEN 512
int PORT = 5005;

typedef struct {
	float x, y, z;
	char time[26];
} accel_data;

int main(int argc, char **argv) {

	if (argc > 2) {
		std::cout << "Usage: [port number]\n";
	}
	if (argv[1] != NULL) {
		std::cout << "Using port " << argv[1] << std::endl;
		std::stringstream port(argv[1]);
		port >> PORT;
	}

	SOCKET sockfd;
	struct sockaddr_in server, si_other;
	int slen, recv_len;
	WSADATA wsa;

	slen = sizeof(si_other);

	// Initialise winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		printf("Failed, Error Code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	printf("Initialized.\n");

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) {
		printf("Could not creatae socket : %d", WSAGetLastError());
	}
	printf("Socket created.\n");

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(PORT);

	//Bind
	if (bind(sockfd, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
		printf("Bind error with socket");
		exit(EXIT_FAILURE);
	}
	puts("Bind done");

	accel_data xyz_data;
	std::ofstream log("accel_data.txt");
	printf("Waiting for data");

	while (1) {
		memset(&xyz_data, 0, sizeof(xyz_data));

		if ((recv_len = recvfrom(sockfd, (char *)&xyz_data, BUFLEN, 0,
			(struct sockaddr *)&si_other, &slen)) == SOCKET_ERROR) {
			printf("recvfrom() failed with error code : %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}

		std::cout << xyz_data.x << ", "
			<< xyz_data.y << ", "
			<< xyz_data.z << ", "
			<< xyz_data.time << std::endl;
		flush(log);
	}
	closesocket(sockfd);
	WSACleanup();
	return 0;
}