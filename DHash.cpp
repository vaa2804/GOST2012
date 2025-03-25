#include "stdafx.h"
#include <thread>
#include "DHash.h"



#define NBYTE 64
#define NLONG 16


void DHash:: Calculate(unsigned  char *buf, size_t len)
//  ������ ���-�������  ��� ��������� ������������ �����
{
	unsigned int offset = 0;
	unsigned int msize;

	unsigned char tmp[64];
	unsigned long *plong;
	plong = reinterpret_cast<unsigned long *> (tmp);

	Reset(); // ����� ������
	while (len >= nByte)  // ��������� ������� �� 512 ���
	{
		Update(reinterpret_cast<unsigned long *> (buf + offset), 512);
		offset += nByte;
		len -= nByte;
	}
	
	//if (len > 0)            // ���������� � ��������� ���������� �����
	{
		memcpy(tmp, buf + offset, len);
		tmp[len] = 1;
		msize = len * 8;
		len++;
		memset(tmp + len, 0, nByte - len);
		Update(plong, msize);
	}

	// ���������� ��������� ���������
	// ����������� N � ����� � ��������
	for (int i = 0; i < 16; i++)
	{
		plong[i] = N[i];
		N[i] = 0;
	}
	Gn(plong);
	Gn(Sigma);

}


void DHash::CalculatePart(unsigned  char *buf, size_t len)
//  ������ ���-�������  ��� ��������� ������������ �����
{
	unsigned int offset = 0;
	unsigned int msize;

	unsigned char tmp[64];
	unsigned long *plong;
	plong = reinterpret_cast<unsigned long *> (tmp);
	while (len >= nByte)  // ��������� ������� �� 512 ���
	{
		Update(reinterpret_cast<unsigned long *> (buf + offset), 512);
		offset += nByte;
		len -= nByte;
	}
	/*
	if (len > 0)            // ���������� � ��������� ���������� �����
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

//  ������ ���-�������  ��� ��������� �����  ��������� 
	// ����� ����� ���������, �� ����������� ���������, ����� ���� �� ������ 512 ��� (64 ����)
	//buf - ����� ���������
	//len - ��� �����
void DHash::CalculateLastPart(unsigned  char* buf, size_t len)
{
	unsigned int offset = 0;
	unsigned int msize;

	unsigned char tmp[64];
	unsigned long* plong;
	plong = reinterpret_cast<unsigned long*> (tmp);
	while (len >= nByte)  // ��������� ������� �� 512 ���
	{
		Update(reinterpret_cast<unsigned long*> (buf + offset), 512);
		offset += nByte;
		len -= nByte;
	}
	
	//if (len > 0)            // ���������� � ��������� ���������� �����
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
// ����� �������� ���-�������
// ������������ ������ ������������ � ����������� �� ������ ������(256 ��� 512 ������ ���)
// hash - ����� ��� ������ ��������
// ln - ����� ������ 
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
// ������ ���-������� ��� ����� ������ ������ �� ����� 512 ���
{
	// ����������� ����� - �������� � ������ 512
	for (int i = 0; i < 16; i++)
	{
		Sigma[i] += buf[i];
		if ((Sigma[i] < buf[i]) && i != 15)
		{
			Sigma[i + 1]++; // ������� �������
			
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
	// ���������� ��������� ���������
	// ����������� N � ����� � ��������
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
	// ���������������� ������ ������������� �
	for (int i = 0; i < 12; i++)
	{
			LPSX(K[i+1], (const unsigned long *) C[i], K[i]);
			Nk++;
	}
}


// ������� ������� � ������������������ ���������� � � LPSXX
/*
void DHash::Gn ( unsigned long *m)   
//�������  ������
//  �������� m ���������� � �������� �������!
{
	// ������ ���-������� ��� ����� ������  m ������ 512 ���
	
		int Nt = 0;  // ����� �������� �������� (���������������)
		int Nmax = 13; // ������������ ����� �������� ��� ���������
					
				
		LPSX (K[0], N, h); // ��������� ��������� �������� � (Nk=0)
		Nk = 0;

		// ������ ������ ��� ������� ������������� �
		std::thread Kthread  (&DHash::KCalculate, this);

		// �������� �� ������ 2 ����� ������ m � ���-�������� h
		X(h, m);
		

		while (true)
		{
			if ( Nt<=Nk) 
			{ 
					// ���������� ���� ������
				if (Nt == 12)
				{
					X(m, K[12]); // ��������� �������� ������� �����
					break;
				}
				LPSXX(K[Nt], m);
				Nt++;
			}
			else
			{
				// ����� ��������� �� ������ � ���������� ������ ������������
				while (Nt > Nk)  ;
			}
		}

		X(h, m);
		Kthread.join();
		
	
		return;
}
*/


// ������� ������� ��� ����������������� ������� ������������� K � ���������� LPSX
void DHash::Gn(unsigned long* m)
//�������  ������
//  �������� m ���������� � �������� �������!
{
	// ������ ���-������� ��� ����� ������  m ������ 512 ���
	LPSX(K[0], N, h); // ��������� ��������� �������� � (Nk=0)
	KCalculate();
	// �������� �� ������ 2 ����� ������ m � ���-�������� h
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
// ����������� ��� ���� 32-������ ����������� �������� � � � ������ 512 ��� � ������� ���������� �� ������ ������� �������
{
	for (int i = 0; i < NLONG; i++)
		A[i] ^= B[i];
}


void S(unsigned char *dest, const unsigned char *source,const unsigned char *ptable)
// ����������� �������� - ����������� �� ��������� ������� � ��������������
// x - ������ ����� 512 ���
{
		for (int i = 0; i < NBYTE; i++)
		dest[i] = ptable [ source[i] ];

}

void P(unsigned char *dest, const unsigned char *source, const unsigned int *ptable)
// ������������  �������� - � �������������� ������ ������������� �������� �� ��������� �������, �������������� � ����������� � �������� ������������
// ptable
// ������ �������������� �������� ����� �������� �������� � ������� ptable
//source - �������� ������  dest - ���������
// ����� ����������� ������� 64 �����
{
	for (int i = 0; i < NBYTE; i++)
		dest[i] = source[ ptable [i] ];


}

void L(unsigned char *dest, const unsigned char *source, const unsigned  long long  *atable, const unsigned long long *mask)
// �������� �������������� ��������� �������� ��������
//source - �������� ������  dest - ���������
// ����� ����������� ������� 64 �����
{
	const unsigned long long *pSource=(const unsigned long long *)source;
	unsigned long long *pDest =(unsigned long long *)dest;
	for (int i = 0; i < 8; i++)
			l (pDest + i, *(pSource + i),atable, mask);
}

void l(unsigned long long  *dest, unsigned long long source, const unsigned long long  *atable, const unsigned long long *mask)
// �������� ��������������  ��������� ������� ������ 64 ���� (V64)
//source - �������� 64-������ ������
//dest - �������������� 64-������ ������
{
	*dest = 0;
	for (int i = 0; i < 64; i++)
		if (mask[i] & source)  *dest = *dest^atable[63 - i];

}

void DHash :: LPS(unsigned char *dest, const unsigned char *source)
// ��������� �������������� LPS
// ������ ������ ������� �� ����������� ��������
// source - �������� 64-������� ������
//dest - �������������� 64-������� ������
{
	unsigned char tmp1[NBYTE], tmp2[NBYTE];
	int i, j;
	for ( i = 0; i < NBYTE; i++)
		tmp1[i] = PI_TAB[source[i]];

	for ( i = 0; i < NBYTE; i++)
		tmp2[i] = tmp1[P_TAB[i]];

	const unsigned long long *pSource = (const unsigned long long *)tmp2;
	unsigned long long *pDest = (unsigned long long *)dest;
	// �������� ���������
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
// ��������� �������������� LPSX
// ������ ������ ������� �� ����������� ��������
// source - �������� 64-������� ������
// K - 64- ������� ���������
//dest - �������������� 64-������� ������
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
	// �������� ���������
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
// ��������� �������������� LPSX
// �������������� � ������� ���������� "�� �����" - � �������� ������ source
// ������ ������ ������� �� ����������� ��������
// source - �������� 64-������� ������
// K - 64- ������� ���������
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






