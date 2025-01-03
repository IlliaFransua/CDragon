#ifndef ROLES_TABLE_H
#define ROLES_TABLE_H

#include "base_models/base_table_model.h"
#include "records/role_record.h"
#include <stdexcept>
#include <string>
#include <vector>

using std::exception;
using std::string, std::to_string;
using std::vector;

class RolesTable final : public BaseTableModel {
public:
  bool insert(const string &name, bool can_create_articles = false,
              bool can_edit_articles = false, bool can_delete_articles = false,
              bool can_archive_articles = false,
              bool can_restore_articles = false) {
    string query = R"(
            INSERT INTO roles (
                name, 
                can_create_articles, 
                can_edit_articles, 
                can_delete_articles, 
                can_archive_articles, 
                can_restore_articles
            ) VALUES (?, ?, ?, ?, ?, ?))";

    vector<string> params = {name,
                             to_string(can_create_articles),
                             to_string(can_edit_articles),
                             to_string(can_delete_articles),
                             to_string(can_archive_articles),
                             to_string(can_restore_articles)};

    try {
      db.execute(query, params);
    } catch (const exception &e) {
      throw;
    }

    return true;
  }

  bool remove(int role_id) {
    string query = "DELETE FROM roles WHERE role_id = ?";
    vector<string> params = {to_string(role_id)};

    try {
      db.execute(query, params);
    } catch (const exception &e) {
      throw;
    }

    return true;
  }

  bool update(int role_id, const string &name, bool can_create_articles,
              bool can_edit_articles, bool can_delete_articles,
              bool can_archive_articles, bool can_restore_articles) {
    string query = R"(
            UPDATE roles 
            SET name = ?, 
                can_create_articles = ?, 
                can_edit_articles = ?, 
                can_delete_articles = ?, 
                can_archive_articles = ?, 
                can_restore_articles = ? 
            WHERE role_id = ?)";

    vector<string> params = {name,
                             to_string(can_create_articles),
                             to_string(can_edit_articles),
                             to_string(can_delete_articles),
                             to_string(can_archive_articles),
                             to_string(can_restore_articles),
                             to_string(role_id)};

    try {
      db.execute(query, params);
    } catch (const exception &e) {
      throw;
    }

    return true;
  }

  RoleRecord getRecordById(int role_id) {
    string query = "SELECT * FROM roles WHERE role_id = ?";
    vector<string> params = {to_string(role_id)};

    unique_ptr<sql::ResultSet> result;
    try {
      result = db.query(query, params);
    } catch (const exception &e) {
      throw;
    }

    if (result->next()) {
      return RoleRecord(result->getInt("role_id"), result->getString("name"),
                        result->getBoolean("can_create_articles"),
                        result->getBoolean("can_edit_articles"),
                        result->getBoolean("can_delete_articles"),
                        result->getBoolean("can_archive_articles"),
                        result->getBoolean("can_restore_articles"));
    }

    throw std::runtime_error("Role not found");
  }

  RoleRecord getRecordByName(const string &name) {
    string query = "SELECT * FROM roles WHERE name = ?";
    vector<string> params = {name};

    unique_ptr<sql::ResultSet> result;
    try {
      result = db.query(query, params);
    } catch (const exception &e) {
      throw;
    }

    if (result->next()) {
      return RoleRecord(result->getInt("role_id"), result->getString("name"),
                        result->getBoolean("can_create_articles"),
                        result->getBoolean("can_edit_articles"),
                        result->getBoolean("can_delete_articles"),
                        result->getBoolean("can_archive_articles"),
                        result->getBoolean("can_restore_articles"));
    }

    throw std::runtime_error("Role not found");
  }

  vector<RoleRecord> getAll() {
    string query = "SELECT * FROM roles";

    unique_ptr<sql::ResultSet> result;
    try {
      result = db.query(query);
    } catch (const exception &e) {
      throw;
    }

    vector<RoleRecord> all_records;
    while (result->next()) {
      all_records.emplace_back(result->getInt("role_id"),
                               result->getString("name"),
                               result->getBoolean("can_create_articles"),
                               result->getBoolean("can_edit_articles"),
                               result->getBoolean("can_delete_articles"),
                               result->getBoolean("can_archive_articles"),
                               result->getBoolean("can_restore_articles"));
    }

    return all_records;
  }

  vector<RoleRecord> searchByName(const string &name) {
    string query = "SELECT * FROM roles WHERE name LIKE ?";
    vector<string> params = {"%" + name + "%"};

    unique_ptr<sql::ResultSet> result;
    try {
      result = db.query(query, params);
    } catch (const exception &e) {
      throw;
    }

    vector<RoleRecord> matched_roles;
    while (result->next()) {
      matched_roles.emplace_back(result->getInt("role_id"),
                                 result->getString("name"),
                                 result->getBoolean("can_create_articles"),
                                 result->getBoolean("can_edit_articles"),
                                 result->getBoolean("can_delete_articles"),
                                 result->getBoolean("can_archive_articles"),
                                 result->getBoolean("can_restore_articles"));
    }

    return matched_roles;
  }
};

#endif // ROLES_TABLE_H