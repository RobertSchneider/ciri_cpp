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
	vector<string> meanings;
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
	vector<vector<string>> conditions;
};
struct CommandStruct
{
	vector<CommandVariable> variables;
	vector<CommandAction> actions;
};

void from_json(const json& j, CommandRequirement& r) {
	r.meanings = j.at("meanings").get<vector<string>>();
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
	if (j.find("conditions") != j.end()) a.conditions = j.at("conditions").get<vector<vector<string>>>();
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
	Word* getWordWithMeaning(Sentence *s, CommandVariable v);
	Word* matchesRequirement(CommandVariable v, Sentence *s);
	CommandVariable* getVariable(string name);
	int getVariableMinDepth(CommandVariable v);
	
	bool getActionConditionsSatisfied(CommandAction a);
	void processAction(CommandAction a);
	string formatAction(string a);
};

Command::Command(json j)
{
	jsonData = j;
	cmdStruct = j;
}

Word* Command::getWordWithMeaning(Sentence *s, CommandVariable v)
{
	vector<string> meanings = v.requirement.meanings;
	for (size_t i = 0; i < s->words.size(); i++)
	{
		for (size_t j = 0; j < meanings.size(); j++)
		{
			if (s->words[i].meaning == meanings[j]) 
			{
				if (v.requirement.depth_g.length() > 0) 
				{
					CommandVariable *dep = getVariable(v.requirement.depth_g);
					if (dep == nullptr) continue;
					int g = getVariableMinDepth(*dep);
					if (g >= s->words[i].depth) 
					{
						continue;
					}
				}
				
				return &s->words[i];
			}
		}
	}
	return nullptr;
}

Word* Command::matchesRequirement(CommandVariable v, Sentence *s)
{
	CommandRequirement r = v.requirement;
	Word *w = getWordWithMeaning(s, v);
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

int Command::getVariableMinDepth(CommandVariable v)
{
	if (v.requirement.depth >= 0) return v.requirement.depth;
	if (v.requirement.depth_g.length() > 0) 
	{
		CommandVariable *dep = getVariable(v.requirement.depth_g);
		if (dep == nullptr) return -1;
		return getVariableMinDepth(*dep) + 1;
	}
	return v.word->depth;
}

bool Command::matches(Sentence s)
{
	//create variables
	for (size_t i = 0; i < cmdStruct.variables.size(); i++)
	{
		CommandVariable *v = &cmdStruct.variables[i];
		Word *word = matchesRequirement(*v, &s);
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
			int g = getVariableMinDepth(*dep);
			if (g >= v.word->depth) return false;
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
			i--;
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
		cout << "> " << action << "\n";
		string output = exec(("cd playground; " + action).c_str());
		if (output.length() > 0)
		{
			cout << "<" <<  output << "\n";
		}
	}
}

bool Command::getActionConditionsSatisfied(CommandAction a)
{
	for	(size_t i = 0; i < a.conditions.size(); i++)
	{
		CommandVariable *var = getVariable(a.conditions[i][0]);
		if (var == nullptr) return false;
		
		bool satisfied = false; 
		for (size_t j = 1; j < a.conditions[i].size(); j++)
		{
			if (var->word->word == a.conditions[i][j])
			{
				satisfied = true;
				break;
			}
		}
		
		if (!satisfied) return false;
	}
	
	return true;
}

void Command::process(Sentence s)
{
	cout << "***PROCESSING CMD " << jsonData << "\n";
	
	for	(size_t i = 0; i < cmdStruct.actions.size(); i++)
	{
		if (getActionConditionsSatisfied(cmdStruct.actions[i]))
		{
			processAction(cmdStruct.actions[i]);
		}
	}
}

#endif