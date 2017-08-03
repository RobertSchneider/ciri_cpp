#ifndef COMMAND_HPP
#define COMMAND_HPP

#include "json.hpp"
#include "Word.hpp"
#include "Structs.hpp"

using namespace std;
using json = nlohmann::json;

std::string exec(string cmd, bool wrapper = false) {
	
	if (wrapper)
	{
		cmd = "python run.py \"" + cmd + "\"";
	}

    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(_popen(cmd.c_str(), "r"), _pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
            result += buffer.data();
    }
    return result;
}

const char varIndicator = '$';

class Command
{
public:
	json jsonData;
	struct CommandStruct cmdStruct;
	
	Command(json j);
	Command();
	bool matches(Sentence s);
	void process(Sentence s);

	string currentSentence;
	
private:
	Word* getWordWithMeaning(Sentence *s, CommandVariable v);
	Word* matchesRequirement(CommandVariable v, Sentence *s);
	CommandVariable* getVariable(string name);
	int getVariableMinDepth(CommandVariable v);
	
	bool getActionConditionsSatisfied(CommandAction a);
	void processAction(CommandAction a);
	string formatAction(string a);
};

void from_json(const json& j, Command& cmd) {

}

Command::Command(json j)
{
	jsonData = j;
	cmdStruct = j;
}

Command::Command()
{

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

void say(string s)
{
	exec(("python say.py \"" + s + "\" &").c_str());
}

void Command::processAction(CommandAction a)
{
	string action = formatAction(a.value);
	if (a.type == "say") 
	{
		cout << "+++ " << action << "\n";
		currentSentence += action;
	}
	if (a.type == "sh")
	{
		cout << "> " << action << "\n";
		string output = exec(("cd playground && " + action).c_str(), true);
		if (output.length() > 0)
		{
			cout << "<" << output << "\n";
			currentSentence += output;
		}
	}
	if (a.type == "commit")
	{
		say(currentSentence);
		currentSentence = "";
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
	currentSentence = "";
	cout << "***PROCESSING CMD " << jsonData << "\n";
	
	for	(size_t i = 0; i < cmdStruct.actions.size(); i++)
	{
		if (getActionConditionsSatisfied(cmdStruct.actions[i]))
		{
			processAction(cmdStruct.actions[i]);
		}
	}

	if (currentSentence.length() > 0)
	{
		say(currentSentence);
		currentSentence = "";
	}
}

#endif