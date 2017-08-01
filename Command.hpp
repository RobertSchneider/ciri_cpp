#ifndef COMMAND_HPP
#define COMMAND_HPP

#include "json.hpp"
#include "Word.hpp"

using namespace std;
using json = nlohmann::json;

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
	Word *word;
};
struct CommandAction
{
	string type;
	string value;
};
struct CommandStruct
{
	vector<CommandVariable> variables;
	vector<CommandAction> actions;
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

void from_json(const json& j, CommandAction& a) {
	a.type = j.at("type").get<string>();
	a.value = j.at("value").get<string>();
}

void from_json(const json& j, CommandStruct& cmd) {
	cmd.variables = j.at("variables").get<vector<CommandVariable>>();
	cmd.actions = j.at("actions").get<vector<CommandAction>>();
}

const char varIndicator = '$';

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
	Word* matchesRequirement(CommandRequirement r, Sentence *s);
	CommandVariable* getVariable(string name);
	void processAction(CommandAction a);
	string formatAction(string a);
};

Command::Command(json j)
{
	jsonData = j;
	cmdStruct = j;
}

Word* Command::getWordWithMeaning(Sentence *s, string meaning)
{
	for (size_t i = 0; i < s->words.size(); i++)
	{
		if (s->words[i].meaning == meaning) return &s->words[i];
	}
	return nullptr;
}

Word* Command::matchesRequirement(CommandRequirement r, Sentence *s)
{
	Word *w = getWordWithMeaning(s, r.meaning);
	if (w == nullptr) return nullptr;
	
	bool passedVals = r.values.size() == 0;
	for (size_t i = 0; i < r.values.size(); i++)
	{
		if (r.values[i] == w->word)
		{
			passedVals = true;
			break;
		}
	}
	
	if (!passedVals) return nullptr;
	if (r.depth >= 0 && w->depth != r.depth) return nullptr;
	
	return w;
}

CommandVariable* Command::getVariable(string name)
{
	for (size_t i = 0; i < cmdStruct.variables.size(); i++)
	{
		if (cmdStruct.variables[i].name == name) return &cmdStruct.variables[i];
	}
	return nullptr;
}

bool Command::matches(Sentence s)
{
	//create variables
	for (size_t i = 0; i < cmdStruct.variables.size(); i++)
	{
		CommandVariable *v = &cmdStruct.variables[i];
		Word *word = matchesRequirement(v->requirement, &s);
		if (word == nullptr) return false;
		
		v->word = word;
	}
	
	//solve depth dependencies
	for (size_t i = 0; i < cmdStruct.variables.size(); i++)
	{
		CommandVariable v = cmdStruct.variables[i];
		if (v.requirement.depth_g.length() > 0) 
		{
			CommandVariable *dep = getVariable(v.requirement.depth_g);
			if (dep == nullptr) return false;
			if (dep->requirement.depth >= v.word->depth) return false;
		}
	}
	
	return true;
}

string Command::formatAction(string a)
{
	string result = "";
	for(std::string::size_type i = 0; i < a.size(); i++) 
	{
		char c = a[i];
		if (c == varIndicator)
		{
			string var = "";
			i++;
			while(i < a.size() && a[i] != ' ')
			{
				var += a[i];
				i++;
			}
			
			CommandVariable *v = getVariable(var);
			if (v != nullptr)
			{
				result += v->word->word;
			}
		}else
		{
			result += c;
		}
	}
	return result;
}

void Command::processAction(CommandAction a)
{
	string action = formatAction(a.value);
	if (a.type == "say") 
	{
		cout << "+++ " << action << "\n";
	}
	if (a.type == "sh")
	{
		exec(action.c_str());
	}
}

void Command::process(Sentence s)
{
	cout << "***PROCESSING CMD " << jsonData << "\n";
	
	for	(size_t i = 0; i < cmdStruct.actions.size(); i++)
	{
		processAction(cmdStruct.actions[i]);
	}
}

#endif