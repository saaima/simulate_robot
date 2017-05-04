#include "Pkt_Def.h"

PktDef::PktDef()
{
	RawBuffer = nullptr;

	my_cmdpacket.header.PktCount = 0;

	my_cmdpacket.header.Drive = 0;
	my_cmdpacket.header.Status = 0;
	my_cmdpacket.header.Sleep = 0;
	my_cmdpacket.header.Arm = 0;
	my_cmdpacket.header.Claw = 0;
	my_cmdpacket.header.Ack = 0;
	my_cmdpacket.header.Padding = 0;
	my_cmdpacket.header.Length = 0;

	my_cmdpacket.Data = nullptr;
	my_cmdpacket.CRC = 0;
}

PktDef::PktDef(char * data)
{
	char *ptr = data;

	memcpy(&my_cmdpacket.header.PktCount, ptr, sizeof(int));

	ptr += sizeof(int);

	my_cmdpacket.header.Drive = (*ptr) & 0x01;
	my_cmdpacket.header.Status = (*ptr >> 1) & 0x01;
	my_cmdpacket.header.Sleep = (*ptr >> 2) & 0x01;
	my_cmdpacket.header.Arm = (*ptr >> 3) & 0x01;
	my_cmdpacket.header.Claw = (*ptr >> 4) & 0x01;
	my_cmdpacket.header.Ack = (*ptr >> 5) & 0x01;

	ptr += sizeof(char);

	memcpy(&my_cmdpacket.header.Length, ptr, sizeof(char));

	ptr += sizeof(char);

	this->SetBodyData(ptr, (my_cmdpacket.header.Length - HEADERSIZE - 1));

	ptr += sizeof(my_cmdpacket.header.Length - HEADERSIZE - 1);

	CalcCRC();
}


void PktDef::SetCmd(CmdType c)
{
	switch (c)
	{
	case 1:
		my_cmdpacket.header.Drive  = 1;
		my_cmdpacket.header.Status = 0;
		my_cmdpacket.header.Sleep  = 0;
		my_cmdpacket.header.Arm    = 0;
		my_cmdpacket.header.Claw   = 0;
		my_cmdpacket.header.Ack    = 0;

		break;
	case 2:
		my_cmdpacket.header.Drive  = 0;
		my_cmdpacket.header.Status = 0;
		my_cmdpacket.header.Sleep  = 1;
		my_cmdpacket.header.Arm    = 0;
		my_cmdpacket.header.Claw   = 0;
		my_cmdpacket.header.Ack    = 0;

		break;
	case 3:
		my_cmdpacket.header.Drive  = 0;
		my_cmdpacket.header.Status = 0;
		my_cmdpacket.header.Sleep  = 0;
		my_cmdpacket.header.Arm    = 1;
		my_cmdpacket.header.Claw   = 0;
		my_cmdpacket.header.Ack    = 0;

		break;
	case 4:
		my_cmdpacket.header.Drive  = 0;
		my_cmdpacket.header.Status = 0;
		my_cmdpacket.header.Sleep  = 0;
		my_cmdpacket.header.Arm    = 0;
		my_cmdpacket.header.Claw   = 1;
		my_cmdpacket.header.Ack    = 0;

		break;
	case 5:
		my_cmdpacket.header.Drive  = 0;
		my_cmdpacket.header.Status = 0;
		my_cmdpacket.header.Sleep  = 0;
		my_cmdpacket.header.Arm    = 0;
		my_cmdpacket.header.Claw   = 0;
		my_cmdpacket.header.Ack    = 1;

		break;
	}
}

void PktDef::SetBodyData(char * data, int size)
{
	my_cmdpacket.header.Length = HEADERSIZE + size + 1;

	my_cmdpacket.Data = new char[size];

	for (int i = 0; i < size; i++)
	{
		my_cmdpacket.Data[i] = *data++;
	}
}


CmdType PktDef::GetCmd()
{
	if (my_cmdpacket.header.Drive)
	{
		return DRIVE;
	}
	else if (my_cmdpacket.header.Sleep)
	{
		return SLEEP;
	}
	else if (my_cmdpacket.header.Arm)
	{
		return ARM;
	}
	else if (my_cmdpacket.header.Claw)
	{
		return CLAW;
	}
	else if (my_cmdpacket.header.Ack)
	{
		return ACK;
	}
	else if (my_cmdpacket.header.Status)
	{
		return STATUS;
	}
	else
	{
		return NACK;
	}
}


void PktDef::SetPktCount(int count)
{
	my_cmdpacket.header.PktCount = count;
}

bool PktDef::GetAck()
{
	return (my_cmdpacket.header.Ack == 1 ? true : false);
}

int PktDef::GetLength()
{
	return (int)my_cmdpacket.header.Length;
}

char* PktDef::GetBodyData()
{
	return this->my_cmdpacket.Data;
}

int PktDef::GetPktCount()
{
	return my_cmdpacket.header.PktCount;
}

bool PktDef::CheckCRC(char* data, int size)
{
	char * ptr = data;
	bool return_v = false;
	int count = 0;

	for (int i = 0; i < size - 1; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			count += (*ptr >> j) & 0x01;
		}

		ptr += sizeof(char);
	}

	if (count == (int)*ptr)
	{
		return_v = true;
	}

	return return_v;
}

void PktDef::CalcCRC()
{
	char * ptr = (char*)&this->my_cmdpacket.header;
	int count = 0;

	for (int i = 0; i < HEADERSIZE; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			count += (*ptr >> j) & 0x01;
		}

		ptr += sizeof(char);
	}

	ptr = this->my_cmdpacket.Data;

	int size = this->my_cmdpacket.header.Length - HEADERSIZE - 1;


	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			count += (*ptr >> j) & 0x01;
		}

		ptr++;
	}

	this->my_cmdpacket.CRC = count;
}

char* PktDef::GenPacket()
{
	int size = my_cmdpacket.header.Length;

	RawBuffer = new char[size];
	char* ptr = RawBuffer;

	memcpy(ptr, &my_cmdpacket.header, HEADERSIZE);

	unsigned char* auxPtr = (unsigned char*)&my_cmdpacket.Data[0];
	for (int i = 0; i < 2; i++) {
		ptr[HEADERSIZE + i] = *(auxPtr++);
	}

	ptr[HEADERSIZE + 2] = my_cmdpacket.CRC;

	return ptr;
}