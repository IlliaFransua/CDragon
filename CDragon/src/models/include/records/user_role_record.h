#ifndef USER_ROLE_RECORD_H
#define USER_ROLE_RECORD_H

#include "base_models/base_record_model.h"
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>

using std::cout, std::endl;
using std::invalid_argument;
using std::to_string;
using std::unordered_map;

class UserRoleRecord : public BaseRecordModel {
private:
  int user_id;
  int role_id;

public:
  UserRoleRecord() {}

  UserRoleRecord(int user_id, int role_id)
      : user_id(user_id), role_id(role_id) {}

  void print() const override {
    cout << "user_id: " << user_id << ", role_id: " << role_id << endl;
  }

  unordered_map<string, string> to_unordered_map() override {
    unordered_map<string, string> data;
    data.insert({"user_id", to_string(user_id)});
    data.insert({"role_id", to_string(role_id)});
    return data;
  }

  // Getters
  int getUserId() const { return user_id; }
  int getRoleId() const { return role_id; }

  // Setters
  void setUserId(int user_id) {
    if (user_id <= 0) {
      throw invalid_argument("user_id must be positive");
    }
    this->user_id = user_id;
  }

  void setRoleId(int role_id) {
    if (role_id <= 0) {
      throw invalid_argument("role_id must be positive");
    }
    this->role_id = role_id;
  }
};

#endif // USER_ROLE_RECORD_H