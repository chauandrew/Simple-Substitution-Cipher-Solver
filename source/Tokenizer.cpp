#include "provided.h"
#include <string>
#include <unordered_set>
#include <algorithm>
using namespace std;

class TokenizerImpl
{
public:
    TokenizerImpl(string separators);	// creates a vector of all the separators
    vector<string> tokenize(const std::string& s) const;	// return a vector of tokens 
private:
	unordered_set<char> m_separators;
	bool isSeparator(const char& a) const;
};

TokenizerImpl::TokenizerImpl(string separators)
{
	for (unsigned int i = 0; i < separators.size(); ++i)
	{
		m_separators.insert(separators[i]);
	}
}

bool TokenizerImpl::isSeparator(const char& a) const
{
	unordered_set<char>::const_iterator got = m_separators.find(a);
	if (got != m_separators.end()) return true;	// if we didn't find the separator, return true
	return false;
}

vector<string> TokenizerImpl::tokenize(const std::string& s) const
{
	vector<string> tokens;
	string cur;
	for (unsigned int i = 0; i < s.size(); ++i)	// go through the entire string s
	{
		cur = "";
		while (!isSeparator(s[i]) && i < s.size())	// add things to cur while it's not a separator
		{
			cur += s[i];
			++i;
		}
		if (cur != "")	// add the string to the vector if cur isn't empty
			tokens.push_back(cur);
	}
    return tokens; 
}

//******************** Tokenizer functions ************************************

// These functions simply delegate to TokenizerImpl's functions.
// You probably don't want to change any of this code.

Tokenizer::Tokenizer(string separators)
{
    m_impl = new TokenizerImpl(separators);
}

Tokenizer::~Tokenizer()
{
    delete m_impl;
}

vector<string> Tokenizer::tokenize(const std::string& s) const
{
    return m_impl->tokenize(s);
}
