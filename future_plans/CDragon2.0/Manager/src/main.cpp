// Это скрипт управления проектом,
// позволяющий выполнять различные команды,
// такие как запуск сервера, миграции базы данных,
// создание суперпользователя и т.д.

#include <iostream>
#include <string>
#include <vector>
#include "language_pack.h"
#include "command_parser.h"
#include "command_executor.h"

using namespace std;

int main(int argc, char *argv[]) {
    string language = load_language_setting();

    LanguagePack* lang_pack;
    if (language == "uk") {
        lang_pack = &lang_uk;
    } else if (language == "ru") {
        lang_pack = &lang_ru;
    } else if (language == "de") {
        lang_pack = &lang_de;
    } else {
        lang_pack = &lang_en;
    }

    if (argc > 1) {
        vector<string> command;

        // з 1 тому що 0 це путь до CDragon Manager
        for (int i = 1; i < argc; ++i) {
            command.push_back(argv[i]);
        }

        if (command[0] == "create" && command.size() >= 3) {
            if (command[1] == "project") {
                create_project(command[2]);
                cout << lang_pack->create_project_msg << command[2] << endl;
            } else if (command[1] == "app") {
                create_app(command[2]);
                cout << lang_pack->create_app_msg << command[2] << endl;
            } else {
                cout << lang_pack->unknown_cmd_msg << command[1] << endl;
            }
        } else if (command[0] == "runserver") {
            run_server();
            cout << lang_pack->runserver_msg << endl;
        } else if (command[0] == "exit") {
            cout << lang_pack->exit_msg << endl;
            return 0;
        } else {
            cout << lang_pack->unknown_cmd_msg;
            for(string word : command) {
                cout << word << " ";
            }
            cout << endl;
        }
    } else {
        cerr << "No command provided." << endl;
        return 1;
    }

    return 0;
}
