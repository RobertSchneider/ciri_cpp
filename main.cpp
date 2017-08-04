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
#include <sys/types.h>
#include <windows.h>
#include <tchar.h> 
#include <stdio.h>
#include <strsafe.h>
#include <thread>

#include "Command.hpp"
#include "Word.hpp"
#include "Structs.hpp"
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

vector<string> getFiles(const char *path)
{
	vector<string> files;

	HANDLE hfind;
	WIN32_FIND_DATA data;

	hfind = FindFirstFile(path, &data);
	if (hfind != INVALID_HANDLE_VALUE)
	{
		do
		{
			files.push_back(data.cFileName);
		} while (FindNextFile(hfind, &data));
	}

	return files;
}

vector<Command> commands;

void loadConfigs()
{
	string path = "E:/Development/ciri_cpp/configs/";
	std::vector<string> configs = getFiles((path + "/*").c_str());

	for (size_t i = 0; i < configs.size(); ++i)
	{
		string file = configs[i];
		if (file.find(".cmd") != string::npos)
		{
			cout << "***FOUND CONFIG " << file << "***\n";
			ifstream t(path + file);
			string str((istreambuf_iterator<char>(t)), istreambuf_iterator<char>());

			json jcreate = json::parse(str);
			commands.push_back(Command(jcreate));
		}
	}
}

void processCommand(vector<Word> words)
{
	Sentence s;
	s.words = words;
	
	for	(size_t i = 0; i < commands.size(); i++)
	{
		bool matches = commands[i].matches(s);
		if (matches)
		{
			commands[i].process(s);
		}
	}
	
	cout << "** FINISHED PROCESSING\n";
}

void processSentence(string response)
{
	string rawResult = exec(("python nlp.py \"" + response + "\"").c_str());
	vector<string> lines = split(rawResult, '\n');

	vector<Word> words;

	cout << rawResult;
	for (size_t i = 0; i < lines.size() - 1; i++)
	{
		vector<string> parts = split(lines[i], ' ');
		if (parts.size() > 0)
		{
			Word word;
			word.index = stoi(parts[0]);
			word.word = parts[1];
			word.wordType = parts[2];
			word.depth = stoi(parts[3]);
			word.meaning = parts[4];

			if (word.meaning == "ROOT") word.depth = 0;

			words.push_back(word);
		}
	}

	processCommand(words);
}

void listeningThread()
{
	while (true)
	{
		string output = exec("python speech.py");
		if (output.length() > 0)
		{
			cout << output;
			processSentence(output);
		}
	}
}

int main()
{
	cout << "***CIRI****\n";

	loadConfigs();

	thread t1(listeningThread);

	/*string response = "create a file called test";
	vector<Word> ws;
	ws.push_back({1, "create", "VERB", 0,"ROOT"});
	ws.push_back({2, "a", "DET", 3,"det"});
	ws.push_back({3, "file", "NOUN", 1,"dobj"});
	ws.push_back({4, "called", "VERB", 3,"partmod"});
	ws.push_back({5, "test", "NOUN", 4,"dep"});

	processCommand(ws);
	return 0;*/
	
	while(true)
	{
		string response = "";
		getline(cin, response);

		//string additional = " 3>&1 2>&3 3>&- | grep -v \":\ I\ \" | grep -v \"WARNING:tensorflow\" | grep -v ^pciBusID | grep -v ^major: | grep -v ^name: |grep -v ^Total\\ memory:|grep -v ^Free\\ memory:|grep -v \"INFO:\"|grep -v \"cpu_feature_guard.cc\"";
		
		processSentence(response);
	}

	return 0;
}