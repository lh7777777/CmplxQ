#ifndef BITSET_H
#define BITSET_H

#include <vector>
#include <string>
#include <string.h>
#include <stdio.h>
#include <new>
#include <algorithm> 
#include <openssl/sha.h>
#include <openssl/rand.h>
#include <openssl/hmac.h>
#include <iostream>
#include "encode.h"
using namespace std;

#define bfnlen 5000
#define bfmlen 5000*10
char k[] = "123456";
unsigned int len = 0;

class ibitset
{
public:
	vector<vector<int> > _ibitset;
	int ibitCount;
public:
	ibitset(size_t ibitCount)
		:ibitCount(ibitCount)
	{
		_ibitset.resize(2);
		_ibitset[0].resize(ibitCount + 1);
		_ibitset[1].resize(ibitCount + 1);
		for(int i=0;i<ibitCount;i++)
		{
			unsigned char ibuf[33] = {};
			HMAC(EVP_sha256(),ik,strlen(ik),(unsigned char*)(to_string(i).c_str()),to_string(i).length(),ibuf,&len);
    		unsigned char obuf[33] = {};
    		SHA256(ibuf, strlen((const char*)ibuf), obuf);

			int id = ((obuf[31] - '0') % 2 + 2) % 2;
			_ibitset[id][i] = 0;
			_ibitset[1-id][i] = 1;

		}

	}

	void set(int x)
	{
		if (x > ibitCount) return;

		unsigned char ibuf[33] = {};
		HMAC(EVP_sha256(),ik,strlen(ik),(unsigned char*)(to_string(x).c_str()),to_string(x).length(),ibuf,&len);
    	unsigned char obuf[33] = {};
    	SHA256(ibuf, strlen((const char*)ibuf), obuf);

		int id = ((obuf[31] - '0') % 2 + 2) % 2;
		_ibitset[id][x] = 1;
		_ibitset[1-id][x] = 0;

	}
	
	bool test(int x)
	{
		if (x > ibitCount) return false;

		unsigned char ibuf[33] = {};
		HMAC(EVP_sha256(),ik,strlen(ik),(unsigned char*)(to_string(x).c_str()),to_string(x).length(),ibuf,&len);
    	unsigned char obuf[33] = {};
    	SHA256(ibuf, strlen((const char*)ibuf), obuf);

		int id = ((obuf[31] - '0') % 2 + 2) % 2;
		return _ibitset[id][x];
	}
};

#endif