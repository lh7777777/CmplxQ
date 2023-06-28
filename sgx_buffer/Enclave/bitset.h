#include <vector>
#include <string>
#include <new>
#include <algorithm> 
#include <openssl/sha.h>
#include <openssl/hmac.h>

using namespace std;

char k[] = "123456";
unsigned int len = 0;

class ibitset
{
public:
	vector<vector<int> > _ibitset;
	int ibitCount;
public:
	ibitset(size_t ibitCount, char* ibfset)
		:ibitCount(ibitCount)
	{		
		_ibitset.resize(2);
		for(int i=0;i<ibitCount;i++)
		{
			int tmp = (int)(ibfset[i] - '0');
			_ibitset[0].push_back(tmp);
		}
		for(int j=ibitCount;j<2*ibitCount;j++)
		{
			int tmp = (int)(ibfset[j] - '0');
			_ibitset[1].push_back(tmp);
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
