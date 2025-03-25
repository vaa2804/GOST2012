#include "stdafx.h"
#include <thread>
#include "DHash.h"



#define NBYTE 64
#define NLONG 16


void DHash:: Calculate(unsigned  char *buf, size_t len)
//  расчёт хэш-функций  для сообщения произвольной длины
{
	unsigned int offset = 0;
	unsigned int msize;

	unsigned char tmp[64];
	unsigned long *plong;
	plong = reinterpret_cast<unsigned long *> (tmp);

	Reset(); // сброс данных
	while (len >= nByte)  // обработка блоками по 512 бит
	{
		Update(reinterpret_cast<unsigned long *> (buf + offset), 512);
		offset += nByte;
		len -= nByte;
	}
	
	//if (len > 0)            // дополнение и обработка последнего блока
	{
		memcpy(tmp, buf + offset, len);
		tmp[len] = 1;
		msize = len * 8;
		len++;
		memset(tmp + len, 0, nByte - len);
		Update(plong, msize);
	}

	// завершение обработки сообщения
	// скопировать N в буфер и обнулить
	for (int i = 0; i < 16; i++)
	{
		plong[i] = N[i];
		N[i] = 0;
	}
	Gn(plong);
	Gn(Sigma);

}


void DHash::CalculatePart(unsigned  char *buf, size_t len)
//  расчёт хэш-функций  для сообщения произвольной длины
{
	unsigned int offset = 0;
	unsigned int msize;

	unsigned char tmp[64];
	unsigned long *plong;
	plong = reinterpret_cast<unsigned long *> (tmp);
	while (len >= nByte)  // обработка блоками по 512 бит
	{
		Update(reinterpret_cast<unsigned long *> (buf + offset), 512);
		offset += nByte;
		len -= nByte;
	}
	/*
	if (len > 0)            // дополнение и обработка последнего блока
	{
		memcpy(tmp, buf + offset, len);
		tmp[len] = 1;
		msize = len * 8;
		len++;
		memset(tmp + len, 0, nByte - len);
		Update(plong, msize);
	}
	*/
}

//  расчёт хэш-функций  для последней части  сообщения 
	// длина части сообщения, за исключением последней, может быть не кратна 512 бит (64 байт)
	//buf - часть сообщения
	//len - его длина
void DHash::CalculateLastPart(unsigned  char* buf, size_t len)
{
	unsigned int offset = 0;
	unsigned int msize;

	unsigned char tmp[64];
	unsigned long* plong;
	plong = reinterpret_cast<unsigned long*> (tmp);
	while (len >= nByte)  // обработка блоками по 512 бит
	{
		Update(reinterpret_cast<unsigned long*> (buf + offset), 512);
		offset += nByte;
		len -= nByte;
	}
	
	//if (len > 0)            // дополнение и обработка последнего блока
	{
		memcpy(tmp, buf + offset, len);
		tmp[len] = 1;
		msize = len * 8;
		len++;
		memset(tmp + len, 0, nByte - len);
		Update(plong, msize);
	}
	
}



void DHash::getHash(unsigned long *hash)
// вернёт значение хэш-функции
// возвращаемые данные определяются в зависимости от режима работы(256 или 512 битный хэш)
// hash - буфер для записи значений
// ln - длина буфера 
{
	int i=0;
	int ln = 16;
	if (this->bType)
	{
		i = 8;
		ln = 8;
	}
	for (int j = 0; j < ln; j++)
		hash[j] = h[i + j];
}


void DHash::Update(unsigned long *buf, unsigned int msize)
// расчёт хэш-функции для блока данных длиной не более 512 бит
{
	// контрольная сумма - сложение в кольце 512
	for (int i = 0; i < 16; i++)
	{
		Sigma[i] += buf[i];
		if ((Sigma[i] < buf[i]) && i != 15)
		{
			Sigma[i + 1]++; // перенос разряда
			
		}
	}
	Gn(buf);

	unsigned int CF = msize;
	for (int i = 0; i < 16; i++)
	{
		N[i] += CF;
		if (N[i] < CF)   CF = 1;
		else break;
	}

	return;
}



void DHash::End()
{
	// завершение обработки сообщения
	// скопировать N в буфер и обнулить
		unsigned long buf[16];
		for (int i = 0; i < 16; i++)
		{
				buf[i] = N[i];
				N[i] = 0;
		}
		Gn(buf);
		Gn(Sigma);
}



void  DHash::KCalculate()
{
	// последовательный расчёт коэффициентов К
	for (int i = 0; i < 12; i++)
	{
			LPSX(K[i+1], (const unsigned long *) C[i], K[i]);
			Nk++;
	}
}


// вариант функции с распараллеливанием вычисления К и LPSXX
/*
void DHash::Gn ( unsigned long *m)   
//функция  сжатия
//  Значение m изменяется в процессе расчёта!
{
	// расчёт хэш-функции для блока данных  m длиной 512 бит
	
		int Nt = 0;  // номер текущего элемента (обрабатываемого)
		int Nmax = 13; // максимальный номер элемента для обработки
					
				
		LPSX (K[0], N, h); // вычислить начальное значение К (Nk=0)
		Nk = 0;

		// запуск потока для расчёта коэффициентов К
		std::thread Kthread  (&DHash::KCalculate, this);

		// сложение по модулю 2 блока данных m с хэш-функцией h
		X(h, m);
		

		while (true)
		{
			if ( Nt<=Nk) 
			{ 
					// обработать блок данных
				if (Nt == 12)
				{
					X(m, K[12]); // последняя итерация останов цикла
					break;
				}
				LPSXX(K[Nt], m);
				Nt++;
			}
			else
			{
				// ждать сообщения от потока о готовности нового коэффициента
				while (Nt > Nk)  ;
			}
		}

		X(h, m);
		Kthread.join();
		
	
		return;
}
*/


// вариант функции без распараллеливания расчёта коэффициентов K и вычисления LPSX
void DHash::Gn(unsigned long* m)
//функция  сжатия
//  Значение m изменяется в процессе расчёта!
{
	// расчёт хэш-функции для блока данных  m длиной 512 бит
	LPSX(K[0], N, h); // вычислить начальное значение К (Nk=0)
	KCalculate();
	// сложение по модулю 2 блока данных m с хэш-функцией h
	X(h, m);
	for (int i=0; i!=12; ++i)
		LPSXX(K[i], m);
	X(m, K[12]);
	X(h, m);
	
}









void DHash:: XOR(unsigned long * dest, unsigned long const *A, unsigned long const  *B)
{
	for (int i = 0; i < NLONG; i++)
		dest[i] = A[i] ^ B[i];

}

void DHash::X(unsigned long *A, unsigned long const *B)
// исключающее или двух 32-битных беззнаковых массивов А и В длиной 512 бит с записью результата по адресу первого массива
{
	for (int i = 0; i < NLONG; i++)
		A[i] ^= B[i];
}


void S(unsigned char *dest, const unsigned char *source,const unsigned char *ptable)
// подстановка значений - копирование из исходного массива в результативный
// x - вектор длины 512 бит
{
		for (int i = 0; i < NBYTE; i++)
		dest[i] = ptable [ source[i] ];

}

void P(unsigned char *dest, const unsigned char *source, const unsigned int *ptable)
// перестановка  значений - в результативный массив подставляются значения из исходного массива, переставленные в соответсвии с таблицей перестановки
// ptable
// индекс подставляемого значения равен значению элемента в массиве ptable
//source - исходный массив  dest - результат
// длина копируемого массива 64 байта
{
	for (int i = 0; i < NBYTE; i++)
		dest[i] = source[ ptable [i] ];


}

void L(unsigned char *dest, const unsigned char *source, const unsigned  long long  *atable, const unsigned long long *mask)
// линейное преобразование множества двоичных векторов
//source - исходный массив  dest - результат
// длина копируемого массива 64 байта
{
	const unsigned long long *pSource=(const unsigned long long *)source;
	unsigned long long *pDest =(unsigned long long *)dest;
	for (int i = 0; i < 8; i++)
			l (pDest + i, *(pSource + i),atable, mask);
}

void l(unsigned long long  *dest, unsigned long long source, const unsigned long long  *atable, const unsigned long long *mask)
// линейное преобразование  двоичного вектора длиной 64 бита (V64)
//source - исходный 64-битный вектор
//dest - результативный 64-битный вектор
{
	*dest = 0;
	for (int i = 0; i < 64; i++)
		if (mask[i] & source)  *dest = *dest^atable[63 - i];

}

void DHash :: LPS(unsigned char *dest, const unsigned char *source)
// составное преобразование LPS
// данные таблиц берутся из статических массивов
// source - исходный 64-байтный вектор
//dest - результативный 64-байтный вектор
{
	unsigned char tmp1[NBYTE], tmp2[NBYTE];
	int i, j;
	for ( i = 0; i < NBYTE; i++)
		tmp1[i] = PI_TAB[source[i]];

	for ( i = 0; i < NBYTE; i++)
		tmp2[i] = tmp1[P_TAB[i]];

	const unsigned long long *pSource = (const unsigned long long *)tmp2;
	unsigned long long *pDest = (unsigned long long *)dest;
	// обнулить результат
	memset (dest, 0, NBYTE);

	unsigned long long  lSource;
	unsigned long long   *pD;

	for ( i = 0; i < 8; i++)
	{
		lSource = *(pSource + i);
		pD = pDest + i;
		for (  j = 0; j < 64; j++)
			if (Mask[j] & lSource)  *pD = *pD^A[63 - j];
	}

	
}

void DHash::LPSX(unsigned long *dest,const unsigned long *K, const unsigned long *source)
// составное преобразование LPSX
// данные таблиц берутся из статических массивов
// source - исходный 64-байтный вектор
// K - 64- байтный множитель
//dest - результативный 64-байтный вектор
{
	unsigned char tmp1[NBYTE], tmp2[NBYTE];
	int i, j;
	unsigned char * pK = (unsigned char *) K;
	unsigned char * psource = (unsigned char *)source;
	unsigned long *ptmp = (unsigned long *) tmp2;
	
	for (int i = 0; i < NLONG; i++)
		ptmp[i] = source[i] ^ K[i];

	for (i = 0; i < NBYTE; i++)
		tmp1[i] = PI_TAB[ tmp2[i] ];

	for (i = 0; i < NBYTE; i++)
		tmp2[i] = tmp1[P_TAB[i]];

	const unsigned long long *pSource = (const unsigned long long *)tmp2;
	unsigned long long *pDest = (unsigned long long *)dest;
	// обнулить результат
	memset(dest, 0, NBYTE);

	unsigned long long  lSource;
	unsigned long long   *pD;

	for (i = 0; i < 8; i++)
	{
		lSource = *(pSource + i);
		pD = pDest + i;
		for (j = 0; j < 64; j++)
			if (Mask[j] & lSource)  *pD = *pD^A[63 - j];
	}


}


void DHash::LPSXX(const unsigned long *K, unsigned long *source)
// составное преобразование LPSX
// преобразование с записью результата "на месте" - в исходный массив source
// данные таблиц берутся из статических массивов
// source - исходный 64-байтный вектор
// K - 64- байтный множитель
{
	unsigned char tmp1[NBYTE], tmp2[NBYTE];
	int i, j;
	unsigned char * pK = (unsigned char *)K;
	unsigned char * psource = (unsigned char *)source;
	unsigned long *ptmp = (unsigned long *)tmp2;

	for (int i = 0; i < NLONG; i++)
		ptmp[i] = source[i] ^ K[i];

	for (i = 0; i < NBYTE; i++)
		tmp1[i] = PI_TAB[tmp2[i]];

	for (i = 0; i < NBYTE; i++)
		tmp2[i] = tmp1[P_TAB[i]];
	
	const unsigned long long *pSource = (const unsigned long long *)tmp2;
		
	unsigned long long   dest;
	unsigned long long *pResult = (unsigned long long *) source;

	for (i = 0; i < 8; i++)
	{
		dest = 0;
		for (j = 0; j < 64; j++)
			if ( Mask[j] & (*(pSource + i)) )  dest = dest^A[63 - j];
		*(pResult + i) = dest;
	}

}






