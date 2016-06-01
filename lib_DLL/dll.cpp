#include <windows.h>
#include <stdio.h>
#include <string>
using namespace std;
extern "C" _declspec(dllexport) char* GetInformation()
{
	
	char* type = "";
	// оманда GetJeyboardType с параметром 0 определ€ет тип клавитуры
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
	//если CaheNumb>=0, то с помощью CPUID определ€етс€  ЁЎ дл€ процессоров Intel
	//если CacheNumb=-2, то с помощью CPUID определ€еис€ L3  ЁЎ дл€ процессоров AMD
	//если CacheNumb=-1, то CPUID используетс€ дл€ определени€ названи€ процессора
	if(CacheNumb>=0)
	_asm
	{
		MOV EAX, InfoType //установка функции команды CPUID
		MOV ECX, CacheNumb
			CPUID	//вызов инструкции процессора
			//полученные в регистрах процессора 
			//результаты CPUID копируютс€
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
	//запись в переданный в функцию массив значений

	CPUInfo[0] = IEAX;
	CPUInfo[1] = IEBX;
	CPUInfo[2] = IECX;
	CPUInfo[3] = IEDX;
}

extern "C" _declspec(dllexport) char* GetCache(int* cache_ways_of_associativity)
{
	int CPUInfo[4]; // регистры EAX, EBX, ECX, EDX
	char*Brand = "";
	//unsigned int cache_ways_of_associativity=0;

	//определ€етс€ тип процессора
	GetCpuid(CPUInfo, 0x00000000, -1);

	if (CPUInfo[2]== 0x444D4163)//если AMD
	{
		//после помещени€ в EDX 0x80000006 и выполнени€ CPUID, в EDX возвращаюс€ хар-ки L3  ЁЎа
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
	else//если Intel
	{
		int cache_level;
		int CacheIndex=0;
		//помещение в EAX 0x04 и в E—X чисел 0, 1, 2 и т.д. и последующие выполнение CPUID
		//возвращает в регистры характеристики возвращаемого  ЁЎа. «аранее предсказать какой  ЁЎ вернЄтс€ нельз€, по этому
		//выполн€етс€ цикл поиска L3  ЁЎа до тех пор пока он либо не найдЄтс€, либо  ЁЎи кончатс€.
		//¬ случае окончани€  ЁЎа, возвращаетс€ 1 и считаетс€, что ассоциативность L3  ЁЎа определить нельз€
		do
		{
			GetCpuid(CPUInfo, 0x04, CacheIndex++);
			cache_level = (CPUInfo[0] >> 5)&0x7;
		} while ((cache_level!=3)&& ((CPUInfo[0] & 0xf0) != 0));

		*cache_ways_of_associativity = ((CPUInfo[1] >> 22) & 0x3FF) + 1;
		
	}
	return Brand;

}