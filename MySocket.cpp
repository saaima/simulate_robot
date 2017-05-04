#include "MySocket.h"
#include <string>


MySocket::MySocket(SocketType socket_t, std::string ip, unsigned int port, ConnectionType connection_t, unsigned int size)
{
	mySocket = socket_t;
	IPAddr = ip;
	Port = port;
	connectionType = connection_t;
	Buffer = new char[size];
	MaxSize = DEFAULT_SIZE;

	if (mySocket == SERVER)
	{

		if ((WSAStartup(MAKEWORD(2, 2), &this->wsaData)) != 0) {
			std::cout << "Could not start DLLs" << std::endl;
			std::cin.get();
			exit(0);
		}

		//****SOCKET****
		WelcomeSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (WelcomeSocket == INVALID_SOCKET)
		{
			std::cout << "Could not initialize socket" << std::endl;
			std::cin.get();
			WSACleanup();
		}

		// ****BIND****
		struct sockaddr_in SvrAddr;
		SvrAddr.sin_family = AF_INET; //Address family type internet
		SvrAddr.sin_port = htons(this->Port); //port (host to network conversion)
		SvrAddr.sin_addr.s_addr = inet_addr(this->IPAddr.c_str()); //IP address
		if ((bind(this->WelcomeSocket, (struct sockaddr *)&SvrAddr, sizeof(SvrAddr))) == SOCKET_ERROR) {
			closesocket(this->WelcomeSocket);
			WSACleanup();
			std::cout << "Could not bind to the socket" << std::endl;
			std::cin.get();
		}

		//****LISTEN****
		if (listen(this->WelcomeSocket, 1) == SOCKET_ERROR)
		{
			closesocket(this->WelcomeSocket);
			WSACleanup();
			std::cout << "Could not listen to the provided socket." << std::endl;
			std::cin.get();
		}
		else
		{
			bTCPConnect = true;
			std::cout << "Waiting for client connection" << std::endl;
		}



		//****ACCEPT****
		ConnectionSocket = SOCKET_ERROR;
		if ((this->ConnectionSocket = accept(this->WelcomeSocket, NULL, NULL)) == SOCKET_ERROR)
		{
			closesocket(this->WelcomeSocket);
			WSACleanup();
			std::cout << "Could not accept incoming connection." << std::endl;
			std::cin.get();
			exit(0);
		}
		else
		{
			std::cout << "Connection Accepted" << std::endl;
		}

	}
	else if (mySocket == CLIENT)
	{
		if ((WSAStartup(MAKEWORD(2, 2), &this->wsaData)) != 0) {
			std::cout << "Could not start DLLs" << std::endl;
			std::cin.get();
			exit(0);
		}

		//****SOCKET****
		ConnectionSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (ConnectionSocket == INVALID_SOCKET)
		{
			std::cout << "Could not initialize socket" << std::endl;
			std::cin.get();
			WSACleanup();
		}
	}
}

MySocket::~MySocket()
{
	delete[] Buffer;
}

void MySocket::ConnectTCP()
{
	struct sockaddr_in SvrAddr;
	SvrAddr.sin_family = AF_INET;
	SvrAddr.sin_port = htons(this->Port);
	SvrAddr.sin_addr.s_addr = inet_addr(this->IPAddr.c_str());
	if ((connect(this->ConnectionSocket, (struct sockaddr*)&SvrAddr, sizeof(SvrAddr))) == SOCKET_ERROR)
	{
		closesocket(this->ConnectionSocket);
		WSACleanup();
		std::cout << "Could not connect to the server" << std::endl;
		std::cin.get();
	}
	else
	{
		std::cout << "Connected" << std::endl;
	}
}

void MySocket::DisconnectTCP()
{
	closesocket(WelcomeSocket);
	bTCPConnect = false;
}

void MySocket::SendData(const char* data, int size)
{
	if (connectionType == 1)
	{
		send(ConnectionSocket, data, size, 0);
	}
	else if (connectionType == 2)
	{
		sendto(ConnectionSocket, data, size, 0, (struct sockaddr *)&SvrAddr, sizeof(SvrAddr));
	}
}

int MySocket::GetData(char* data)
{
	int len;
	if (connectionType == 1)
	{
		len = recv(this->ConnectionSocket, Buffer, MaxSize, 0);
	}
	else
	{
		//recvfrom(this->client_socket, this->rx_buffer, sizeof(this->rx_buffer), 0, (struct sockaddr *) &CltAddr, &addr_len);
	}

	memcpy(data, Buffer, len);



	return len;

}

std::string MySocket::GetIPAddr()
{
	return IPAddr;
}

void MySocket::SetIPAddr(std::string ip)
{
	IPAddr = ip;
}

void MySocket::SetPort(int port)
{
	Port = port;
}

int MySocket::GetPort()
{
	return Port;
}

SocketType MySocket::GetType()
{
	return mySocket;
}

void MySocket::SetType(SocketType socket)
{
	mySocket = socket;
}
