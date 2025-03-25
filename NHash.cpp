// NHash.cpp Тестирование класса хэш-функции
//

#include "stdafx.h"
#include "DHash.h"

#include <ctime>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <intrin.h>


int main()
{

	bool bType = true;   // false - 512 битная функция, true - 256 битная функция
	DHash hash(bType); 
	unsigned long  h[16];  // 512 бит - буфер для вывода значения хэш-функции
	unsigned char* hbyte = (unsigned char*)h;
	

	//unsigned char M[] = { 0,3,1,3,2,3,3,3,4,3,5,3,6,3,7,3,8,3,9,3,0,3,1,3,2,3,3,3,4,3,5,3,6,3,7,3,8,3,9,3,0,3,1,3,2,3,3,3,4,3,5,3,6,3,7,3,8,3,9,3,0,3,1,3,2,3,3,3,4,3,5,3,6,3,7,3,8,
	//3,9,3,0,3,1,3,2,3,3,3,4,3,5,3,6,3,7,3,8,3,9,3,0,3,1,3,2,3,3,3,4,3,5,3,6,3,7,3,8,3,9,3,0,3,1,3,2,3 };

	// Пример 1 ГОСТ Р 34.11-2012
	//unsigned char M[] = { 0x30, 0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,
	//	0x39,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x30,0x31,0x32 };

	// Пример 2 ГОСТ Р 34.11-2012
	  unsigned char M[] = { 0xd1, 0xe5,0x20, 0xe2,0xe5,0xf2,0xf0,0xe8,0x2c,0x20,0xd1,0xf2,0xf0,0xe8,0xe1,0xee,0xe6,0xe8,0x20,0xe2,0xed,0xf3,0xf6,0xe8,0x2c,0x20,0xe2,0xe5,0xfe,0xf2,0xfa,0x20,0xf1,0x20,0xec,0xee,0xf0,0xff,0x20,0xf1,0xf2,
		0xf0,0xe5,0xeb,0xe0,0xec,0xe8,0x20,0xed,0xe0,0x20,0xf5,0xf0,0xe0,0xe1,0xf0,0xfb,0xff,0x20,0xef,0xeb,0xfa,0xea,0xfb,0x20,0xc8,0xe3,0xee,0xf0,0xe5,0xe2,0xfb };



	size_t len = sizeof(M);
	std::cout << "M length " << len << std::endl;

	//Формирование тестового файла (запись в файл)
	std::ofstream outfile;
	outfile.open("C:\\Work\\stribog72.txt", std::ios_base::out | std::ios_base::binary);
	for (int i = 0; i < len; i++)
		outfile << M[i];

	// Формирование большого тестового файла
	unsigned char bigM[2048];
	std::ofstream newoutfile;
	int lenBig = 2048;
	int offsetBig = 0;
	newoutfile.open("C:\\Work\\stribog2048.txt", std::ios_base::out | std::ios_base::binary);
	for (int i = 0; i < lenBig; i++)
	{
			newoutfile << M[i%72];
			bigM[offsetBig] = M[i%72];
			offsetBig++;
	}
	newoutfile.close();

	std::cout << "Length of test array " << offsetBig << std::endl;

	int ioff;

	/*
	/*  Тест для стандартного блока 
	hash.Reset();
	hash.Calculate(M,len);   
	hash.getHash(h);
	
	if (bType == true)
		ioff = 31;
	else ioff = 63;
	std::cout<<std::hex;
	for (int i = ioff; i >= 0; i--)
		std::cout<<std::hex << (unsigned short) hbyte[i];

	std::cout << std::endl;
	*/

	/*  Тест для большого стандартного блока */
	hash.Reset();
	hash.Calculate(bigM, lenBig);
	hash.getHash(h);
	if (bType == true)
		ioff = 31;
	else ioff = 63;
	std::cout << std::hex;
	for (int i = ioff; i >= 0; i--)
		std::cout << std::hex << (unsigned short)hbyte[i];
	

/* Тест с разбиением на 2 фрагмента   длина фрагмента должна быть кратна 512 байт
	hash.Reset();
	hash.CalculatePart(M, 64);
	hash.CalculatePart(M + 64, 8);
	hash.End();
	hash.getHash(h);
	if (bType == true)
		ioff = 31;
	else ioff = 63;
	std::cout << "Тест с разбиением на фрагменты " << std::endl;
	std::cout << std::hex;
	for (int i = ioff; i >= 0; i--)
		std::cout << std::hex << (unsigned short)hbyte[i];
*/


/*
	unsigned char bigM[512];
	size_t offsetBig = 0;

	std::ofstream outfile;
	outfile.open("C:\\Work\\stribog.txt", std::ios_base::out | std::ios_base::binary);
	for (int k=0;k< 1; k++)
		for (int i = 0; i < len; i++)
		{
			outfile << M[i];
			bigM[offsetBig++] = M[i];
		}
	for (int i = 0; i < 56; i++)
	{
		outfile << M[i];
		bigM[offsetBig++] = M[i];
	}
	outfile.close();

	std::cout << "Length of test array " << offsetBig << std::endl;


	

	
/*
	 Tест для большого блока  
	std::cout << "Block length " << len << std::endl;
	std::cout << "Length of test array " << offsetBig << std::endl;
	//std::cout << bigM;

	hash.Reset();
	hash.Calculate(bigM, offsetBig);
	unsigned char* hbyte = (unsigned char*)h;
	hash.getHash(h);
	int ioff;
	if (bType == true)
		ioff = 31;
	else ioff = 63;
	std::cout << std::hex;
	for (int i = ioff; i >= 0; i--)
		std::cout << std::hex << (unsigned short)hbyte[i];

	*/
	/*
	size_t count = 1000;
	size_t testlen = count * len;
	unsigned char* testM = new unsigned char[testlen];
	for (int i = 0; i != count; ++i)
	{
		int base = i * len;
		for (int j = 0; j != len; ++j)
			testM[base + j] = M[j];
	}
	*/

	

	size_t ln = 64;
	
	// расчёт значений хэш-функции по частям
	/*
	hash.Reset();
	hash.CalculatePart(M, ln);
	hash.CalculatePart(M + ln, (len - ln));
	hash.End();
	hash.getHash(h, 8);
	*/
	/*
	double time1 = clock();
	unsigned __int64 tsc1 = __rdtsc();
	//hash.Calculate(M, len);
	hash.Calculate (testM, testlen);
	double time2 = clock();
	unsigned __int64 tsc2 = __rdtsc();

	std::cout << "CPU time " << time2 - time1 << "CPU cycles " << tsc2 - tsc1 << std::endl;
	*/

	//hash.getHash(h);
	//delete[] testM;
	//int c = std::cin.get();
	
	 return 0;
}

