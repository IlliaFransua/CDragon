#ifndef TEMPLATE_ENGINE_H
#define TEMPLATE_ENGINE_H

#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <sstream>
#include <regex>
#include <stdexcept>

using std::string, std::unordered_map, std::vector;
using std::ifstream, std::stringstream, std::streamsize;
using std::regex, std::regex_search, std::smatch;
using std::runtime_error;

class TemplateRenderer {
public:
    static const string render(
        const string& templatePath, 
        const unordered_map<string, vector<string>>& data
        ) {
        string result = readFileToString(templatePath);

        replaceCycles(result, data);
        replaceVariables(result, data);

        return minifyHTML(result);
    }

private:
    static string minifyHTML(const std::string& html) {
        string compressed = html;

        compressed = std::regex_replace(compressed, std::regex(R"(<!--.*?-->)"), "");
        compressed = std::regex_replace(compressed, std::regex(">\\s+<"), "><");

        return compressed;
    }

    static string readFileToString(const string& filePath) {
        ifstream file(filePath);
        if (!file.is_open()) {
            throw runtime_error("Could not open file");
        }

        string content;

        file.seekg(0, std::ios::end);
        content.reserve(file.tellg());
        file.seekg(0, std::ios::beg);

        content.assign((std::istreambuf_iterator<char>(file)),
                    std::istreambuf_iterator<char>());

        return content;
    }

    static void replaceCycles(
        string& output, 
        const unordered_map<string, vector<string>>& data
        ) {
        regex regexPattern(R"(\{\{ for (\w+) in (\w+) \}\}([\s\S]*?)\{\{ endfor \}\})", std::regex_constants::ECMAScript);
        smatch match;

        while (regex_search(output, match, regexPattern)) {
            string item = match[1];
            string list = match[2];
            string scope = match[3];

            auto it = data.find(list);
            if (it != data.end()) {
                string result;

                for (const string& value : it->second) {
                    string rendered_scope = scope;
                    string placeholder = "{{ " + item + " }}";

                    size_t pos = rendered_scope.find(placeholder);
                    while (pos != string::npos) {
                        rendered_scope.replace(pos, placeholder.length(), value);
                        pos = rendered_scope.find(placeholder, pos + value.length());
                    }

                    result += rendered_scope;
                }
                
                output.replace(match.position(0), match.length(0), result);
            }
        }
    }

    static void replaceVariables(
        string& output, 
        const unordered_map<string, vector<string>>& data
        ) {
        for (const auto& [key, values] : data) {
            string placeholder = "{{ " + key + " }}";
            size_t pos = output.find(placeholder);
            
            while (pos != string::npos) {
                string replacement = values.empty() ? "" : values.at(0);
                output.replace(pos, placeholder.length(), replacement);
                pos = output.find(placeholder, pos + replacement.length());
            }
        }
    }
};

#endif // TEMPLATE_ENGINE_H
