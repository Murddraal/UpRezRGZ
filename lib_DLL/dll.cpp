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
	//если CacheNumb=-1, то CPUID используетс€ дл€ определени€ марки процессора
	switch (CacheNumb)
	{
		case -1:
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
			break;
		}
		default:
		{
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
			break;
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

	//определ€етс€  процессор
	GetCpuid(CPUInfo, 0x00000000, -1);

	if (CPUInfo[2] == 0x444D4163)//если AMD
	{
		Brand = "AMD";
		//после помещени€ в EAX 0x80000006 и выполнени€ CPUID, в EDX возвращаюс€ хар-ки L3  ЁЎа
		GetCpuid(CPUInfo, 0x80000006, -2);
		switch (((CPUInfo[3] >> 12) & 0x0000001F))
		{
		case 0x0:
			*cache_ways_of_associativity = 0;
			break;
		case 0x02:
			*cache_ways_of_associativity = 2;
			break;
		case 0x04:
			*cache_ways_of_associativity = 4;
			break;
		case 0x06:
			*cache_ways_of_associativity = 8;
			break;
		case 0x08:
			*cache_ways_of_associativity = 16;
			break;
		case 0x0A:
			*cache_ways_of_associativity = 32;
			break;
		case 0x0B:
			*cache_ways_of_associativity = 48;
			break;
		case 0x0C:
			*cache_ways_of_associativity = 64;
			break;
		case 0x0D:
			*cache_ways_of_associativity = 96;
			break;
		case 0x0E:
			*cache_ways_of_associativity = 128;
			break;
		case 0x0F:
			*cache_ways_of_associativity = -1;
			break;
		}		

	}
	else//если Intel
	{
		Brand = "Intel";
		int cache_level;
		int CacheIndex = 0;
		//помещение в EAX 0x04 и в E—X чисел 0, 1, 2 и т.д. и последующие выполнение CPUID
		//возвращает в регистры характеристики возвращаемого  ЁЎа. «аранее предсказать какой  ЁЎ вернЄтс€ нельз€, по этому
		//выполн€етс€ цикл поиска L3  ЁЎа до тех пор пока он либо не найдЄтс€, либо  ЁЎи кончатс€.
		//¬ случае окончани€  ЁЎа, возвращаетс€ 1 и считаетс€, что ассоциативность L3  ЁЎа определить нельз€
		do
		{
			GetCpuid(CPUInfo, 0x04, CacheIndex++);
			cache_level = (CPUInfo[0] >> 5) & 0x7;
		} while ((cache_level != 3) && ((CPUInfo[0] & 0xf0) != 0));

		*cache_ways_of_associativity = ((CPUInfo[1] >> 22) & 0x3FF) + 1;

	}
	return Brand;

}