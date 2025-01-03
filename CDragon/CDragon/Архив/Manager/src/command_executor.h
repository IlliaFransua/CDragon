#ifndef COMMAND_EXECUTOR_H
#define COMMAND_EXECUTOR_H

#include <string>

using namespace std;

void create_project(const string& project_name);
void create_app(const string& app_name);
void run_server();

#endif // COMMAND_EXECUTOR_H
