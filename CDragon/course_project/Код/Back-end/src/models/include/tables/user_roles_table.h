#ifndef USER_ROLES_TABLE_H
#define USER_ROLES_TABLE_H

#include "base_models/base_table_model.h"
#include "records/user_role_record.h"
#include <stdexcept>
#include <string>
#include <vector>

using std::exception;
using std::string, std::to_string;
using std::vector;

class UserRolesTable final : public BaseTableModel {
public:
  bool insert(int user_id, int role_id) {
    string query = "INSERT INTO user_roles (user_id, role_id) VALUES (?, ?)";
    vector<string> params = {to_string(user_id), to_string(role_id)};

    try {
      db.execute(query, params);
    } catch (const exception &e) {
      throw;
    }

    return true;
  }

  bool remove(int user_id, int role_id) {
    string query = "DELETE FROM user_roles WHERE user_id = ? AND role_id = ?";
    vector<string> params = {to_string(user_id), to_string(role_id)};

    try {
      db.execute(query, params);
    } catch (const exception &e) {
      throw;
    }

    return true;
  }

  vector<UserRoleRecord> getAll() {
    string query = "SELECT user_id, role_id FROM user_roles";

    unique_ptr<sql::ResultSet> result;
    try {
      result = db.query(query);
    } catch (const exception &e) {
      throw;
    }

    vector<UserRoleRecord> all_records;
    while (result->next()) {
      int user_id = result->getInt("user_id");
      int role_id = result->getInt("role_id");

      all_records.emplace_back(user_id, role_id);
    }

    return all_records;
  }

  vector<int> getRoleIdsByUserId(int user_id) {
    string query = "SELECT role_id FROM user_roles WHERE user_id = ?";
    vector<string> params = {to_string(user_id)};

    unique_ptr<sql::ResultSet> result;
    try {
      result = db.query(query, params);
    } catch (const exception &e) {
      throw;
    }

    vector<int> role_ids;
    while (result->next()) {
      role_ids.push_back(result->getInt("role_id"));
    }

    return role_ids;
  }

  vector<int> getUserIdsByRoleId(int role_id) {
    string query = "SELECT user_id FROM user_roles WHERE role_id = ?";
    vector<string> params = {to_string(role_id)};

    unique_ptr<sql::ResultSet> result;
    try {
      result = db.query(query, params);
    } catch (const exception &e) {
      throw;
    }

    vector<int> user_ids;
    while (result->next()) {
      user_ids.push_back(result->getInt("user_id"));
    }

    return user_ids;
  }

  UserRoleRecord getRecordByUserIdAndRoleId(int user_id, int role_id) {
    string query = "SELECT * FROM user_roles WHERE user_id = ? AND role_id = ?";
    vector<string> params = {to_string(user_id), to_string(role_id)};

    unique_ptr<sql::ResultSet> result;
    try {
      result = db.query(query, params);
    } catch (const exception &e) {
      throw;
    }

    if (result->next()) {
      int user_id = result->getInt("user_id");
      int role_id = result->getInt("role_id");

      return UserRoleRecord(user_id, role_id);
    }

    throw std::runtime_error("UserRole not found");
  }
};

#endif // USER_ROLES_TABLE_H