
#ifndef PKT_DEF_H
#define PKT_DEF_H



#include <stdio.h>
#include <iostream>
#include <iomanip>

#define FORWARD 1;
#define BACKWARD 2;
#define RIGHT 3;
#define LEFT 4;
#define UP 5;
#define DOWN 6;
#define OPEN 7;
#define CLOSE 8;

const int HEADERSIZE = 6;

struct Header
{
	int PktCount;
	unsigned char Drive : 1;
	unsigned char Status : 1;
	unsigned char Sleep : 1;
	unsigned char Arm : 1;
	unsigned char Claw : 1;
	unsigned char Ack : 1;
	unsigned char Padding : 2; //Padding for unsigned char
	unsigned char Length;
};

struct MotorBody
{
	char Direction;
	char Duration;
};


enum CmdType { DRIVE = 1, SLEEP, ARM, CLAW, ACK, STATUS, NACK };


class PktDef
{
private:
	struct CmdPacket
	{
		Header header;
		char* Data;
		unsigned char CRC;
	}my_cmdpacket;

	char* RawBuffer;

public:
	//all member functions
	PktDef();
	PktDef(char *);
	void SetCmd(CmdType);
	void SetBodyData(char *, int);
	void SetPktCount(int);
	CmdType GetCmd();
	bool GetAck();
	int GetLength();
	char *GetBodyData();
	int GetPktCount();
	bool CheckCRC(char *, int);
	void CalcCRC();
	char* GenPacket();
};

#endif