#ifndef KEYWORDS_TABLE_H
#define KEYWORDS_TABLE_H

#include "base_models/base_table_model.h"
#include "records/keyword_record.h"
#include <stdexcept>
#include <string>
#include <vector>

using std::exception;
using std::string, std::to_string;
using std::vector;

class KeywordsTable final : public BaseTableModel {
public:
  bool insert(const string &keyword) {
    string query = "INSERT INTO keywords (keyword) VALUES (?)";
    vector<string> params = {keyword};

    try {
      db.execute(query, params);
    } catch (const exception &e) {
      throw;
    }

    return true;
  }

  bool remove(int keyword_id) {
    string query = "DELETE FROM keywords WHERE keyword_id = ?";
    vector<string> params = {to_string(keyword_id)};

    try {
      db.execute(query, params);
    } catch (const exception &e) {
      throw;
    }

    return true;
  }

  bool update(int keyword_id, const string &new_keyword) {
    string query = "UPDATE keywords SET keyword = ? WHERE keyword_id = ?";
    vector<string> params = {new_keyword, to_string(keyword_id)};

    try {
      db.execute(query, params);
    } catch (const exception &e) {
      throw;
    }

    return true;
  }

  KeywordRecord getRecordById(int keyword_id) {
    string query =
        "SELECT keyword_id, keyword FROM keywords WHERE keyword_id = ?";
    vector<string> params = {to_string(keyword_id)};

    unique_ptr<sql::ResultSet> result;
    try {
      result = db.query(query, params);
    } catch (const exception &e) {
      throw;
    }

    if (result->next()) {
      int id = result->getInt("keyword_id");
      string keyword = result->getString("keyword");
      return KeywordRecord(id, keyword);
    }

    throw std::runtime_error("Keyword not found");
  }

  vector<KeywordRecord> getAll() {
    string query = "SELECT keyword_id, keyword FROM keywords ORDER BY keyword";

    unique_ptr<sql::ResultSet> result;
    try {
      result = db.query(query);
    } catch (const exception &e) {
      throw;
    }

    vector<KeywordRecord> all_records;
    while (result->next()) {
      int id = result->getInt("keyword_id");
      string keyword = result->getString("keyword");

      all_records.emplace_back(id, keyword);
    }

    return all_records;
  }

  vector<KeywordRecord> searchByKeyword(const string &keyword) {
    string query =
        "SELECT keyword_id, keyword FROM keywords WHERE keyword LIKE ?";
    vector<string> params = {"%" + keyword + "%"};

    unique_ptr<sql::ResultSet> result;
    try {
      result = db.query(query, params);
    } catch (const exception &e) {
      throw;
    }

    vector<KeywordRecord> matched_keywords;
    while (result->next()) {
      int id = result->getInt("keyword_id");
      string keyword = result->getString("keyword");

      matched_keywords.emplace_back(id, keyword);
    }

    return matched_keywords;
  }
};

#endif // KEYWORDS_TABLE_H