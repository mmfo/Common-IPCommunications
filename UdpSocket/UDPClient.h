#pragma once
#include <iostream>
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")
#pragma warning(disable:4996)

using namespace std;
#define SERVER "127.0.0.1"  // or "localhost" - ip address of UDP server
//#define BUFLEN 512  // max length of answer
//#define PORT 8888  // the port on which to listen for incoming data
class UDPClient
{
private:
	WSADATA ws;
	sockaddr_in server;
	int client_socket;
public:
	UDPClient() {}
	UDPClient(int x) {
		initialiseWinsock();
		createSocket();
	}
	//~UDPClient() {
	//	closesocket(client_socket);
	//	//WSACleanup();
	//}
	void initialiseWinsock() {
		printf("Initialising UDPClient Winsock...");
		if (WSAStartup(MAKEWORD(2, 2), &ws) != 0)
		{
			printf("Failed. Error Code: %d", WSAGetLastError());
			//return 1;
			return;
			//exit(0);
		}
		printf("Initialised.\n");
	}
	void createSocket() {
		if ((client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR) // <<< UDP socket
		{
			printf("socket() failed with error code: %d", WSAGetLastError());
			//return 2;
			//exit(0);
			return;
		}
	}
	void setupAddressStructure(unsigned short port) {
		//memset((char*)&server, 0, sizeof(server));
		server.sin_family = AF_INET;
		server.sin_port = htons(port);
		server.sin_addr.S_un.S_addr = inet_addr(SERVER);
	}
	int sendTheMessage(Message message, unsigned short port) {
		setupAddressStructure(port);
		if (sendto(client_socket, (char*)&message, sizeof(message), 0, (sockaddr*)&server, sizeof(sockaddr_in)) == SOCKET_ERROR)
		{
			printf("sendto() failed with error code: %d", WSAGetLastError());
			//return 3;
			return -1;
			//exit(0);
		} 
		return 0;
	}
};
