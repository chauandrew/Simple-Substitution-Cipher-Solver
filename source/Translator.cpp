#include "provided.h"
#include "MyHash.h"
#include <vector>
#include <unordered_map>
#include <iostream>
#include <string>
using namespace std;

class TranslatorImpl
{
public:
	TranslatorImpl();
	bool pushMapping(string ciphertext, string plaintext);	// add a mapping from cipher -> plain
	bool popMapping();	// put the top map onto the current mapping
	string getTranslation(const string& ciphertext) const;	// translates input using the current map

private:
	unordered_map<char, char> m_currMap;
	std::vector<unordered_map<char, char>> m_Stack;	//vector of mapping tables, to be used as a stack
	unsigned int m_size;		// keeps track of where the top of the stack is
	bool isValidPair(const char& key, const char& value) const;
};


TranslatorImpl::TranslatorImpl()
	:m_size(0)
{
	for (int i = 0; i < 26; ++i)	// initialize the starting map with uppercase letters
		m_currMap['A' + i] = '?';
}

bool TranslatorImpl::pushMapping(string ciphertext, string plaintext)
{
	// check to make sure both strings are valid
	if (ciphertext.size() != plaintext.size())	return false;	// strings must match in size
																// initialize current map
	unordered_map<char, char> tempMap = m_currMap;
	// check to make sure that the character mappings are valid letters and consistent
	for (unsigned short i = 0; i < ciphertext.size(); ++i)
	{
		if (!isalpha(ciphertext[i]) || !isalpha(plaintext[i]))	// check that they're letters
			if (ciphertext[i] != plaintext[i]) return false;
		if (!isValidPair(ciphertext[i], plaintext[i])) return false;	// check that the pair is valid
	}
	// Once we reach here, both strings are valid. update the current mapping, update the stack, and return true
	for (unsigned short i = 0; i < ciphertext.size(); ++i)
		tempMap[toupper(ciphertext[i])] = toupper(plaintext[i]);
	m_Stack.push_back(m_currMap);
	m_size++;
	m_currMap = tempMap;
	return true;
}

// make it so we actually get rid of the things.
bool TranslatorImpl::popMapping()
{
	if (m_size == 0) return false;	// if stack is empty, return false
	m_currMap = m_Stack[m_size - 1];	// update the current map to the top of the stack
	m_Stack.pop_back();
	m_size--;	// reduce the size of the stack 
	return true;  // This compiles, but may not be correct
}

bool TranslatorImpl::isValidPair(const char& key, const char& value) const
{
	unordered_map<char, char>::const_iterator posKey = m_currMap.find(toupper(key));	// get the position/place of the key
	// if value appears anywhere in the map besides where the key is return false
	for (unordered_map<char, char>::const_iterator posVal = m_currMap.begin() ; posVal != m_currMap.end() ; posVal++)
	{
		if (posVal == posKey) continue;
		if (posVal->second == toupper(value)) return false;
	}
	// if the key is unmapped or already maps to the correct value, return true
	if (posKey->second == toupper(value) || posKey->second == '?') return true; 
	return false;
}

string TranslatorImpl::getTranslation(const string& ciphertext) const
{
	string translated = "";
	char cur;
	for (unsigned int i = 0; i < ciphertext.size(); ++i)
	{
		if (!isalpha(ciphertext[i]))	// if the character isn't a letter, just add it to the translated string
			cur = ciphertext[i];
		else					// if the character is a letter, then add the value it maps to to the translated string
		{
			unordered_map<char, char>::const_iterator it = m_currMap.find(toupper(ciphertext[i]));
			if (it != m_currMap.end())	cur = it->second;
		}

		// make sure we keep lowercase things lowercase and uppercase things uppercase
		if (islower(ciphertext[i])) cur = tolower(cur);
		translated += cur;
	}
	return translated;
}

//******************** Translator functions ************************************

// These functions simply delegate to TranslatorImpl's functions.
// You probably don't want to change any of this code.

Translator::Translator()
{
	m_impl = new TranslatorImpl;
}

Translator::~Translator()
{
	delete m_impl;
}

bool Translator::pushMapping(string ciphertext, string plaintext)
{
	return m_impl->pushMapping(ciphertext, plaintext);
}

bool Translator::popMapping()
{
	return m_impl->popMapping();
}

string Translator::getTranslation(const string& ciphertext) const
{
	return m_impl->getTranslation(ciphertext);
}
