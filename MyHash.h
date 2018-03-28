#ifndef MY_HASH_H
#define MY_HASH_H

#include <functional>
#include <string>

/////////////////////////////////////////////////////////////
// MyHash Interface
/////////////////////////////////////////////////////////////

template<typename KeyType, typename ValueType>
class MyHash
{
public:
	MyHash(double maxLoadFactor = 0.5)
		:m_maxLoad(maxLoadFactor), m_size(0), m_head(nullptr), m_tail(nullptr)
	{
		if (m_maxLoad > 2.0) m_maxLoad = 2.0;
		init();
	}
	~MyHash();
	void reset();
	void associate(const KeyType& key, const ValueType& value);
	int getNumItems() const;
	double getLoadFactor() const;

	// for a map that can't be modified, return a pointer to const ValueType
	const ValueType* find(const KeyType& key) const;

	// for a modifiable map, return a pointer to modifiable ValueType
	ValueType* find(const KeyType& key)
	{
		return const_cast<ValueType*>(const_cast<const MyHash*>(this)->find(key));
	}

	// C++11 syntax for preventing copying and assignment
	MyHash(const MyHash&) = delete;
	MyHash& operator=(const MyHash&) = delete;

private:

	/////////////////////////////////////////////////////////////
	// Private Node Implementation
	/////////////////////////////////////////////////////////////

	struct Node {
		Node(const KeyType& key_, const ValueType& val_)
			:next(nullptr), nextIter(nullptr), key(key_), val(val_)
		{}

		KeyType   key;
		ValueType val;
		Node*     next;		// used for each bucket
		Node*     nextIter;	// used for a list between all nodes
	};

	/////////////////////////////////////////////////////////////
	// Private data members
	/////////////////////////////////////////////////////////////

	unsigned long m_nBuckets;
	unsigned long m_size;
	double        m_maxLoad;
	Node*         m_head;
	Node*         m_tail;
	Node**        m_bucketArray;

	/////////////////////////////////////////////////////////////
	// Private member functions
	/////////////////////////////////////////////////////////////

	// returns a bucket number for a given key
	unsigned int  getBktNum(const KeyType& key) const
	{
		std::hash<KeyType> keyHash;
		unsigned int num = keyHash(key);
		return num % m_nBuckets;
	}
	// pass in an existing node and add it to the hash map
	void addExistingNode(Node* ptr)
	{
		int pos = getBktNum(ptr->key);
		Node* last = m_bucketArray[pos];
		if (!last)	// if bucket is initially empty
			m_bucketArray[pos] = ptr;	// bucket points to node
		else
		{
			while (last->next != nullptr)	// move last to the last node of a bucket
				last = last->next;
			last->next = ptr;	// add the new node to the end of the list
		}
		ptr->next = nullptr;
	}
	// doubles the current bucket size and reallocates memory for it.
	void changeBucketSize()
	{
		m_nBuckets *= 2;
		delete[] m_bucketArray;
		m_bucketArray = new Node*[m_nBuckets];
		// initialize the new array 
		for (unsigned int i = 0; i < m_nBuckets; ++i)
			m_bucketArray[i] = nullptr;

		// add values to the new array
		Node* temp = m_head;
		while (temp)	// loop through the hash tables list of all nodes
		{
			addExistingNode(temp);
			temp = temp->nextIter;
		}
		//cout << m_nBuckets << '\t' << m_size << '\t' << getNumItems() << endl;
	}
	// returns true if needs to increase number of buckets
	bool shouldIncreaseSize() const
	{
		if (getLoadFactor() > m_maxLoad)
			return true;
		else return false;
	}
	// clears all allocated memory 
	void clearMem()
	{
		Node* temp = m_head;
		while (m_head)		// delete every node
		{
			temp = m_head->nextIter;
			delete m_head;
			m_head = temp;
		}
		delete[] m_bucketArray;	// delete the dynamic array
		m_bucketArray = nullptr;
		m_size = 0;
	}
	// resets bucket size to 100
	void init()
	{
		m_nBuckets = 100;		// create a new dynamic array of node pointers
		m_bucketArray = new Node*[m_nBuckets];
		m_head = nullptr;
		m_tail = nullptr;
		for (unsigned int i = 0; i < m_nBuckets; ++i)
			m_bucketArray[i] = nullptr;
	}

};

//////////////////////////////////////////////////////////////////
// Public Member Function ImplementationsB) t
/////////////////////////////////////////////////////////////////

	template <class KeyType, class ValueType>
	MyHash<KeyType, ValueType>::~MyHash()	// delete the dynamic array and all the nodes
	{
		clearMem();
	}

	template <class KeyType, class ValueType>
	void MyHash<KeyType, ValueType>::reset()
	{
		clearMem();
		init();
	}

template <class KeyType, class ValueType>
void MyHash<KeyType, ValueType>::associate(const KeyType& key, const ValueType& value)
{
	int pos = getBktNum(key);		// find the appropriate bucket for the key
	Node* addMe = new Node(key, value);
	// first add the new node to its bucket
	if (!m_bucketArray[pos])	// if that bucket hasn't been filled yet
	{
		m_bucketArray[pos] = addMe;
		m_size++;
	}
	else	// if the bucket has other items in it
	{
		Node* curr = m_bucketArray[pos];	// last starts at wherever the bucket points
		if (!curr)	// if the bucket is emtpy, make it point to the new node
			m_bucketArray[pos] = addMe;
		else
		{
			while (curr->next != nullptr)	// go to the last slot of the bucket list
			{
				if (curr->key == key) break;	// if the key already exists, break out of the loop to update
				curr = curr->next;	// keeping moving through the node
			}
			if (curr->key == key)	// if the key/value pair already exists, update it and then return
			{
				curr->val = value;	// update
				delete addMe;		// delete the extra node we allocated earlier
				return;
			}
			curr->next = addMe;		// add the new node to the end of the list
		}
		m_size++;	// once we've added, increment the size
	}

	// then add the node to the iterator list
	if (!m_head)	// if this is the first node we add ever
	{
		m_head = addMe;	// head and tail point to the new node
		m_tail = addMe;
		m_head->nextIter = nullptr;	// set up the list for iterating through nodes
	}
	else		// if this is not the first node we add
	{
		m_tail->nextIter = addMe;	// add it to the back of the list
		m_tail = addMe;				// move tail to the back
	}
	if (shouldIncreaseSize())	// if the size is too big after adding, then change bucket size
		changeBucketSize();
}

	template <class KeyType, class ValueType>
	int MyHash<KeyType, ValueType>::getNumItems() const
	{
		return m_size;
	}

	template <class KeyType, class ValueType>
	double MyHash<KeyType, ValueType>::getLoadFactor()	const
	{
		return static_cast<double>(m_size) / static_cast<double>(m_nBuckets);
	}
	
	template <class KeyType, class ValueType>
	const ValueType* MyHash<KeyType, ValueType>::find(const KeyType& key)	const
	{
		int pos = getBktNum(key);
		Node* curr = m_bucketArray[pos];	// go to the slot the key should be in
		while (curr)	// search through the bucket for the key
		{
			if (curr->key == key)		// if we find the key, return the value's address
				return &(curr->val);
			curr = curr->next;	// otherwise continue searching
		}
		// if we didn't find it, return NULL
		return nullptr;
	}

#endif  
