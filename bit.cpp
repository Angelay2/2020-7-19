/*
位图: 操作哈希表
	实现: 整数数组
	1. 数据单元是bit位
	2. 节省空间, 一个字节可以存放8个整数的二值信息, 不存放数据本身
	3. 操作效率高, 通过哈希映射获取位置, 通过位运算执行操作, 时间效率: O(1)
	4. 位置映射: 
		a.获取整数位置 --> n/32 
		b.获取整数的bit位置: n%32
		获取整数中某一个(x)bit位的值:  整数 >> (x-1) & 1
	5. 位图需要的空间大小和数据的范围有关, 和数据本身的大小没有关系
	6. 适合场景: 大数据量, 数据不重复, 信息简单
	
	哈希切割: 相同IP会被划分到同一个文件中, 大数据量的问题转换成小数据量的问题
*/

#include <string>
#include <vector>
#include <iostream>
using namespace std;

class BitMap
{
public:
	BitMap(size_t range)
	{
		_bit.resize(range / 32 + 1);
	}

	//查询：Test
	bool Test(size_t n)
	{
		//整数位置
		int idx = n / 32;
		int bitIdx = n % 32;
		//获取对应bit位二进制值
		if ((_bit[idx] >> bitIdx) & 1)
			return true;
		else
			return false;
	}
	//存储: Set
	void Set(size_t n)
	{
		int idx = n / 32;
		int bitIdx = n % 32;

		_bit[idx] |= (1 << bitIdx);

	}

	//删除: Reset
	void Reset(size_t n)
	{
		int idx = n / 32;
		int bitIdx = n % 32;

		_bit[idx] &= ~(1 << bitIdx);
	}
private:
	vector<int> _bit;
};

void testBit()
{
	BitMap bit(100);

	bit.Set(4);
	bit.Set(31);
	bit.Set(48);

	bool ret = bit.Test(4);
	ret = bit.Test(31);
	ret = bit.Test(48);
	ret = bit.Test(25);

	bit.Reset(31);
	bit.Reset(4);
	bit.Reset(48);
}

template <class T, class HF1, class HF2, class HF3>
class BloomFilter
{
public:
	// bit位数量 = 哈希函数个数 * 数据量 / ln2
	BloomFilter(size_t num)
		:_bit(5 * num)
		, _bitCount(5 * num)
	{}

	// Set
	void Set(const T& value)
	{
		HF1 hf1;
		HF2 hf2;
		HF3 hf3;
		//计算哈希值
		size_t hashCode1 = hf1(value);
		size_t hashCode2 = hf2(value);
		size_t hashCode3 = hf3(value);

		_bit.Set(hashCode1 % _bitCount);
		_bit.Set(hashCode2 % _bitCount);
		_bit.Set(hashCode3 % _bitCount);
	}

	// Test
	bool Test(const T& value)
	{
		HF1 hf1;
		size_t hashCode1 = hf1(value);
		if (!_bit.Test(hashCode1 % _bitCount))
			return false;
		HF2 hf2;
		size_t hashCode2 = hf2(value);
		if (!_bit.Test(hashCode2 % _bitCount))
			return false;
		HF3 hf3;
		size_t hashCode3 = hf3(value);
		if (!_bit.Test(hashCode3 % _bitCount))
			return false;

		//返回true: 不一定正确
		return true;
	}

private:
	BitMap _bit;
	size_t _bitCount;

};

struct strToInt1
{
	size_t operator()(const string& str)
	{
		size_t hash = 0;
		for (auto& ch : str)
		{
			hash = hash * 131 + ch;
		}
		return hash;
	}
};

struct strToInt2
{
	size_t operator()(const string& str)
	{
		size_t hash = 0;
		for (auto& ch : str)
		{
			hash = hash * 65599 + ch;
		}
		return hash;
	}
};

struct strToInt3
{
	size_t operator()(const string& str)
	{
		size_t hash = 0;
		size_t magic = 63689;
		for (auto& ch : str)
		{
			hash = hash * magic + ch;
			magic *= 378551;
		}
		return hash;
	}
};

void testB()
{
	BloomFilter<string, strToInt1, strToInt2, strToInt3> bf(300);
	string str1 = "http://www.cplusplus.com/reference/unordered_set/unordered_set/?kw=unordered_set";
	string str2 = "http://www.cplusplus.com/reference/unordered_set/unordered_set/?kw=unordered_se";
	string str3 = "http://www.cplusplus.com/reference/unordered_set/unordered_set/?kw=unordered_ses";

	string str4 = "http://www.cplusplus.com/reference/unordered_set/unordered_set/?kw=unordered_se1";
	string str5 = "http://www.cplusplus.com/reference/unordered_set/unordered_set/?kw=u   nordered_seq";

	bf.Set(str1);
	bf.Set(str2);
	bf.Set(str3);

	bool ret = bf.Test(str1);
	ret = bf.Test(str2);
	ret = bf.Test(str3);
	ret = bf.Test(str4);
	ret = bf.Test(str5);

}

int main()
{
	//testBit();
	testB();
	return 0;
}