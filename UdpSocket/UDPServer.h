#pragma once
#include <iostream>
#include <winsock2.h>
#include "..\..\Common\IDD.h"
//#define BUFLEN 512
//#define PORT 8888
#pragma comment(lib,"ws2_32.lib") // Winsock Library
#pragma warning(disable:4996)
using namespace std;
class UDPServer
{
private:
	sockaddr_in server, client;
	WSADATA wsa;
	SOCKET server_socket;
public:	
	UDPServer() {	}
	UDPServer(unsigned short port) {
		initializeWinsock();
		createSocket();
		tryBind(port);
	}
	void initializeWinsock() {
		printf("Initialising UDPServer Winsock...");
		if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		{
			printf("Failed. Error Code: %d", WSAGetLastError());
			return;
			//exit(0);//replace to return
		}
		printf("Initialised.\n");
	}
	void createSocket() {
		if ((server_socket = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
		{
			printf("Could not create socket: %d", WSAGetLastError());
			//exit(0); ???
		}
		printf("Socket created.\n");
	}
	void prepareTheSockaddr_inStructure(unsigned short port) {
		server.sin_family = AF_INET;
		server.sin_addr.s_addr = INADDR_ANY;
		
		server.sin_port = htons(port);

	}
	void tryBind(unsigned short port) {
		prepareTheSockaddr_inStructure(port);
		// bind
		if (bind(server_socket, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
		{
			printf("Bind failed with error code: %d", WSAGetLastError());
			return;
			//exit(EXIT_FAILURE);
		}
		puts("Bind done.");
	}
	void changePort(unsigned short port) {
		server.sin_port = htons(port);
	}
	int recieveData(Message* buf, unsigned short port) {//run this only if tryBind() true
		server.sin_port = htons(port);
		printf("Waiting for data...\n");
		//fflush(stdout);
		// try to receive some data, this is a blocking call
		int message_len;
		int slen = sizeof(sockaddr_in);
		if (message_len = recvfrom(server_socket, (char*)buf, sizeof(Message), 0, (sockaddr*)&client, &slen) == SOCKET_ERROR)
		{
			printf("recvfrom() failed with error code: %d\n", WSAGetLastError());
			return -1;
			// print details of the client/peer and the data received
			printf("Received packet from %s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
		}
		printf("\nget data...\n");

		return 0;
	}
	/*~UDPServer() {
		closesocket(server_socket);
		WSACleanup();
	}*/
};