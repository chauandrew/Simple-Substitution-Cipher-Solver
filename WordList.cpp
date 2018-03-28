#include "provided.h"
#include "MyHash.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <functional>
using namespace std;


class WordListImpl
{
public:
	WordListImpl();
	~WordListImpl();
    bool loadWordList(string filename);	// load words into the wordlist
    bool contains(string word) const;	// returns true if a word is in the list
    vector<string> findCandidates(string cipherWord, string currTranslation) const;	// returns a vector of potential candidates given an untranslated and (partially) translated word
private:
	MyHash<string, vector<string>>* m_patternToWords;
	bool matchesTranslation(const std::string& input, const std::string& currTranslation) const;
	// gets the pattern of a word
	string getPattern(const string& input) const 
{
	char replacements[]{ 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K',
		'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S' /*, 'T', 'U', 'V', 'W', 'X', 'Y', 'Z' */};
	short usedChars[27];
	for (int i = 0; i < 27; ++i) usedChars[i] = -1;
	unsigned short counter = 0;
	string temp = "";
	unsigned char cur;

	// go through the string and create a pattern for it
	for (unsigned int i = 0; i < input.size(); ++i)
	{
		int charVal;
		if      (isupper(input[i])) charVal = input[i] - 'A';	// deal with uppercase, lowercase, and apostrophes separately
		else if (islower(input[i])) charVal = input[i] - 'a';
		else    { temp += input[i]; continue; }		// if the character isn't a letter (!, ', etc.), add it to the word unchanged

		if (usedChars[charVal] == -1)	// If we haven't used this letter yet
		{
			cur = replacements[counter];	// change the letter
			usedChars[charVal] = counter;	// mark the letter as used and save the key it maps to
			counter++;
		}
		else	// if we have already used this letter
		{
			short t = usedChars[charVal];
			cur = replacements[t];
		}
		temp += tolower(cur);
	}
	return temp;
}	
};

WordListImpl::WordListImpl()
{
	m_patternToWords = new MyHash<std::string, std::vector<std::string>>;
}

WordListImpl::~WordListImpl()
{
	delete m_patternToWords;
}

bool WordListImpl::matchesTranslation(const std::string& input, const std::string& currTranslation) const
{
	for (unsigned int i = 0; i < currTranslation.size(); ++i)	// loop through both strings
	{
		if (currTranslation[i] == '?') continue;
		else if (input[i] != currTranslation[i]) return false;	// if the values don't match, then return false
	}
	return true;	// if all values match, return true
}

bool WordListImpl::loadWordList(std::string filename)
{
	m_patternToWords->reset();	// first reset
	ifstream wordfile(filename);
	if (!wordfile)	// if we can't find/open the file, return false
		return false;
	std::string currStr;
	bool skip;	// loop through the text file
	while (getline(wordfile, currStr))
	{
		skip = false;
		for (unsigned int i = 0; i < currStr.size(); ++i)	// make sure every character in the word is a letter or an \'
		{
			if (!isalpha(currStr[i]) && currStr[i] != '\'') {
				skip = true;	break;
			}
			if (isupper(currStr[i])) currStr[i] = tolower(currStr[i]);		// store words as all lowercase
		}
		if (skip) continue;	// if word has weird characters, skip it
		
		// add the word to the hash map
		string currPattern = getPattern(currStr);	// get the pattern of the current string
		vector<string>* checkRepeat = (m_patternToWords->find(currPattern));	// checkRepeat gets the address of any existing pattern's word list vector
		vector<string> currVec;
		if(checkRepeat) 
			checkRepeat->push_back(currStr);		// If vector isn't empty, update it
		else		// otherwise, add the new vector to the map
		{
			currVec.push_back(currStr);		// add
			m_patternToWords->associate(currPattern, currVec);
		}
	}
	return true;
}

bool WordListImpl::contains(string word) const
{
	// since we store our words as lowercase, we want to search through a lowercase version of the word
	for (unsigned int i = 0; i < word.size(); ++i)
		if (isalpha(word[i])) word[i] = tolower(word[i]);

	string currPattern = getPattern(word);
	vector<string>* wordList = m_patternToWords->find(currPattern);	// find the vector for the pattern of the word we're searching for
	if (wordList)											// if the word pattern has been found
		for (unsigned int i = 0; i < wordList->size(); ++i)	// search through the vector of all the words with that same pattern
			if ((*wordList)[i] == word) return true;	// if we find it, return true. Otherwise return false
	return false;	// return false if the pattern and/or word wasn't found
}

vector<string> WordListImpl::findCandidates(string cipherWord, string currTranslation) const
{
	vector<string> *possCandidates, allCandidates;	// initialize variables

	// since we store our words as lowercase, we want to search through a lowercase translation to see if it matches the word
	for (unsigned int i = 0; i < currTranslation.size(); ++i)
		if (isalpha(currTranslation[i])) currTranslation[i] = tolower(currTranslation[i]);

	possCandidates = m_patternToWords->find(getPattern(cipherWord));	// search for the pattern of the cipher word
	if (possCandidates)	// if pattern exists
		for (unsigned int i = 0; i < possCandidates->size(); ++i)	// run the pattern against the current translation
			if (matchesTranslation((*possCandidates)[i], currTranslation))
				allCandidates.push_back((*possCandidates)[i]);	// if a word has the proper pattern and matches the translation, add it to the vector
	return allCandidates;
}


//******************** WordList functions ************************************

// These functions simply delegate to WordListImpl's functions.
// You probably don't want to change any of this code.

WordList::WordList()
{
    m_impl = new WordListImpl;
}

WordList::~WordList()
{
    delete m_impl;
}

bool WordList::loadWordList(string filename)
{
    return m_impl->loadWordList(filename);
}

bool WordList::contains(string word) const
{
    return m_impl->contains(word);
}

vector<string> WordList::findCandidates(string cipherWord, string currTranslation) const
{
   return m_impl->findCandidates(cipherWord, currTranslation);
}
