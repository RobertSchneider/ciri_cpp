#ifndef WORD_HPP
#define WORD_HPP

#include <string>
#include <sstream>

using namespace std;

class Word
{
public:
	int index;			//1
	string word;		//create
	string wordType;	//VERB
	int depth;			//4
	string meaning;		//NOUN
};

class Sentence
{
public:
	vector<Word> words;
};

ostream & operator<<(ostream & str, Word const & v) { 
	str << to_string(v.index) + " " + v.word + " " + v.wordType + " " + to_string(v.depth) + " " + v.meaning;
	return str;
}

#endif // WORD_HPP
