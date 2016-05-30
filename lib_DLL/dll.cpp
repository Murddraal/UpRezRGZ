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
		MOV EAX, InfoType //установка функции команды CPUID
		MOV ECX, 3
			CPUID	//вызов инструкции процессора
			//полученные в регистрах процессора 
			//результаты CPUID копируются
			MOV IEAX, EAX
			MOV IEBX, EBX
			MOV IECX, ECX
			MOV IEDX, EDX
	}
	//запись в переданный в функцию массив значений
	CPUInfo[0] = IEAX;
	CPUInfo[1] = IEBX;
	CPUInfo[2] = IECX;
	CPUInfo[3] = IEDX;
}

extern "C" _declspec(dllexport) int GetCache()
{
	int CPUInfo[4]; // регистры EAX, EBX, ECX, EDX
	char CPUBrandString[48];//имя процессора
	string Cache = "";
	// идентификационная информация о процессоре
	//(имя процессора - строка до 48 символов длиной)
	GetCpuid(CPUInfo, 0x80000002);
	memcpy(CPUBrandString, CPUInfo, sizeof(CPUInfo));
	GetCpuid(CPUInfo, 0x80000003);
	memcpy(CPUBrandString + 16, CPUInfo, sizeof(CPUInfo));
	GetCpuid(CPUInfo, 0x80000004);
	memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));

	unsigned int cache_ways_of_associativity;
	if (strstr(CPUBrandString, "AMD"))//если AMD
	{
		unsigned int ByteEAX, ByteEBX;
		//0x80000005-содержит информацию о TLB 
		GetCpuid(CPUInfo, 0x80000005);
		//получаем 1 байты регистров EAX и EBX
		//в них содержится информация о TLB команд
		ByteEAX = CPUInfo[0] & 0x000000FF;
		ByteEBX = CPUInfo[1] & 0x000000FF;
		//если в данных регистрах, 1й байт не 00h(Reserved), то
		//TLB имеет следующий размер
		if (ByteEAX != 0x00) Cache += "2 - MB and 4 - MB pages. +";
		if (ByteEBX != 0x00) Cache += "4 KB pages.\n";

	}
	else//если Intel
	{
		unsigned int Byte;
		//0x02-содержит информацию о Cache 
		GetCpuid(CPUInfo, 0x04);
		int IsValid;
		unsigned int bias;//сдвиг
		
		int cache_type = CPUInfo[0] & 0x1F;

		char * cache_type_string;
		switch (cache_type) {
		case 1: cache_type_string = "Кэш данных"; break;
		case 2: cache_type_string = "Кэш инструкций"; break;
		case 3: cache_type_string = "Универсальный кэш"; break;
		default: cache_type_string = "Неопределен"; break;
		}

		int cache_level = (CPUInfo[0] >>= 5) & 0x7;
		int cache_is_self_initializing = (CPUInfo[0] >>= 3) & 0x1;
		int cache_is_fully_associative = (CPUInfo[0] >>= 1) & 0x1;
		unsigned int cache_sets = CPUInfo[2] + 1;
		unsigned int cache_coherency_line_size = (CPUInfo[1] & 0xFFF) + 1;
		unsigned int cache_physical_line_partitions = ((CPUInfo[1] >>= 12) & 0x3FF) + 1;
		cache_ways_of_associativity = ((CPUInfo[1] >>= 10) & 0x3FF) + 1;
		
	}
	return cache_ways_of_associativity;

}