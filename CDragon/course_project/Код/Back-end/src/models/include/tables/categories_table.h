#ifndef CATEGORY_TABLE_H
#define CATEGORY_TABLE_H

#include "base_models/base_table_model.h"
#include "records/category_record.h"
#include <stdexcept>
#include <string>
#include <vector>

using std::exception;
using std::string, std::to_string;
using std::vector;

class CategoriesTable final : public BaseTableModel {
public:
  bool insert(const string &name, const string &description) {
    string query = "INSERT INTO categories (name, description) VALUES (?, ?)";
    vector<string> params = {name, description};

    try {
      db.execute(query, params);
    } catch (const exception &e) {
      throw;
    }

    return true;
  }

  bool remove(int category_id) {
    string query = "DELETE FROM categories WHERE category_id = ?";
    vector<string> params = {to_string(category_id)};

    try {
      db.execute(query, params);
    } catch (const exception &e) {
      throw;
    }

    return true;
  }

  bool update(int category_id, const string &name, const string &description) {
    string query =
        "UPDATE categories SET name = ?, description = ? WHERE category_id = ?";
    vector<string> params = {name, description, to_string(category_id)};

    try {
      db.execute(query, params);
    } catch (const exception &e) {
      throw;
    }

    return true;
  }

  CategoryRecord getRecordById(int category_id) {
    string query = "SELECT category_id, name, description FROM categories "
                   "WHERE category_id = ?";
    vector<string> params = {to_string(category_id)};

    unique_ptr<sql::ResultSet> result;
    try {
      result = db.query(query, params);
    } catch (const exception &e) {
      throw;
    }

    if (result->next()) {
      int id = result->getInt("category_id");
      string name = result->getString("name");
      string description = result->getString("description");
      return CategoryRecord(id, name, description);
    }

    throw std::runtime_error("Category not found");
  }

  vector<CategoryRecord> getAll() {
    string query =
        "SELECT category_id, name, description FROM categories ORDER BY name";

    unique_ptr<sql::ResultSet> result;
    try {
      result = db.query(query);
    } catch (const exception &e) {
      throw;
    }

    vector<CategoryRecord> all_records;
    while (result->next()) {
      int id = result->getInt("category_id");
      string name = result->getString("name");
      string description = result->getString("description");

      all_records.emplace_back(id, name, description);
    }

    return all_records;
  }

  vector<CategoryRecord> searchByName(const string &name) {
    string query = "SELECT category_id, name, description FROM categories "
                   "WHERE name LIKE ?";
    vector<string> params = {"%" + name + "%"};

    unique_ptr<sql::ResultSet> result;
    try {
      result = db.query(query, params);
    } catch (const exception &e) {
      throw;
    }

    vector<CategoryRecord> matched_categories;
    while (result->next()) {
      int id = result->getInt("category_id");
      string name = result->getString("name");
      string description = result->getString("description");

      matched_categories.emplace_back(id, name, description);
    }

    return matched_categories;
  }
};

#endif // CATEGORY_TABLE_H