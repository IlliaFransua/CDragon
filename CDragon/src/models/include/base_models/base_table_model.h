#ifndef BASE_TABLE_MODEL_H
#define BASE_TABLE_MODEL_H

#include "database.h"
#include <string>

using std::string;

class BaseTableModel {
protected:
  Database db;

  BaseTableModel(const string &host = "tcp://127.0.0.1:3306",
                 const string &user = "root",
                 const string &password = "16003989",
                 const string &dbname = "SRF") {
    db.connect(host, user, password, dbname);
  }

public:
  virtual ~BaseTableModel() = default;
};

#endif // BASE_TABLE_MODEL_H