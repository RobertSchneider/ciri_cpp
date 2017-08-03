#ifndef STRUCTS_HPP
#define STRUCTS_HPP

#include <string.h>
#include <vector>
#include "json.hpp"
#include "Word.hpp"

using namespace std;
using json = nlohmann::json;

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

#endif