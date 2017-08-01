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
#include <dirent.h>
#include <sys/types.h>
#include "Command.hpp"
#include "Word.hpp"

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

vector<string> getFiles(const char *path)
{
	vector<string> files;
    struct dirent *entry;
    DIR *dir = opendir(path);
    if (dir == NULL) {
        return files;
    }

    while ((entry = readdir(dir)) != NULL) {
        files.push_back(entry->d_name);
    }

    closedir(dir);
    return files;
}

vector<Command> commands;

void loadConfigs()
{
	std::vector<string> configs = getFiles("configs/");

	for (int i = 0; i < configs.size(); ++i)
	{
		string file = configs[i];
		if (file.find(".cmd") != string::npos)
		{
			cout << "***FOUND CONFIG " << file << "***\n";
			ifstream t("configs/" + file);
			string str((istreambuf_iterator<char>(t)), istreambuf_iterator<char>());

			json jcreate = json::parse(str);
			commands.push_back(jcreate);
		}
	}
}

void processCommand(vector<Word> words)
{
	Sentence s;
	s.words = words;
	
	for	(int i = 0; i < commands.size(); i++)
	{
		bool matches = commands[i].matches(s);
		if (matches)
		{
			commands[i].process(s);
		}
	}
}

int main()
{
	cout << "***CIRI****\n";

	loadConfigs();

	string response = "create a file called test";
	vector<Word> ws;
	ws.push_back({1, "create", "VERB", 0,"ROOT"});
	ws.push_back({2, "a", "DET", 3,"det"});
	ws.push_back({3, "file", "NOUN", 1,"dobj"});
	ws.push_back({4, "called", "VERB", 3,"partmod"});
	ws.push_back({5, "test", "NOUN", 4,"dep"});

	processCommand(ws);
	return 0;
	
	//getline(cin, response);

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
		}
	}
	
	processCommand(words);

	return 0;
}