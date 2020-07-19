/*
即使负载因子为1, 也不一定每个位置上都有元素 ,可能一个位置上挂了很多元素, 总共的元素个数等于位置总数
插入: 时间复杂度最差为O(logN)
增容:
	遍历旧表当中的每一个元素(非空单链表)
	计算每一个元素在新表中的位置
	把元素重新挂载到新表对应的位置
迭代器: 
	1. _next非空: 更新到_next
	2. _next为空: 更新到下一个非空链表头结点
HashTable: K,V KeyOfvalue, HashFun
	K: 数据键值
	V: 键值对应的value, unordered_map ---> pair<K,V>, unordered_set --> K
	KeyOfvalue: 获取value对应的键值
	HashFun: 把键值K转换成整数数据(非整数类型的转换, 非整数 ---> 映射 ---> 整数)
	哈希迭代器: 前置声明, 友元类声明
	成员: 节点, 哈希表指针
	++操作: 
		next是否为空: 
			不为空: 更新到next结点
			为空: 
				1. 首先通过kov获取键值
				2. 通过hashFun计算键值对应的整数值
				3. 通过整数值计算当前结点在哈希表中的位置
				4. 从上一步计算位置的下一个位置开始, 寻找第一个非空链表的头结点
				5. 如果没有找到, 更新为空指针

				不需要使用表的内容, 需要哈希表的大小和表, 需要访问哈希表的私有成员--> 友元函数

		begin: 第一个非空链表的头结点
		end: 空节点
		哈希表(开散列): 可以存放任意类型的数据 一般采用开散列实现哈希表, 
			如果键值类型为非数值类型, 可以通过hashFun转换为整数值
			插入,查找,删除:
				首先需要进行的操作为: 通过键值计算位置
				然后进行类似单链表的操作


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

	bool insert(const V& value)
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
				return false;
			cur = cur->_next;
		}

		//3. 插入: 头插
		cur = new Node(value);

		cur->_next = _table[idx];
		_table[idx] = cur;

		++_size;
		return true;
	}

	void checkCapacity()
	{
		if (_size == _table.size())
		{
			size_t newSize = _size == 0 ? 5 : 2 * _size;
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
	bool insert(const pair<K, V>& value)
	{
		return _ht.insert(value);
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

	bool insert(const K& value)
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

int main()
{
	//testHt();
	//testMap();
	//testSet();
	testSet2();
	return 0;
}