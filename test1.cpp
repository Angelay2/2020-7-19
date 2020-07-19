/*
哈希表的大小给一个素数大小, 到底要增加多少容量, 通过一素数表来晓得

*/
#include <string>
#include <iostream>
#include <vector>
using namespace std;
template <class K>
struct keyOfValue
{
	const K& operator()(const K& key)
	{
		return key;
	}
};


//开散列： 指针数组 + 单链表

template <class V>
struct HashNode
{
	V _value;
	HashNode<V>* _next;

	HashNode(const V& val = V())
		:_value(val)
		, _next(nullptr)
	{}
};

//前置声明
template <class K, class V, class KeyOfValue, class HF>
class HashTable;

template <class K, class V, class KeyOfValue, class HF>
struct HashIterator
{
	typedef HashNode<V> Node;
	typedef HashIterator<K, V, KeyOfValue, HF> Self;

	typedef HashTable<K, V, KeyOfValue, HF> HT;

	Node* _node;
	HT* _ht;

	HashIterator(Node* node, HT* ht)
		:_node(node)
		, _ht(ht)
	{}

	V& operator*()
	{
		return _node->_value;
	}

	V* operator->()
	{
		return &_node->_value;
	}

	bool operator!=(const Self& it)
	{
		return _node != it._node;
	}

	Self& operator++()
	{
		if (_node->_next)
			_node = _node->_next;
		else
		{
			//找到下一个非空链表的头结点
			// 1. 定位当前节点在哈希表中的位置
			//kov: 获取value的key
			KeyOfValue kov;
			//hf: 获取key转换之后的整数
			HF hf;
			size_t idx = hf(kov(_node->_value)) % _ht->_table.size();

			// 2. 从表中的下一个位置开始查找第一个非空链表的头结点
			++idx;
			Node* cur = _ht->_table[idx];

			for (; idx < _ht->_table.size(); ++idx)
			{
				if (_ht->_table[idx])
				{
					_node = _ht->_table[idx];
					break;
				}
			}
			// 3. 判断是否找到一个非空的头结点
			if (idx == _ht->_table.size())
				_node = nullptr;
		}
		return *this;
	}
};

template <class K, class V, class KeyOfValue, class HF>
class HashTable
{
public:

	//迭代器声明为友元类
	template <class K, class V, class KeyOfValue, class HF>
	friend struct HashIterator;

	typedef HashNode<V> Node;
	typedef HashIterator<K, V, KeyOfValue, HF> iterator;

	iterator begin()
	{
		//第一个非空链表的头结点
		for (size_t i = 0; i < _table.size(); ++i)
		{
			Node* cur = _table[i];
			if (cur)
				return iterator(cur, this);
		}
		return iterator(nullptr, this);
	}

	iterator end()
	{
		return iterator(nullptr, this);
	}

	pair<iterator, bool> insert(const V& value)
	{
		checkCapacity();

		//1. 计算位置
		KeyOfValue kov;
		HF hf;
		int idx = hf(kov(value)) % _table.size();

		//2. 搜索key是否已经存在
		Node* cur = _table[idx];
		while (cur)
		{
			if (kov(cur->_value) == kov(value))
				//return false;
				return make_pair(iterator(cur, this), false);
			cur = cur->_next;
		}

		//3. 插入: 头插
		cur = new Node(value);

		cur->_next = _table[idx];
		_table[idx] = cur;

		++_size;
		//return true;
		return make_pair(iterator(cur, this), true);
	}

	size_t getNextSize(size_t n)
	{
		const int PRIMECOUNT = 28;
		const size_t primeList[PRIMECOUNT] =
		{
			53ul, 97ul, 193ul, 389ul, 769ul,
			1543ul, 3079ul, 6151ul, 12289ul, 24593ul,
			49157ul, 98317ul, 196613ul, 393241ul, 786433ul,
			1572869ul, 3145739ul, 6291469ul, 12582917ul, 25165843ul,
			50331653ul, 100663319ul, 201326611ul, 402653189ul, 805306457ul,
			1610612741ul, 3221225473ul, 4294967291ul
		};

		for (int i = 0; i < PRIMECOUNT; ++i)
		{
			if (primeList[i] > n)
				return primeList[i];
		}

		return primeList[PRIMECOUNT];
	}

	void checkCapacity()
	{
		if (_size == _table.size())
		{
			//size_t newSize = _size == 0 ? 5 : 2 * _size;
			size_t newSize = getNextSize(_size);
			vector<Node*> newHt;
			newHt.resize(newSize);
			KeyOfValue kov;
			HF hf;
			//遍历旧表中的非空单链表
			for (size_t i = 0; i < _table.size(); ++i)
			{
				Node* cur = _table[i];
				//遍历当前单链表
				while (cur)
				{
					//0. 记录旧表中的下一个元素
					Node* next = cur->_next;

					//1. 计算新的位置
					int idx = hf(kov(cur->_value)) % newHt.size();

					//2. 头插
					cur->_next = newHt[idx];
					newHt[idx] = cur;

					// 3. 处理下一个元素
					cur = next;
				}
				_table[i] = nullptr;
			}

			_table.swap(newHt);
		}
	}

	Node* find(const K& key)
	{
		if (_table.size() == 0)
			return nullptr;

		HF hf;

		int idx = hf(key) % _table.size();

		Node* cur = _table[idx];
		KeyOfValue kov;
		while (cur)
		{
			if (kov(cur->_value) == key)
				return cur;
			cur = cur->_next;
		}
		return nullptr;
	}

	bool erase(const K& key)
	{
		HF hf;
		int idx = hf(key) % _table.size();
		Node* cur = _table[idx];
		//单链表删除逻辑
		Node* prev = nullptr;

		KeyOfValue kov;
		while (cur)
		{
			if (kov(cur->_value) == key)
			{
				//删除
				//判断删除的是否为头结点
				if (prev == nullptr)
				{
					_table[idx] = cur->_next;
				}
				else
				{
					prev->_next = cur->_next;
				}

				delete cur;
				--_size;
				return true;
			}
			else
			{
				prev = cur;
				cur = cur->_next;
			}
		}
		return false;
	}

private:
	vector<Node*> _table;
	size_t _size = 0;
};

//void testHt()
//{
//	HashTable<int, int, keyOfValue<int>> ht;
//
//	ht.insert(10);
//	ht.insert(7);
//	ht.insert(35);
//	ht.insert(12);
//	ht.insert(25);
//	ht.insert(4);
//	ht.insert(14);
//	ht.insert(11);
//	ht.insert(9);
//
//	HashNode<int>* cur = ht.find(12);
//	cout << cur << endl;
//	cur = ht.find(100);
//	cout << cur << endl;
//
//	bool ret = ht.erase(12);
//	ret = ht.erase(100);
//	ret = ht.erase(25);
//}

//字符转换为整数
struct strToInt
{
	size_t operator()(const string& str)
	{
		size_t hash = 0;
		for (const auto& ch : str)
		{
			hash = hash * 131 + ch;
		}
		return hash;
	}
};

template <class K>
struct hashFun
{
	size_t operator()(const K& key)
	{
		return key;
	}
};

template <class K, class V, class HF = hashFun<K>>
class UnorderedMap
{
	struct MapKeyOfValue
	{
		const K& operator()(const pair<K, V>& value)
		{
			return value.first;
		}
	};
public:
	typedef typename HashTable<K, pair<K, V>, MapKeyOfValue, HF>::iterator iterator;

	pair<iterator, bool> insert(const pair<K, V>& value)
	{
		return _ht.insert(value);
	}

	V& operator[](const K& key)
	{
		pair<iterator, bool> ret = _ht.insert(make_pair(key, V()));
		return ret.first->second;
	}

	iterator begin()
	{
		return _ht.begin();
	}

	iterator end()
	{
		return _ht.end();
	}


private:
	HashTable<K, pair<K, V>, MapKeyOfValue, HF> _ht;
};

template <class K, class HF = hashFun<K>>
class UnorderedSet
{
	struct SetKeyOfValue
	{
		const K& operator()(const K& value)
		{
			return value;
		}
	};
public:
	typedef typename HashTable<K, K, SetKeyOfValue, HF>::iterator iterator;

	iterator begin()
	{
		return _ht.begin();
	}

	iterator end()
	{
		return _ht.end();
	}

	pair<iterator, bool> insert(const K& value)
	{
		return _ht.insert(value);
	}
private:
	HashTable<K, K, SetKeyOfValue, HF> _ht;
};

void testMap()
{
	UnorderedMap<int, int> um;
	um.insert(make_pair(1, 1));
	um.insert(make_pair(15, 15));
	um.insert(make_pair(3, 3));
	um.insert(make_pair(2, 2));
	um.insert(make_pair(14, 14));
	um.insert(make_pair(40, 40));

}

void testSet()
{
	UnorderedSet<int> um;
	um.insert(1);
	um.insert(3);
	um.insert(6);
	um.insert(18);
	um.insert(21);
	um.insert(31);

	UnorderedSet<int>::iterator it = um.begin();
	while (it != um.end())
	{
		cout << *it << " ";
		++it;
	}
	cout << endl;

}

void testSet2()
{
	UnorderedSet<string, strToInt> us;
	us.insert("123");
	us.insert("456");
	us.insert("01");
	us.insert("36");
}

void testMap2()
{
	UnorderedMap<int, int>  um;
	um[1] = 1;
	um[2] = 2;
	um[10] = 10;
	um[11] = 11;
	um[8] = 8;
	um[5] = 5;

	auto it = um.begin();
	while (it != um.end())
	{
		cout << it->first << "---->" << it->second << endl;
		++it;
	}

	um[1] = 100;

	it = um.begin();
	while (it != um.end())
	{
		cout << it->first << "---->" << it->second << endl;
		++it;
	}
}

//int main()
//{
//	//testHt();
//	//testMap();
//	//testSet();
//	//testSet2();
//	testMap2();
//	return 0;
//}