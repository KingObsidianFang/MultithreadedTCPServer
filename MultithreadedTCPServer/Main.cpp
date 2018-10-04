#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h> 
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

//Function Prototypes

int main()
{
	//Variables
	int iResult;//An int used in error checking
	WSADATA wsaData; //used to start up winsock
	addrinfo hints;  //used to define what kind of connection we want

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
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, NULL); //1st param is a host name(I don't understand this yet)
															//2nd param is a port
															//pointer to our hints struct telling what types of connections we want info for
															//pointer to addrinfo we want to store the info in
	if (iResult != 0)//error check, the rest will be the same as the init check refer back to that if you need explanation
	{
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}
}