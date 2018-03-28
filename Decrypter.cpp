#include "provided.h"
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_set>
#include <iostream>
using namespace std;

class DecrypterImpl
{
public:
	DecrypterImpl();
	~DecrypterImpl();
    bool load(string filename);	// load words to use in decrypting
    vector<string> crack(const string& ciphertext);	// return a vector of all possible translations
private:
	WordList*   m_wordList;
	Tokenizer   m_tokenizer;
	Translator* m_translator;

	void   crackHelper(string ciphertext_message, string cipherWord, string candidate, vector<string>& output);
	bool   containsChar(const string& word, const char& character) const;	// returns true if a word contains a character
	string getNextWord(const string& ciphertext) const;	// returns the most translated version of the next word we should translate
	bool   isValidSoFar(const string& message) const;	// true if all words translated SO FAR are valid
	bool   isFullyTranslated(const string& message) const;	// true if no '?' left, false if there are questions marks 
};

DecrypterImpl::DecrypterImpl()
	:m_tokenizer(",;:.!()[]{}-\"#$%^& 1234567890")	// create the tokenizer with all of these different separators
{
	m_wordList   = new WordList;
	m_translator = new Translator;
}

DecrypterImpl::~DecrypterImpl()
{
	delete m_wordList;
	delete m_translator;
}

bool DecrypterImpl::load(string filename)
{
	delete m_wordList;
	m_wordList = new WordList;
	return m_wordList->loadWordList(filename);
}

vector<string> DecrypterImpl::crack(const string& ciphertext)
{
	string curr = getNextWord(ciphertext);	// get the next word to evaluate
	string currTranslated = m_translator->getTranslation(curr);		// get the translated version of our next word to evaluate
	vector<string> candidates = m_wordList->findCandidates(curr, currTranslated);	// find valid candidates for our word
	if (candidates.empty()) return vector<string>();	// if no candidates, return an empty vector
	vector<string> output;	// create a vector to hold any correct translations
	for (unsigned int i = 0; i < candidates.size(); ++i)
	{
		crackHelper(ciphertext, curr, candidates[i], output);
	}
	std::sort(output.begin(), output.end());	// i can't get myhash to store in a sorted list, so i do this lazy thing instead to return a sorted vector
	return output;
}

void DecrypterImpl::crackHelper(string ciphertext_message, string cipherWord, string candidate, vector <string>& output)
{	
	// if this current candidate is not translatable, return. If It is, push the entire new mapping onto the stack at once
	if (!m_translator->pushMapping(cipherWord, candidate)) return;	
	
	string translatedMessage = m_translator->getTranslation(ciphertext_message);	// translate the entire ciphertext message using this translation
	
	// compute these bools so we don't have to repeatedly dive into functions unnecessarily for the next few if statements
	bool fullyTranslated = isFullyTranslated(translatedMessage);
	bool validSoFar = isValidSoFar(translatedMessage);

	// if not valid, then pop the mapping and return false
	if (!validSoFar)
	{
		m_translator->popMapping();
		return;
	}

	// if not fully translated but valid so far, recurse
	if (validSoFar && !fullyTranslated) {
		vector<string> temp = crack(ciphertext_message);	
		output.insert(output.end(), temp.begin(), temp.end());	// add whatever we get from recursion to this output vector
		m_translator->popMapping();	// pop the current mapping, regardless of what temp has in it
	}

	// if fully translated and valid, then this IS a completely valid translation of the cipher
	if (fullyTranslated && validSoFar) {
		m_translator->popMapping();	// get rid of the current mapping
		output.push_back(translatedMessage);	// add this message to the translated vector
	}
}

bool   DecrypterImpl::containsChar(const string& word, const char& character) const
{
	for (unsigned short i = 0; i < word.size(); ++i)
		if (word[i] == character) return false;
	return true;
}
bool   DecrypterImpl::isValidSoFar(const string& message) const
{
	vector<string> separatedWords = m_tokenizer.tokenize(message);
	for (unsigned int i = 0; i < separatedWords.size(); ++i)
	{
		if (!containsChar(separatedWords[i], '?')) continue;		// if a word has any question marks, skip it
		if (!m_wordList->contains(separatedWords[i]))	return false;	// if a complete word is not valid, return false
	}
	return true;
}
string DecrypterImpl::getNextWord(const std::string& ciphertext) const
{
	vector<string> untranslatedVec = m_tokenizer.tokenize(ciphertext);			// tokenize the untranslated vector
	string encrypted = m_translator->getTranslation(ciphertext);	// translate the current string 
	vector<string> translatedVec = m_tokenizer.tokenize(encrypted);	// tokenize the translated string
	int posMostUnknown(0), mostUnknown(0);
	for (unsigned int i = 0; i < translatedVec.size(); ++i)	// go through vector of words
	{
		int currUnknown(0);
		for (unsigned int j = 0; j < translatedVec[i].size(); ++j)	// for each word, count the number of unknown characters
		{
			if (translatedVec[i][j] == '?')
				currUnknown++;
		}
		if (currUnknown > mostUnknown)	// if a word has more unknown characters, hold onto its position
		{
			posMostUnknown = i;
			mostUnknown = currUnknown;
		}
		else if (currUnknown == mostUnknown)	// if both words have the same number of unknown characters, return the longer word
			if (translatedVec[i].size() > translatedVec[posMostUnknown].size()) posMostUnknown = i;
	}
	return untranslatedVec[posMostUnknown];
}
bool   DecrypterImpl::isFullyTranslated(const string& message) const
{
	for (unsigned int i = 0; i < message.size(); ++i)
		if (message[i] == '?') return false;
	return true;
}

//******************** Decrypter functions ************************************

// These functions simply delegate to DecrypterImpl's functions.
// You probably don't want to change any of this code.

Decrypter::Decrypter()
{
    m_impl = new DecrypterImpl;
}

Decrypter::~Decrypter()
{
    delete m_impl;
}

bool Decrypter::load(string filename)
{
    return m_impl->load(filename);
}

vector<string> Decrypter::crack(const string& ciphertext)
{
   return m_impl->crack(ciphertext);
}