#ifndef IBF_H
#define IBF_H

#include"bitset.h"
template<class T>
struct Hash
{
	const T& operator()(const T& key)
	{
		return key;
	}
};
// BKDRHash
struct BKDRHash
{
	size_t operator()(const string& str)
	{
		size_t hash = 0;
		for (size_t i = 0; i < str.length(); ++i)
		{
			hash = hash * 131 + str[i];
		}

		return hash;
	}
};

// SDBHash
struct SDBHash
{
	size_t operator()(const string str)
	{
		size_t hash = 0;
		for (size_t i = 0; i < str.length(); ++i)
		{
			hash = 65599 * hash + str[i]; 
		}
		return hash;
	}
};

// RSHash
struct RSHash
{
	size_t operator()(const string str)
	{
		size_t hash = 0;
		size_t magic = 63689;
		for (size_t i = 0; i < str.length(); ++i)
		{
			hash = hash * magic + str[i];
			magic *= 378551;
		}
		return hash;
	}
};
// JSHash
struct JSHash
{
	size_t operator()(const string str)
	{
		size_t hash = 1315423911;
		for (size_t i = 0; i < str.length(); ++i)
		{
			hash = hash ^ ((hash << 5) + str[i] + (hash >> 2));
		}
		return hash;
	}
};
// BPHash
struct BPHash
{
	size_t operator()(const string str)
	{
		size_t hash = 0;
		for (size_t i = 0; i < str.length(); ++i)
		{
			hash = hash << 7 ^ str[i];
		}
		return hash;
	}
};

struct DJBHash
{
	size_t operator()(const string str)
	{
		size_t hash = 5381;
		for (size_t i = 0; i < str.length(); ++i)
		{
			hash = ((hash << 5) + hash) + str[i];
		}
		return hash;
	}
};

template<class T = string, class Hash1 = BKDRHash, class Hash2 = SDBHash, class Hash3 = RSHash, class Hash4 = JSHash, class Hash5 = BPHash, class Hash6 = DJBHash>
class IBloomFilter
{
public:
	ibitset _ibs;
public:
	IBloomFilter(size_t size)
		:_ibs(bfmlen/bfnlen * size)
		, _N(bfmlen/bfnlen * size)
	{}

	void iset(const T& x)
	{
		size_t index1 = Hash1()(x) % _N;
		size_t index2 = Hash2()(x) % _N;
		size_t index3 = Hash3()(x) % _N;
		size_t index4 = Hash4()(x) % _N;
		size_t index5 = Hash5()(x) % _N;

		_ibs.set(index1);
		_ibs.set(index2);
		_ibs.set(index3);
		_ibs.set(index4);
		_ibs.set(index5);
	}
	bool IsInIBloomFilter(const T& x)
	{
		size_t index1 = Hash1()(x) % _N;
		size_t index2 = Hash2()(x) % _N;
		size_t index3 = Hash3()(x) % _N;
		size_t index4 = Hash4()(x) % _N;
		size_t index5 = Hash5()(x) % _N;

		return _ibs.test(index1)
			&& _ibs.test(index2)
			&& _ibs.test(index3)
			&& _ibs.test(index4)
			&& _ibs.test(index5);
	}

private:
	
	size_t _N;
};

#endif