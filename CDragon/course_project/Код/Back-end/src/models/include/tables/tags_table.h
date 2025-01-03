#ifndef TAG_TABLE_H
#define TAG_TABLE_H

#include "base_models/base_table_model.h"
#include "records/tag_record.h"
#include <stdexcept>
#include <string>
#include <vector>

using std::exception;
using std::string, std::to_string;
using std::vector;

class TagsTable final : public BaseTableModel {
public:
  bool insert(const string &name) {
    string query = "INSERT INTO tags (name) VALUES (?)";
    vector<string> params = {name};

    try {
      db.execute(query, params);
    } catch (const exception &e) {
      throw;
    }

    return true;
  }

  bool remove(int tag_id) {
    string query = "DELETE FROM tags WHERE tag_id = ?";
    vector<string> params = {to_string(tag_id)};

    try {
      db.execute(query, params);
    } catch (const exception &e) {
      throw;
    }

    return true;
  }

  bool update(int tag_id, const string &name) {
    string query = "UPDATE tags SET name = ? WHERE tag_id = ?";
    vector<string> params = {name, to_string(tag_id)};

    try {
      db.execute(query, params);
    } catch (const exception &e) {
      throw;
    }

    return true;
  }

  TagRecord getRecordById(int tag_id) {
    string query = "SELECT * FROM tags WHERE tag_id = ?";
    vector<string> params = {to_string(tag_id)};

    unique_ptr<sql::ResultSet> result;
    try {
      result = db.query(query, params);
    } catch (const exception &e) {
      throw;
    }

    if (result->next()) {
      int id = result->getInt("tag_id");
      string name = result->getString("name");

      return TagRecord(id, name);
    }

    throw std::runtime_error("Tag not found");
  }

  vector<TagRecord> getAll() {
    string query = "SELECT * FROM tags ORDER BY name";

    unique_ptr<sql::ResultSet> result;
    try {
      result = db.query(query);
    } catch (const exception &e) {
      throw;
    }

    vector<TagRecord> all_records;
    while (result->next()) {
      int id = result->getInt("tag_id");
      string name = result->getString("name");

      all_records.emplace_back(id, name);
    }

    return all_records;
  }

  vector<TagRecord> searchByName(const string &name) {
    string query = "SELECT * FROM tags WHERE name LIKE ?";
    vector<string> params = {"%" + name + "%"};

    unique_ptr<sql::ResultSet> result;
    try {
      result = db.query(query, params);
    } catch (const exception &e) {
      throw;
    }

    vector<TagRecord> matched_tags;
    while (result->next()) {
      int id = result->getInt("tag_id");
      string tag_name = result->getString("name");

      matched_tags.emplace_back(id, tag_name);
    }

    return matched_tags;
  }
};

#endif // TAG_TABLE_H