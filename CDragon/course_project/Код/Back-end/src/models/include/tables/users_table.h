#ifndef USERS_TABLE_H
#define USERS_TABLE_H

#include "base_models/base_table_model.h"
#include "records/user_record.h"
#include <stdexcept>
#include <string>
#include <vector>

using std::exception;
using std::runtime_error;
using std::string, std::to_string;
using std::vector;

class UsersTable : public BaseTableModel {
public:
  bool insert(const string &username, const string &email, const string &phone,
              const string &password_hash) {
    string query = "INSERT INTO users (username, email, phone, password_hash) "
                   "VALUES (?, ?, ?, ?)";
    vector<string> params = {username, email, phone, password_hash};

    try {
      db.execute(query, params);
    } catch (const exception &e) {
      throw;
    }

    return true;
  }

  bool remove(int user_id) {
    string query = "DELETE FROM users WHERE user_id = ?";
    vector<string> params = {to_string(user_id)};

    try {
      db.execute(query, params);
    } catch (const exception &e) {
      throw;
    }

    return true;
  }

  bool update(int user_id, const string &username, const string &email,
              const string &phone, const string &password_hash) {
    string query = "UPDATE users SET username = ?, email = ?, phone = ?, "
                   "password_hash = ? WHERE user_id = ?";
    vector<string> params = {username, email, phone, password_hash,
                             to_string(user_id)};

    try {
      db.execute(query, params);
    } catch (const exception &e) {
      throw;
    }

    return true;
  }

  vector<UserRecord> getAll() {
    string query = "SELECT user_id, username, email, phone, password_hash, "
                   "created_at FROM users";

    unique_ptr<sql::ResultSet> result;
    try {
      result = db.query(query);
    } catch (const exception &e) {
      throw;
    }

    vector<UserRecord> all_records;
    while (result->next()) {
      int user_id = result->getInt("user_id");
      string username = result->getString("username");
      string email = result->getString("email");
      string phone = result->getString("phone");
      string password_hash = result->getString("password_hash");
      string created_at = result->getString("created_at");

      all_records.emplace_back(user_id, username, email, phone, password_hash,
                               created_at);
    }

    return all_records;
  }

  UserRecord getRecordById(int user_id) {
    string query = "SELECT * FROM users WHERE user_id = ? LIMIT 1";
    vector<string> params = {to_string(user_id)};

    unique_ptr<sql::ResultSet> result;
    try {
      result = db.query(query, params);
    } catch (const exception &e) {
      throw;
    }

    if (result->next()) {
      string username = result->getString("username");
      string email = result->getString("email");
      string phone = result->getString("phone");
      string password_hash = result->getString("password_hash");
      string created_at = result->getString("created_at");

      return UserRecord(user_id, username, email, phone, password_hash,
                        created_at);
    }

    throw runtime_error("User not found");
  }

  UserRecord getRecordByUsername(const string &username) {
    string query = "SELECT * FROM users WHERE username = ? LIMIT 1";
    vector<string> params = {username};

    unique_ptr<sql::ResultSet> result;
    try {
      result = db.query(query, params);
    } catch (const exception &e) {
      throw;
    }

    if (result->next()) {
      int user_id = result->getInt("user_id");
      string email = result->getString("email");
      string phone = result->getString("phone");
      string password_hash = result->getString("password_hash");
      string created_at = result->getString("created_at");

      return UserRecord(user_id, username, email, phone, password_hash,
                        created_at);
    }

    throw runtime_error("User with the given username not found");
  }

  UserRecord getRecordByUsernameAndPassword(const string &username,
                                            const string &password) {
    string query = "SELECT * FROM users WHERE username = ? LIMIT 1";
    vector<string> params = {username};

    unique_ptr<sql::ResultSet> result;
    try {
      result = db.query(query, params);
    } catch (const exception &e) {
      throw;
    }

    if (result->next()) {
      int user_id = result->getInt("user_id");
      string email = result->getString("email");
      string phone = result->getString("phone");
      string password_hash = result->getString("password_hash");
      string created_at = result->getString("created_at");

      if (password == password_hash) {
        return UserRecord(user_id, username, email, phone, password_hash,
                          created_at);
      } else {
        throw runtime_error("Incorrect password");
      }
    }

    throw runtime_error("User with the given username not found");
  }

  UserRecord getRecordByEmail(const string &email) {
    string query = "SELECT * FROM users WHERE email = ? LIMIT 1";
    vector<string> params = {email};

    unique_ptr<sql::ResultSet> result;
    try {
      result = db.query(query, params);
    } catch (const exception &e) {
      throw;
    }

    if (result->next()) {
      int user_id = result->getInt("user_id");
      string username = result->getString("username");
      string phone = result->getString("phone");
      string password_hash = result->getString("password_hash");
      string created_at = result->getString("created_at");

      return UserRecord(user_id, username, email, phone, password_hash,
                        created_at);
    }

    throw runtime_error("User with the given email not found");
  }

  UserRecord getRecordByPhone(const string &phone) {
    string query = "SELECT * FROM users WHERE phone = ? LIMIT 1";
    vector<string> params = {phone};

    unique_ptr<sql::ResultSet> result;
    try {
      result = db.query(query, params);
    } catch (const exception &e) {
      throw;
    }

    if (result->next()) {
      int user_id = result->getInt("user_id");
      string username = result->getString("username");
      string email = result->getString("email");
      string password_hash = result->getString("password_hash");
      string created_at = result->getString("created_at");

      return UserRecord(user_id, username, email, phone, password_hash,
                        created_at);
    }

    throw runtime_error("User with this phone not found");
  }
};

#endif // USERS_TABLE_H