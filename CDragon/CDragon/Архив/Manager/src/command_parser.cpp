#include "command_parser.h"
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

string load_language_setting() {
    ifstream config("output/config.txt");
    string line;
    string language = "en"; // язык по умолчанию

    if (config.is_open()) {
        while (getline(config, line)) {
            if (line.find("language=") == 0) {
                language = line.substr(9);
                break;
            }
        }
        config.close();
    } else {
        cerr << "Error: failed to open the config.txt settings file." << endl;
    }

    return language;
}

// старая ненужная
vector<string> split_command(const string& input) {
    vector<string> result;
    istringstream iss(input);
    string word;

    while (iss >> word) {
        result.push_back(word);
    }
    return result;
}

