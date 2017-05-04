#ifndef MYSOCKET_H
#define MYSOCKET_H

#include <windows.networking.sockets.h>
#pragma comment(lib, "Ws2_32.lib")
#include <iostream>

const int DEFAULT_SIZE = 128;

enum SocketType
{
	CLIENT = 1, SERVER
};

enum ConnectionType
{
	TCP = 1, UDP
};

class MySocket
{
	char * Buffer;
	SOCKET WelcomeSocket, ConnectionSocket;
	sockaddr_in SvrAddr;
	SocketType mySocket;
	std::string IPAddr;
	int Port;
	ConnectionType connectionType;
	bool bTCPConnect = false;
	int MaxSize;
	WSADATA wsaData;

public:
	MySocket(SocketType, std::string, unsigned int, ConnectionType, unsigned int);
	~MySocket();
	void ConnectTCP();
	void DisconnectTCP();
	void SendData(const char*, int);
	int GetData(char*);
	std::string GetIPAddr();
	void SetIPAddr(std::string);
	void SetPort(int);
	int GetPort();
	SocketType GetType();
	void SetType(SocketType);
};

#endif
#pragma once
