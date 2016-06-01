#include <windows.h>
#include <stdio.h>
#include <string>
using namespace std;
extern "C" _declspec(dllexport) char* GetInformation()
{
	
	char* type = "";
	//������� GetJeyboardType � ���������� 0 ���������� ��� ���������
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
		default:
		{type = "Unrecognized keyboard type";
		break;
		}
	}

	return type;
}
void GetCpuid(int CPUInfo[4], int InfoType, int CacheNumb)
{
	int IEAX, IEBX, IECX, IEDX;
	//���� CaheNumb>=0, �� � ������� CPUID ������������ ��� ��� ����������� Intel
	//���� CacheNumb=-2, �� � ������� CPUID ������������ L3 ��� ��� ����������� AMD
	//���� CacheNumb=-1, �� CPUID ������������ ��� ����������� �������� ����������
	if(CacheNumb>=0)
	_asm
	{
		MOV EAX, InfoType //��������� ������� ������� CPUID
		MOV ECX, CacheNumb
			CPUID	//����� ���������� ����������
			//���������� � ��������� ���������� 
			//���������� CPUID ����������
			MOV IEAX, EAX
			MOV IEBX, EBX
			MOV IECX, ECX
			MOV IEDX, EDX
	}
	else
		if(CacheNumb==-2)
	_asm
	{
		MOV EDX, InfoType 
			CPUID	
			MOV IEAX, EAX
			MOV IEBX, EBX
			MOV IECX, ECX
			MOV IEDX, EDX
	}	
		else
		{
			_asm
			{
				MOV EAX, InfoType 
				CPUID	
					MOV IEAX, EAX
					MOV IEBX, EBX
					MOV IECX, ECX
					MOV IEDX, EDX
			}
		}
	//������ � ���������� � ������� ������ ��������

	CPUInfo[0] = IEAX;
	CPUInfo[1] = IEBX;
	CPUInfo[2] = IECX;
	CPUInfo[3] = IEDX;
}

extern "C" _declspec(dllexport) char* GetCache(int* cache_ways_of_associativity)
{
	int CPUInfo[4]; // �������� EAX, EBX, ECX, EDX
	char*Brand = "";
	//unsigned int cache_ways_of_associativity=0;

	//������������ ��� ����������
	GetCpuid(CPUInfo, 0x00000000, -1);

	if (CPUInfo[2]== 0x444D4163)//���� AMD
	{
		//����� ��������� � EDX 0x80000006 � ���������� CPUID, � EDX ����������� ���-�� L3 ����
		GetCpuid(CPUInfo, 0x80000006, -2);
		if ((CPUInfo[3] & 0x0000000F) == 0x0)
			*cache_ways_of_associativity=0;
		if ((CPUInfo[3] & 0x0000000F) == 0x2)
			*cache_ways_of_associativity = 2;
		if ((CPUInfo[3] & 0x0000000F) == 0x4)
			*cache_ways_of_associativity = 4;
		if ((CPUInfo[3] & 0x0000000F) == 0x6)
			*cache_ways_of_associativity = 8;
		if ((CPUInfo[3] & 0x0000000F) == 0x8)
			*cache_ways_of_associativity = 16;
		if ((CPUInfo[3] & 0x0000000F) == 0xA)
			*cache_ways_of_associativity = 32;
		if ((CPUInfo[3] & 0x0000000F) == 0xB)
			*cache_ways_of_associativity = 48;
		if ((CPUInfo[3] & 0x0000000F) == 0xC)
			*cache_ways_of_associativity = 64;
		if ((CPUInfo[3] & 0x0000000F) == 0xD)
			*cache_ways_of_associativity = 96;
		if ((CPUInfo[3] & 0x0000000F) == 0xE)
			*cache_ways_of_associativity = 128;
		if ((CPUInfo[3] & 0x0000000F) == 0xF)
			*cache_ways_of_associativity = -1;

	}
	else//���� Intel
	{
		int cache_level;
		int CacheIndex=0;
		//��������� � EAX 0x04 � � E�X ����� 0, 1, 2 � �.�. � ����������� ���������� CPUID
		//���������� � �������� �������������� ������������� ����. ������� ����������� ����� ��� ������� ������, �� �����
		//����������� ���� ������ L3 ���� �� ��� ��� ���� �� ���� �� �������, ���� ���� ��������.
		//� ������ ��������� ����, ������������ 1 � ���������, ��� ��������������� L3 ���� ���������� ������
		do
		{
			GetCpuid(CPUInfo, 0x04, CacheIndex++);
			cache_level = (CPUInfo[0] >> 5)&0x7;
		} while ((cache_level!=3)&& ((CPUInfo[0] & 0xf0) != 0));

		*cache_ways_of_associativity = ((CPUInfo[1] >> 22) & 0x3FF) + 1;
		
	}
	return Brand;

}