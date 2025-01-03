#ifndef LANGUAGE_PACK_H
#define LANGUAGE_PACK_H

#include <string>

using namespace std;

struct LanguagePack {
    string create_project_msg;
    string create_app_msg;
    string runserver_msg;
    string unknown_cmd_msg;
    string exit_msg;
};

extern LanguagePack lang_en;
extern LanguagePack lang_uk;
extern LanguagePack lang_ru;
extern LanguagePack lang_de;

#endif // LANGUAGE_PACK_H
