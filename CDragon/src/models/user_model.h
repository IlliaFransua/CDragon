#ifndef USERMODEL_H
#define USERMODEL_H

#include "base_model.h"
#include <unordered_map>
#include <string>

using std::unordered_map, std::string;

class UserModel : public BaseModel {
private:
    int id;
    string name;
    string email;

public:
    UserModel(int id, const string& name, const string& email)
        : id(id), name(name), email(email) {}
    UserModel() {}
    ~UserModel() {}

    virtual void saveRecord() override {
    
    }

    virtual void deleteRecord() override {

    }

    virtual unordered_map<string, string> to_unordered_map() override {
        unordered_map<string, string> a;
        return a;
    }

    virtual void display() override {

    }

};

#endif