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

extern "C" _declspec(dllexport) string GetCache()
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
		GetCpuid(CPUInfo, 0x02);
		int IsValid;
		unsigned int bias;//сдвиг
		//перебираем регистры EAX, EBX, ECX, EDX
		for (int i = 0; i < 4; i++)
		{
			//старший бит каждого регистра содержит ли
			//регистр достоверную информацию или зарезервирован
			//если достоверная, то вернет 0
			//зарезервирован, вернет 1
			IsValid = (CPUInfo[i] & 0x80000000) >> 31;
			//число, умножив на которое, получаем нужный байт
			bias = i ? 0x000000FF : 0x0000FF00;

			if (IsValid == 0)//достоверная информация
			//перебираем байты в каждом регистре
			//если EAX, первый байт всегда возвращает 01H, игнорируем его
			//иначе просматриваем регистры с 1ого байта
				for (int j = (!i ? 1 : 0); j < 4; j++)
				{
					//получаем нужный байт, множив его на число и сдвинув вконец
					Byte = (CPUInfo[i] & bias) >> (8 * j);
					//множитель для следующего байта
					bias = bias << 8;
					//ищем нужный и выдем размер буфера TLB
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