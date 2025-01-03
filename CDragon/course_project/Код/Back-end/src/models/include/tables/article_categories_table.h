#ifndef ARTICLE_CATEGORIES_TABLE_H
#define ARTICLE_CATEGORIES_TABLE_H

#include "base_models/base_table_model.h"
#include "records/article_category_record.h"
#include <stdexcept>
#include <string>
#include <vector>

using std::exception;
using std::string, std::to_string;
using std::vector;

class ArticleCategoriesTable final : public BaseTableModel {
public:
  ArticleCategoriesTable() : BaseTableModel() {}

  bool insert(int article_id, int category_id) {
    string query = "INSERT INTO article_categories (article_id, category_id) "
                   "VALUES (?, ?)";
    vector<string> params = {to_string(article_id), to_string(category_id)};

    try {
      db.execute(query, params);
    } catch (const exception &e) {
      throw;
    }

    return true;
  }

  bool remove(int article_id, int category_id) {
    string query = "DELETE FROM article_categories WHERE article_id = ? AND "
                   "category_id = ?";
    vector<string> params = {to_string(article_id), to_string(category_id)};

    try {
      db.execute(query, params);
    } catch (const exception &e) {
      throw;
    }

    return true;
  }

  vector<int> getCategoryIdsForArticle(int article_id) {
    string query =
        "SELECT category_id FROM article_categories WHERE article_id = ?";
    vector<string> params = {to_string(article_id)};

    unique_ptr<sql::ResultSet> result;
    try {
      result = db.query(query, params);
    } catch (const exception &e) {
      throw;
    }

    vector<int> category_ids;
    while (result->next()) {
      category_ids.push_back(result->getInt("category_id"));
    }

    return category_ids;
  }

  vector<int> getArticleIdsForCategory(int category_id) {
    string query =
        "SELECT article_id FROM article_categories WHERE category_id = ?";
    vector<string> params = {to_string(category_id)};

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

  vector<ArticleCategoryRecord> getAll() {
    string query = "SELECT article_id, category_id FROM article_categories";

    unique_ptr<sql::ResultSet> result;
    try {
      result = db.query(query);
    } catch (const exception &e) {
      throw;
    }

    vector<ArticleCategoryRecord> all_records;
    while (result->next()) {
      int article_id = result->getInt("article_id");
      int category_id = result->getInt("category_id");

      all_records.emplace_back(article_id, category_id);
    }

    return all_records;
  }
};

#endif // ARTICLE_CATEGORIES_TABLE_H