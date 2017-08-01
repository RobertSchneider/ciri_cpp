#ifndef COMMAND_HPP
#define COMMAND_HPP

#include "json.hpp"
#include "Word.hpp"

using namespace std;
using json = nlohmann::json;

struct CommandRequirement
{
	string meaning;
	vector<string> values;
	int depth;
	string depth_g;
	string depth_l;
};
struct CommandVariable
{
	string name;
	CommandRequirement requirement;
	//runtime
	string value;
};
struct CommandStruct
{
	vector<CommandVariable> variables;
};

void from_json(const json& j, CommandRequirement& r) {
	r.meaning = j.at("meaning").get<string>();
	if (j.find("values") != j.end()) r.values = j.at("values").get<vector<string>>();
	if (j.find("depth") != j.end()) r.depth = j.at("depth").get<int>(); else r.depth = -1;
	if (j.find("depth_g") != j.end()) r.depth_g = j.at("depth_g").get<string>(); else r.depth_g = "";
	if (j.find("depth_l") != j.end()) r.depth_l = j.at("depth_l").get<string>(); else r.depth_l = "";
}

void from_json(const json& j, CommandVariable& v) {
	v.name = j.at("name").get<string>();
	v.requirement = j.at("requirement").get<CommandRequirement>();
}

void from_json(const json& j, CommandStruct& cmd) {
	cmd.variables = j.at("variables").get<vector<CommandVariable>>();
}

class Command
{
public:
	json jsonData;
	struct CommandStruct cmdStruct;
	
	Command(json j);
	bool matches(Sentence s);
	void process(Sentence s);
	
private:
	Word* getWordWithMeaning(Sentence *s, string meaning);
	bool matchesRequirement(CommandRequirement r, Sentence *s, Word *wrd);
};

Command::Command(json j)
{
	jsonData = j;
	cmdStruct = j;
}

Word* Command::getWordWithMeaning(Sentence *s, string meaning)
{
	for (int i = 0; i < s->words.size(); i++)
	{
		if (s->words[i].meaning == meaning) return &s->words[i];
	}
	return nullptr;
}

bool Command::matchesRequirement(CommandRequirement r, Sentence *s, Word *wrd)
{
	Word *w = getWordWithMeaning(s, r.meaning);
	if (w == nullptr) return false;
	
	bool passedVals = r.values.size() == 0;
	for (int i = 0; i < r.values.size(); i++)
	{
		if (r.values[i] == w->word)
		{
			passedVals = true;
			break;
		}
	}
	
	if (!passedVals) return false;
	if (r.depth >= 0 && w->depth != r.depth) return false;
	
	*wrd = *w;
	return true;
}

bool Command::matches(Sentence s)
{
	//create variables
	for (int i = 0; i < cmdStruct.variables.size(); i++)
	{
		CommandVariable v = cmdStruct.variables[i];
		Word word;
		bool matches = matchesRequirement(v.requirement, &s, &word);
		if (!matches) return false;
		
		v.value = word.word;
	}
	
	return true;
}

void Command::process(Sentence s)
{
	cout << "***PROCESSING CMD " << jsonData << "\n";
}

#endif