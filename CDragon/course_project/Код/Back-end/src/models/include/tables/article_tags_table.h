#ifndef ARTICLE_TAGS_TABLE_H
#define ARTICLE_TAGS_TABLE_H

#include "base_models/base_table_model.h"
#include "records/article_tag_record.h"
#include <stdexcept>
#include <string>
#include <vector>

using std::exception;
using std::string, std::to_string;
using std::vector;

class ArticleTagsTable final : public BaseTableModel {
public:
  ArticleTagsTable() : BaseTableModel() {}

  bool insert(int article_id, int tag_id) {
    string query =
        "INSERT INTO article_tags (article_id, tag_id) VALUES (?, ?)";
    vector<string> params = {to_string(article_id), to_string(tag_id)};

    try {
      db.execute(query, params);
    } catch (const exception &e) {
      throw;
    }

    return true;
  }

  bool remove(int article_id, int tag_id) {
    string query =
        "DELETE FROM article_tags WHERE article_id = ? AND tag_id = ?";
    vector<string> params = {to_string(article_id), to_string(tag_id)};

    try {
      db.execute(query, params);
    } catch (const exception &e) {
      throw;
    }

    return true;
  }

  vector<int> getTagIdsForArticle(int article_id) {
    string query = "SELECT tag_id FROM article_tags WHERE article_id = ?";
    vector<string> params = {to_string(article_id)};

    unique_ptr<sql::ResultSet> result;
    try {
      result = db.query(query, params);
    } catch (const exception &e) {
      throw;
    }

    vector<int> tag_ids;
    while (result->next()) {
      tag_ids.push_back(result->getInt("tag_id"));
    }

    return tag_ids;
  }

  vector<int> getArticleIdsForTag(int tag_id) {
    string query = "SELECT article_id FROM article_tags WHERE tag_id = ?";
    vector<string> params = {to_string(tag_id)};

    unique_ptr<sql::ResultSet> result;
    try {
      result = db.query(query, params);
    } catch (const exception &e) {
      throw;
    }

    vector<int> article_ids;
    while (result->next()) {
      article_ids.emplace_back(result->getInt("article_id"));
    }

    return article_ids;
  }

  vector<ArticleTagRecord> getAll() {
    string query = "SELECT article_id, tag_id FROM article_tags";

    unique_ptr<sql::ResultSet> result;
    try {
      result = db.query(query);
    } catch (const exception &e) {
      throw;
    }

    vector<ArticleTagRecord> all_records;
    while (result->next()) {
      int article_id = result->getInt("article_id");
      int tag_id = result->getInt("tag_id");

      all_records.emplace_back(article_id, tag_id);
    }

    return all_records;
  }
};

#endif // ARTICLE_TAGS_TABLE_H