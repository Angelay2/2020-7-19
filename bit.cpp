/*
λͼ: ������ϣ��
	ʵ��: ��������
	1. ���ݵ�Ԫ��bitλ
	2. ��ʡ�ռ�, һ���ֽڿ��Դ��8�������Ķ�ֵ��Ϣ, ��������ݱ���
	3. ����Ч�ʸ�, ͨ����ϣӳ���ȡλ��, ͨ��λ����ִ�в���, ʱ��Ч��: O(1)
	4. λ��ӳ��: 
		a.��ȡ����λ�� --> n/32 
		b.��ȡ������bitλ��: n%32
		��ȡ������ĳһ��(x)bitλ��ֵ:  ���� >> (x-1) & 1
	5. λͼ��Ҫ�Ŀռ��С�����ݵķ�Χ�й�, �����ݱ���Ĵ�Сû�й�ϵ
	6. �ʺϳ���: ��������, ���ݲ��ظ�, ��Ϣ��
	
	��ϣ�и�: ��ͬIP�ᱻ���ֵ�ͬһ���ļ���, ��������������ת����С������������
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

	//��ѯ��Test
	bool Test(size_t n)
	{
		//����λ��
		int idx = n / 32;
		int bitIdx = n % 32;
		//��ȡ��Ӧbitλ������ֵ
		if ((_bit[idx] >> bitIdx) & 1)
			return true;
		else
			return false;
	}
	//�洢: Set
	void Set(size_t n)
	{
		int idx = n / 32;
		int bitIdx = n % 32;

		_bit[idx] |= (1 << bitIdx);

	}

	//ɾ��: Reset
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
	// bitλ���� = ��ϣ�������� * ������ / ln2
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
		//�����ϣֵ
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

		//����true: ��һ����ȷ
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