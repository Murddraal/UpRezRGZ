#include <windows.h>
#include <stdio.h>
#include <string>
using namespace std;
extern "C" _declspec(dllexport) string GetInformation()
{
	
	string type = "";
	switch (GetKeyboardType(0))
	{
		case 1:
		{type = "IBM PC/XT or compatible (83-key) keyboard";
		break;		}
		case 2:		
		{type = "Olivetti 'ICO' (102-key) keyboard";
		break;	}
		case 3:		
		{type = "IBM PC/AT (84-key) or similar keyboard";
		break;		}
		case 4:		
		{type = "IBM enhanced (101- or 102-key) keyboard";
		break;		}
		case 5:		
		{type = "Nokia 1050 and similar keyboards";
		break;		}
		case 6:		
		{type = "Nokia 9140 and similar keyboards";
		break;		}
		case 7:		
		{type = "Japanese keyboard";
		break;		}
	}

	return type;
}
void GetCpuid(int CPUInfo[4], int InfoType)
{
	int IEAX, IEBX, IECX, IEDX;
	_asm
	{
		MOV EAX, InfoType //��������� ������� ������� CPUID
			CPUID	//����� ���������� ����������
			//���������� � ��������� ���������� 
			//���������� CPUID ����������
			MOV IEAX, EAX
			MOV IEBX, EBX
			MOV IECX, ECX
			MOV IEDX, EDX
	}
	//������ � ���������� � ������� ������ ��������
	CPUInfo[0] = IEAX;
	CPUInfo[1] = IEBX;
	CPUInfo[2] = IECX;
	CPUInfo[3] = IEDX;
}

extern "C" _declspec(dllexport) string GetCache()
{
	int CPUInfo[4]; // �������� EAX, EBX, ECX, EDX
	char CPUBrandString[48];//��� ����������
	string Cache = "";
	// ����������������� ���������� � ����������
	//(��� ���������� - ������ �� 48 �������� ������)
	GetCpuid(CPUInfo, 0x80000002);
	memcpy(CPUBrandString, CPUInfo, sizeof(CPUInfo));
	GetCpuid(CPUInfo, 0x80000003);
	memcpy(CPUBrandString + 16, CPUInfo, sizeof(CPUInfo));
	GetCpuid(CPUInfo, 0x80000004);
	memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
	if (strstr(CPUBrandString, "AMD"))//���� AMD
	{
		unsigned int ByteEAX, ByteEBX;
		//0x80000005-�������� ���������� � TLB 
		GetCpuid(CPUInfo, 0x80000005);
		//�������� 1 ����� ��������� EAX � EBX
		//� ��� ���������� ���������� � TLB ������
		ByteEAX = CPUInfo[0] & 0x000000FF;
		ByteEBX = CPUInfo[1] & 0x000000FF;
		//���� � ������ ���������, 1� ���� �� 00h(Reserved), ��
		//TLB ����� ��������� ������
		if (ByteEAX != 0x00) Cache += "2 - MB and 4 - MB pages. +";
		if (ByteEBX != 0x00) Cache += "4 KB pages.\n";

	}
	else//���� Intel
	{
		unsigned int Byte;
		//0x02-�������� ���������� � Cache 
		GetCpuid(CPUInfo, 0x02);
		int IsValid;
		unsigned int bias;//�����
		//���������� �������� EAX, EBX, ECX, EDX
		for (int i = 0; i < 4; i++)
		{
			//������� ��� ������� �������� �������� ��
			//������� ����������� ���������� ��� ��������������
			//���� �����������, �� ������ 0
			//��������������, ������ 1
			IsValid = (CPUInfo[i] & 0x80000000) >> 31;
			//�����, ������� �� �������, �������� ������ ����
			bias = i ? 0x000000FF : 0x0000FF00;

			if (IsValid == 0)//����������� ����������
			//���������� ����� � ������ ��������
			//���� EAX, ������ ���� ������ ���������� 01H, ���������� ���
			//����� ������������� �������� � 1��� �����
				for (int j = (!i ? 1 : 0); j < 4; j++)
				{
					//�������� ������ ����, ������ ��� �� ����� � ������� ������
					Byte = (CPUInfo[i] & bias) >> (8 * j);
					//��������� ��� ���������� �����
					bias = bias << 8;
					//���� ������ � ����� ������ ������ TLB
					if (Byte == 0x22 || Byte == 0x46 || Byte == 0xD0 || Byte == 0xD1 || Byte == 0xD2)
					{
						Cache = "4-way set associative";
					}
					if (Byte == 0x23 || Byte == 0x25 || Byte == 0x29 || Byte == 0x47 || Byte == 0xD6 || Byte == 0xD7 || Byte == 0xD8)
					{
						Cache = "8-way set associative";
					}
					if (Byte == 0x4A || Byte == 0x4C || Byte == 0xDC || Byte == 0xDD || Byte == 0xDE)
					{
						Cache = "12-way set associative";
					}
					if (Byte == 0x49 || Byte == 0x4B || Byte == 0x4D || Byte == 0xE2 || Byte == 0xE3 || Byte == 0xE4)
					{
						Cache = "16-way set associative";
					}
					if (Byte == 0xEA || Byte == 0xEB || Byte == 0xEC)
					{
						Cache = "24-way set associative";
					}
			}
		}
	}
	return Cache;

}