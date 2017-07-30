#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <sstream>
#include <vector>
#include <iterator>
#include <fstream>
#include <streambuf>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

template<typename Out>
void split(const std::string &s, char delim, Out result) {
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        *(result++) = item;
    }
}


std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}

std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
            result += buffer.data();
    }
    return result;
}

class Word
{
public:
	int index;
	string word;
	string wordType;
	int depth;
	string meaning;
};

ostream & operator<<(ostream & str, Word const & v) { 
	str << to_string(v.index) + " " + v.word + " " + v.wordType + " " + to_string(v.depth) + " " + v.meaning;
	return str;
}

void loadConfigs()
{
	ifstream t("configs/create.cmd");
	string str((istreambuf_iterator<char>(t)), istreambuf_iterator<char>());

	auto jcreate = json::parse(str);
	cout << jcreate.dump();
}


int main()
{
	cout << "***CIRI****\n";

	loadConfigs();

	string response;
	getline(cin, response);

	string rawResult = exec(("python test.py \"" + response+"\"").c_str());

	vector<string> lines = split(rawResult, '\n');

	vector<Word> words;

	cout << rawResult;
	for (int i = 0; i < lines.size(); i++)
	{
		vector<string> parts = split(lines[i], '\t');
		if (parts.size() > 0)
		{
			Word word;
			word.index = stoi(parts[0]);
			word.word = parts[1];
			word.wordType = parts[3];
			word.depth = stoi(parts[6]);
			word.meaning = parts[7];

			words.push_back(word);
			cout << word << "\n";
		}
	}

	return 0;
}