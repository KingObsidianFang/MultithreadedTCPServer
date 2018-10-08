#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h> 
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <thread>
#include <vector>

#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"
#define MAXTHREADCOUNT 4

//Function Prototypes
int clientWork(SOCKET clientSocket);

int main()
{
	//Variables
	int iResult;//An int used in error checking
	WSADATA wsaData; //used to start up winsock
	addrinfo hints; //used to define what kind of connection we want
	addrinfo* result;  //used to store 
	SOCKET ListenSocket = NULL;//The socket we listen for connections on
	SOCKET ClientSocket; //The socket that represents our client connection
	int currentSoc = 0;
	std::vector<std::thread> ths;

	//Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData); //Initializes winsock & says we're using winsock 2.2, 
													//&wsadata is the address of a WSADATA that will be filled
													//with the details of the winsock implementation

	if (iResult != 0)//error check for winsock init
	{
		printf("WSAStartup failed with error: %d\n", WSAGetLastError);//prints error 
		WSACleanup();//clean up of winsock
		return 1;//exits program;  There's a pretty good chance that there's more we could do instead of just bail on the program, but i'm not sure yet
	}

	ZeroMemory(&hints, sizeof(hints));//zeroes memory to avoid garbage values in unfilled hints memory
	hints.ai_family = AF_INET; // says we're using IPV4 
	hints.ai_socktype = SOCK_STREAM; //defines a stream of data going both directions(client->server server->client)
	hints.ai_protocol = IPPROTO_TCP; //says we're using TCP as our protocol
	hints.ai_flags = AI_PASSIVE;// says we're going to use the socket in a bind() call
	//MSDN documents the addrinfo struct and will tell you more information


	//Getting information about addresses on our system
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result); //1st param is a host name(I don't understand this yet)
															//2nd param is a port
															//pointer to our hints struct telling what types of connections we want info for
															//pointer to a pointer that points to the addrinfo we want to store the info in
	if (iResult != 0)//error check, the rest will be the same as the init check refer back to that if you need explanation
	{
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);//makes a socket of the type we defined in hints
																					   //we use the socket later to listen for client connections
	if (ListenSocket == INVALID_SOCKET)//error check
	{
		printf("socket failed with error: %d\n", WSAGetLastError);
		freeaddrinfo(result);  //frees the memory used in the getaddrinfo return
		WSACleanup();
		return 1;
	}

	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen); //Binds our new socket to the port we defined

	if (iResult == SOCKET_ERROR)//error checking
	{
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket); //close the socket we just opened to free up resources
		WSACleanup();
		return 1;
	}

	freeaddrinfo(result);//frees up the memory used in getaddrinfo, we don't need this beacuse 
						//we've already bound the listening socket we want

	while (1) {//infinite loop to handle client accepts and start threads accordingly
		iResult = listen(ListenSocket, SOMAXCONN);
		if (iResult == SOCKET_ERROR)
		{
			printf("listen failed with error: %d\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			return 1;
		}
		ClientSocket = accept(ListenSocket, NULL, NULL); //Accepts the client connection on our current ClientSocket
		if (ClientSocket == INVALID_SOCKET) {//error checking
			printf("accept failed with error: %d\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			return 1;
		}
		else
		{
			ths.push_back(std::thread(clientWork, ClientSocket));
		}
	}
	closesocket(ListenSocket);
	WSACleanup();
}

int clientWork(SOCKET clientSocket)
{
	int iResult, iSendResult;
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	iResult = recv(clientSocket, recvbuf, recvbuflen, 0);
	if (iResult > 0) {
		printf("Message: %s\n", recvbuf);
		printf("Bytes received: %d\n", iResult);
		// Echo the buffer back to the sender
		iSendResult = send(clientSocket, recvbuf, iResult, 0);
		if (iSendResult == SOCKET_ERROR) {
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(clientSocket);
			WSACleanup();
			return 1;
		}
		printf("Bytes sent: %d\n", iSendResult);
	}
	else if (iResult == 0)
		printf("Connection closing...\n");
	else {
		printf("recv failed with error: %d\n", WSAGetLastError());
		closesocket(clientSocket);
		WSACleanup();
		return 1;
	}

	closesocket(clientSocket);
}