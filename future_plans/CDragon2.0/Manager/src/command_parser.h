#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include <string>
#include <vector>

using namespace std;

string load_language_setting();
vector<string> split_command(const string& input);

#endif // COMMAND_PARSER_H
