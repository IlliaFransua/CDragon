#ifndef ARTICLE_KEYWORDS_TABLE_H
#define ARTICLE_KEYWORDS_TABLE_H

#include "base_models/base_table_model.h"
#include "records/article_keyword_record.h"
#include <stdexcept>
#include <string>
#include <vector>

using std::exception;
using std::string, std::to_string;
using std::vector;

class ArticleKeywordsTable final : public BaseTableModel {
public:
  ArticleKeywordsTable() : BaseTableModel() {}

  bool insert(int article_id, int keyword_id) {
    string query =
        "INSERT INTO article_keywords (article_id, keyword_id) VALUES (?, ?)";
    vector<string> params = {to_string(article_id), to_string(keyword_id)};

    try {
      db.execute(query, params);
    } catch (const std::exception &e) {
      throw;
    }

    return true;
  }

  bool remove(int article_id, int keyword_id) {
    string query =
        "DELETE FROM article_keywords WHERE article_id = ? AND keyword_id = ?";
    vector<string> params = {to_string(article_id), to_string(keyword_id)};

    try {
      db.execute(query, params);
    } catch (const std::exception &e) {
      throw;
    }

    return true;
  }

  vector<int> getKeywordIdsForArticle(int article_id) {
    string query =
        "SELECT keyword_id FROM article_keywords WHERE article_id = ?";
    vector<string> params = {to_string(article_id)};

    unique_ptr<sql::ResultSet> result;
    try {
      result = db.query(query, params);
    } catch (const exception &e) {
      throw;
    }

    vector<int> keyword_ids;
    while (result->next()) {
      keyword_ids.emplace_back(result->getInt("keyword_id"));
    }

    return keyword_ids;
  }

  vector<int> getArticleIdsForKeyword(int keyword_id) {
    string query =
        "SELECT article_id FROM article_keywords WHERE keyword_id = ?";
    vector<string> params = {to_string(keyword_id)};

    unique_ptr<sql::ResultSet> result;
    try {
      result = db.query(query, params);
    } catch (const exception &e) {
      throw;
    }

    vector<int> article_ids;
    while (result->next()) {
      article_ids.emplace_back(result->getInt("keyword_id"));
    }

    return article_ids;
  }

  vector<ArticleKeywordRecord> getAll() {
    string query = "SELECT article_id, keyword_id FROM article_keywords";

    unique_ptr<sql::ResultSet> result;
    try {
      result = db.query(query);
    } catch (const exception &e) {
      throw;
    }

    vector<ArticleKeywordRecord> all_records;
    while (result->next()) {
      int article_id = result->getInt("article_id");
      int keyword_id = result->getInt("keyword_id");

      all_records.emplace_back(article_id, keyword_id);
    }

    return all_records;
  }
};

#endif // ARTICLE_KEYWORDS_TABLE_H