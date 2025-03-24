#ifndef BASEMODEL_H
#define BASEMODEL_H

#include <database.h>
#include <unordered_map>
#include <string>

using std::string;
using std::unordered_map;

class BaseModel {
protected:
    Database db;

public:
    BaseModel() {
        string host = "tcp://127.0.0.1:3306";
        string user = "root";
        string password = "1234";
        string dbname = "CDragon";
        db.connect(host, user, password, dbname);
    }
    virtual ~BaseModel() = default;
    
    virtual void saveRecord() = 0;
    virtual void deleteRecord() = 0;

    virtual unordered_map<string, string> to_unordered_map() = 0;
    virtual void display() = 0;
};

#endif // BASEMODEL_H