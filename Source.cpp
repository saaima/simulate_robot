#include "MySocket.h"
#include "Pkt_Def.h"

#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <thread>
#include <cstdlib>

bool ExeComplete = false;

void CommandThread(std::string ip, int port)
{
	MySocket CommandSocket(SocketType::CLIENT, ip, port, ConnectionType::TCP, 100);

	CommandSocket.ConnectTCP();

	PktDef TxPkt, RxPkt;
	int command, direction;
	unsigned int duration;
	int packet_count = 0;
	MotorBody motorbody;
	bool end_cmd_loop = false;

	do
	{
		std::cout << "Enter Command Number : \n\n 1 - Drive \n 2 - Sleep \n 3 - Arm \n 4 - Claw \n 5 - Ack \n";
		std::cin >> command;

		switch (command)
		{

		case CmdType::DRIVE:



			std::cout << "Enter Number for Direction to Drive \n\n 1- Forward \n 2- Backward \n 3- Right \n 4- Left \n";
			std::cin >> direction;
			std::cout << "Enter Length of Drive in Seconds \n";
			std::cin >> duration;

			TxPkt.SetCmd(CmdType::DRIVE);

			switch (direction)
			{
			case 1:
				motorbody.Direction = FORWARD;
				break;

			case 2:
				motorbody.Direction = BACKWARD;
				break;

			case 3:
				motorbody.Direction = RIGHT;
				break;

			case 4:
				motorbody.Direction = LEFT;
				break;
			}

			motorbody.Duration = duration;
			TxPkt.SetBodyData((char *)&motorbody, 2);

			TxPkt.CalcCRC();
			break;



		case  CmdType::SLEEP:



			TxPkt.SetCmd(CmdType::SLEEP);

			motorbody.Direction = 0;
			motorbody.Duration = 0;
			TxPkt.SetBodyData((char *)&motorbody, 2);

			TxPkt.CalcCRC();
			break;



		case  CmdType::ARM:



			
			std::cout << "Enter Number for Arm Direction \n 1- UP \n 2- DOWN \n";
			std::cin >> direction;

			TxPkt.SetCmd(CmdType::ARM);

			switch (direction)
			{
			case 1:
				motorbody.Direction = UP;
				break;

			case 2:
				motorbody.Direction = DOWN;
				break;
			}

			motorbody.Duration = 0;
			TxPkt.SetBodyData((char *)&motorbody, 2);

			TxPkt.CalcCRC();
			break;



		case  CmdType::CLAW:



			std::cout << "Enter Command Number for Claw \n 1- OPEN \n 2-CLOSE \n";
			std::cin >> direction;

			TxPkt.SetCmd(CmdType::CLAW);

			switch (direction)
			{
			case 1:
				motorbody.Direction = OPEN;
				break;

			case 2:
				motorbody.Direction = CLOSE;
				break;
			}

			motorbody.Duration = 0;
			TxPkt.SetBodyData((char *)&motorbody, 2);

			TxPkt.CalcCRC();
			break;



		case  CmdType::ACK:



			TxPkt.SetCmd(CmdType::ACK);

			motorbody.Direction = 0;
			motorbody.Duration = 0;
			TxPkt.SetBodyData((char *)&motorbody, 2);

			TxPkt.CalcCRC();
			break;


		}

		char * buffer = TxPkt.GenPacket();

		CommandSocket.SendData(buffer, TxPkt.GetLength());

		delete buffer;

		buffer = new char[DEFAULT_SIZE];

		CommandSocket.GetData(buffer);

		RxPkt = PktDef(buffer);

		if ((TxPkt.GetCmd() == CmdType::SLEEP) && (RxPkt.GetCmd() == CmdType::SLEEP) && RxPkt.GetAck())
		{
			end_cmd_loop = true;
		}

	} while (!end_cmd_loop);

	CommandSocket.DisconnectTCP();
	ExeComplete = true;
}


void TelemetryThread(std::string ip, int port)
{
	MySocket TelemetrySocket(SocketType::CLIENT, ip, port, ConnectionType::TCP, 100);

	TelemetrySocket.ConnectTCP();

	bool check_crc = false;
	bool check_header = false;

	while (true)
	{
		char * buffer;
		buffer = new char[DEFAULT_SIZE];

		TelemetrySocket.GetData(buffer);

		PktDef RxPkt(buffer);

		check_crc = RxPkt.CheckCRC(buffer, RxPkt.GetLength());

		if (RxPkt.GetCmd() == CmdType::STATUS)
		{
			check_header = true;
		}

		if (check_crc && check_header)
		{
			int sonar=0, arm_position=0;
			char * ptr = RxPkt.GetBodyData();

			memcpy(&sonar, ptr, 2);

			ptr += 2;

			memcpy(&arm_position, ptr, 2);

			ptr += 2;

			std::cout << "\n\nSonar Data: " << sonar << std::endl;
			std::cout << "Arm Position: " << arm_position << std::endl;

			if ((*ptr) & 0x01)
			{
				std::cout << "Drive: " << (bool)*ptr << std::endl;
			}
			else
			{
				std::cout << "Drive:  0" << std::endl;
			}


			
			if (((*ptr >> 1) & 0x01) && ((*ptr >> 3) & 0x01))
			{
				std::cout << "Arm is Up, Claw is Open";
			}
			else if (((*ptr >> 1) & 0x01) && ((*ptr >> 4) & 0x01))
			{
				std::cout << "Arm is Up, Claw is Closed";
			}
			else if (((*ptr >> 2) & 0x01) && ((*ptr >> 3) & 0x01))
			{
				std::cout << "Arm is Down, Claw is Open";
			}
			else if (((*ptr >> 2) & 0x01) && ((*ptr >> 4) & 0x01))
			{
				std::cout << "Arm is Down, Claw is Closed";
			}
		}
	}
}


int main(int argc, char** argv)
{
		int c_port = atoi(argv[2]);
		int t_port = atoi(argv[3]);


		std::thread t1(CommandThread, argv[1], c_port);
		std::thread t2(TelemetryThread, argv[1], t_port);
		t1.detach();
		t2.detach();

		while (!ExeComplete) {};
}