#ifndef USER_RECORD_H
#define USER_RECORD_H

#include "base_models/base_record_model.h"
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>

using std::cout, std::endl;
using std::invalid_argument;
using std::to_string;
using std::unordered_map;

class UserRecord : public BaseRecordModel {
private:
  int user_id;
  string username;
  string email;
  string phone;
  string password_hash;
  string created_at;

public:
  UserRecord() {}

  UserRecord(int user_id, const string &username, const string &email,
             const string &phone, const string &password_hash,
             const string &created_at)
      : user_id(user_id), username(username), email(email), phone(phone),
        password_hash(password_hash), created_at(created_at) {}

  UserRecord(const UserRecord &other)
      : user_id(other.user_id), username(other.username), email(other.email),
        phone(other.phone), password_hash(other.password_hash),
        created_at(other.created_at) {}

  void print() const override {
    cout << "user_id: " << user_id << ", username: " << username
         << ", email: " << email << ", phone: " << phone
         << ", created_at: " << created_at << endl;
  }

  unordered_map<string, string> to_unordered_map() override {
    unordered_map<string, string> data;
    data.insert({"user_id", to_string(user_id)});
    data.insert({"username", username});
    data.insert({"email", email});
    data.insert({"phone", phone});
    data.insert({"password_hash", password_hash});
    data.insert({"created_at", created_at});
    return data;
  }

  // Getters
  int getUserId() const { return user_id; }
  string getUsername() const { return username; }
  string getEmail() const { return email; }
  string getPhone() const { return phone; }
  string getPasswordHash() const { return password_hash; }
  string getCreatedAt() const { return created_at; }

  // Setters
  void setUserId(int user_id) {
    if (user_id <= 0) {
      throw invalid_argument("user_id must be positive");
    }
    this->user_id = user_id;
  }

  void setUsername(const string &username) {
    if (username.empty()) {
      throw invalid_argument("username cannot be empty");
    }
    this->username = username;
  }

  void setEmail(const string &email) {
    if (email.empty()) {
      throw invalid_argument("email cannot be empty");
    }
    this->email = email;
  }

  void setPhone(const string &phone) {
    if (phone.empty()) {
      throw invalid_argument("phone cannot be empty");
    }
    this->phone = phone;
  }

  void setPasswordHash(const string &password_hash) {
    if (password_hash.empty()) {
      throw invalid_argument("password_hash cannot be empty");
    }
    this->password_hash = password_hash;
  }

  void setCreatedAt(const string &created_at) {
    if (created_at.empty()) {
      throw invalid_argument("created_at cannot be empty");
    }
    this->created_at = created_at;
  }
};

#endif // USER_RECORD_H