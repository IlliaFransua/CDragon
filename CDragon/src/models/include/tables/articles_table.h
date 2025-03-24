#ifndef ARTICLES_TABLE_H
#define ARTICLES_TABLE_H

#include "base_models/base_table_model.h"
#include "records/article_record.h"
#include <ctime>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

using nlohmann::json;
using std::exception;
using std::string, std::to_string;
using std::unordered_map;
using std::vector;

class ArticlesTable final : public BaseTableModel {
public:
  ArticlesTable() : BaseTableModel() {}

  bool insert(const string &title, const string &summary, const string &content,
              const string &status, const string &release_date,
              const string &slug, const string &seo_title,
              const string &seo_description, const string &preview_image) {
    string query =
        "INSERT INTO articles (title, summary, content, status, release_date,"
        "slug, seo_title, seo_description, preview_image, views) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";

    vector<string> params = {title,         summary, content,   status,
                             release_date,  slug,    seo_title, seo_description,
                             preview_image, "0"};

    try {
      db.execute(query, params);
    } catch (const exception &e) {
      throw;
    }

    return true;
  }

  bool update(const ArticleRecord &article) {
    string query = "UPDATE articles SET title = ?, summary = ?, content = ?, "
                   "status = ?, release_date = ?, slug = ?, "
                   "seo_title = ?, seo_description = ?, preview_image = ?, "
                   "views = ? WHERE article_id = ?";

    vector<string> params = {article.getTitle(),
                             article.getSummary(),
                             article.getContent().dump(),
                             article.getStatus(),
                             article.getReleaseDate(),
                             article.getSlug(),
                             article.getSeoTitle(),
                             article.getSeoDescription(),
                             article.getPreviewImage(),
                             to_string(article.getViews()),
                             to_string(article.getArticleId())};

    try {
      db.execute(query, params);
    } catch (const exception &e) {
      throw;
    }

    return true;
  }

  bool remove(int article_id) {
    string query = "DELETE FROM articles WHERE article_id = ?";
    vector<string> params = {to_string(article_id)};

    try {
      db.execute(query, params);
    } catch (const exception &e) {
      throw;
    }

    return true;
  }

  bool publish(int article_id) {
    string query =
        "UPDATE articles SET status = 'published' WHERE article_id = ?";

    vector<string> params = {to_string(article_id)};

    try {
      db.execute(query, params);
    } catch (const exception &e) {
      throw;
    }

    return true;
  }

  bool archive(int article_id) {
    string query =
        "UPDATE articles SET status = 'archived' WHERE article_id = ?";

    vector<string> params = {to_string(article_id)};

    try {
      db.execute(query, params);
    } catch (const exception &e) {
      throw;
    }

    return true;
  }

  ArticleRecord getLatestPublishedArticle() {
    time_t now = time(0);
    char now_time[20];
    strftime(now_time, sizeof(now_time), "%Y-%m-%d %H:%M:%S", localtime(&now));

    string query = "SELECT * FROM articles WHERE status = 'published' "
                   "AND release_date <= ? ORDER BY release_date DESC LIMIT 1";

    vector<string> params = {now_time};

    unique_ptr<sql::ResultSet> result;

    try {
      result = db.query(query, params);
    } catch (const exception &e) {
      throw;
    }

    if (result->next()) {
      int id = result->getInt("article_id");
      string title = result->getString("title");
      string summary = result->getString("summary");
      string content = result->getString("content");
      string status = result->getString("status");
      string release_date = result->getString("release_date");
      string slug = result->getString("slug");
      string seo_title = result->getString("seo_title");
      string seo_description = result->getString("seo_description");
      string preview_image = result->getString("preview_image");
      int views = result->getInt("views");

      return ArticleRecord(id, title, summary, content, status, release_date,
                           slug, seo_title, seo_description, preview_image,
                           views);
    }

    throw runtime_error("Article not found");
  }

  ArticleRecord getRecordById(int article_id) {
    string query = "SELECT * FROM articles WHERE article_id = ? LIMIT 1;";
    vector<string> params = {to_string(article_id)};

    unique_ptr<sql::ResultSet> result;
    try {
      result = db.query(query, params);
    } catch (const exception &e) {
      throw;
    }

    if (result->next()) {
      return ArticleRecord(
          result->getInt("article_id"), result->getString("title"),
          result->getString("summary"),
          json::parse(string(result->getString("content"))),
          result->getString("status"), result->getString("release_date"),
          result->getString("slug"), result->getString("seo_title"),
          result->getString("seo_description"),
          result->getString("preview_image"), result->getInt("views"));
    }

    throw runtime_error("Article not found");
  }

  ArticleRecord getRecordBySlug(const string &slug) {
    string query = "SELECT * FROM articles WHERE slug = ? LIMIT 1;";
    vector<string> params = {slug};

    unique_ptr<sql::ResultSet> result;
    try {
      result = db.query(query, params);
    } catch (const exception &e) {
      throw;
    }

    if (result->next()) {
      return ArticleRecord(
          result->getInt("article_id"), result->getString("title"),
          result->getString("summary"),
          json::parse(string(result->getString("content"))),
          result->getString("status"), result->getString("release_date"),
          result->getString("slug"), result->getString("seo_title"),
          result->getString("seo_description"),
          result->getString("preview_image"), result->getInt("views"));
    }

    throw runtime_error("Article not found");
  }

  vector<ArticleRecord> getAll() {
    string query = "SELECT * FROM articles";

    unique_ptr<sql::ResultSet> result;
    try {
      result = db.query(query);
    } catch (const exception &e) {
      throw;
    }

    vector<ArticleRecord> articles;
    while (result->next()) {
      articles.emplace_back(
          result->getInt("article_id"), result->getString("title"),
          result->getString("summary"),
          json::parse(string(result->getString("content"))),
          result->getString("status"), result->getString("release_date"),
          result->getString("slug"), result->getString("seo_title"),
          result->getString("seo_description"),
          result->getString("preview_image"), result->getInt("views"));
    }

    return articles;
  }

  vector<ArticleRecord> getAllRecordsByStatus(const string &status) {
    string query = "SELECT * FROM articles WHERE status = ?";
    vector<string> params = {status};

    unique_ptr<sql::ResultSet> result;
    try {
      result = db.query(query, params);
    } catch (const exception &e) {
      throw;
    }

    vector<ArticleRecord> articles;
    while (result->next()) {
      articles.emplace_back(
          result->getInt("article_id"), result->getString("title"),
          result->getString("summary"),
          json::parse(string(result->getString("content"))),
          result->getString("status"), result->getString("release_date"),
          result->getString("slug"), result->getString("seo_title"),
          result->getString("seo_description"),
          result->getString("preview_image"), result->getInt("views"));
    }

    return articles;
  }

  vector<ArticleRecord> getAllRecordsByCategory(const string &category_name) {
    string query = "SELECT articles.* "
                   "FROM articles "
                   "JOIN article_categories ON articles.article_id = "
                   "article_categories.article_id "
                   "JOIN categories ON article_categories.category_id = "
                   "categories.category_id "
                   "WHERE categories.name = ?";

    vector<string> params = {category_name};

    unique_ptr<sql::ResultSet> result;
    try {
      result = db.query(query, params);
    } catch (const exception &e) {
      throw;
    }

    vector<ArticleRecord> articles;
    while (result->next()) {
      articles.emplace_back(
          result->getInt("article_id"), result->getString("title"),
          result->getString("summary"),
          json::parse(string(result->getString("content"))),
          result->getString("status"), result->getString("release_date"),
          result->getString("slug"), result->getString("seo_title"),
          result->getString("seo_description"),
          result->getString("preview_image"), result->getInt("views"));
    }

    return articles;
  }

  bool incrementViews(int article_id) {
    string query = "UPDATE articles SET views = views + 1 WHERE article_id = ?";
    vector<string> params = {to_string(article_id)};

    try {
      db.execute(query, params);
    } catch (const exception &e) {
      throw;
    }

    return true;
  }

  vector<ArticleRecord> searchRecordByTitle(const string &title) {
    string query = "SELECT * FROM articles WHERE title LIKE ?";
    vector<string> params = {"%" + title + "%"};

    unique_ptr<sql::ResultSet> result;
    try {
      result = db.query(query, params);
    } catch (const exception &e) {
      throw;
    }

    vector<ArticleRecord> articles;
    while (result->next()) {
      articles.emplace_back(
          result->getInt("article_id"), result->getString("title"),
          result->getString("summary"),
          json::parse(string(result->getString("content"))),
          result->getString("status"), result->getString("release_date"),
          result->getString("slug"), result->getString("seo_title"),
          result->getString("seo_description"),
          result->getString("preview_image"), result->getInt("views"));
    }

    return articles;
  }

  vector<ArticleRecord> filterArticles(const string &searchTitle,
                                       const string &statusFilter,
                                       const string &releaseDateStart,
                                       const string &releaseDateEnd,
                                       const vector<string> &categoryFilter,
                                       const vector<string> &keywordFilter,
                                       const vector<string> &tagFilter) {
    stringstream query;
    query << "SELECT DISTINCT a.* "
          << "FROM articles a "
          << "LEFT JOIN article_categories ac ON a.article_id = ac.article_id "
          << "LEFT JOIN categories c ON ac.category_id = c.category_id "
          << "LEFT JOIN article_keywords ak ON a.article_id = ak.article_id "
          << "LEFT JOIN keywords k ON ak.keyword_id = k.keyword_id "
          << "LEFT JOIN article_tags at ON a.article_id = at.article_id "
          << "LEFT JOIN tags t ON at.tag_id = t.tag_id "
          << "WHERE 1=1 ";

    vector<string> params;

    if (!searchTitle.empty()) {
      query << " AND a.title LIKE ? ";
      params.emplace_back("%" + searchTitle + "%");
    } else {
      query << " AND (a.title LIKE ? OR ? = '') ";
      params.emplace_back("");
      params.emplace_back("");
    }

    if (!statusFilter.empty()) {
      query << " AND a.status = ? ";
      params.emplace_back(statusFilter);
    } else {
      query << " AND (a.status = ? OR ? = '') ";
      params.emplace_back("");
      params.emplace_back("");
    }

    if (!releaseDateStart.empty()) {
      query << " AND a.release_date >= ? ";
      params.emplace_back(releaseDateStart);
    } else {
      query << " AND (a.release_date >= ? OR ? = '') ";
      params.emplace_back("");
      params.emplace_back("");
    }

    if (!releaseDateEnd.empty()) {
      query << " AND a.release_date <= ? ";
      params.emplace_back(releaseDateEnd);
    } else {
      query << " AND (a.release_date <= ? OR ? = '') ";
      params.emplace_back("");
      params.emplace_back("");
    }

    if (!categoryFilter.empty()) {
      query << " AND c.name IN (" << createPlaceholders(categoryFilter.size())
            << ") ";
      for (const auto &category : categoryFilter) {
        params.emplace_back(category);
      }
    } else {
      query << " AND (c.name IN (" << createPlaceholders(1) << ") OR ? = '') ";
      params.emplace_back("");
      params.emplace_back("");
    }

    if (!keywordFilter.empty()) {
      query << " AND k.keyword IN (" << createPlaceholders(keywordFilter.size())
            << ") ";
      for (const auto &keyword : keywordFilter) {
        params.emplace_back(keyword);
      }
    } else {
      query << " AND (k.keyword IN (" << createPlaceholders(1)
            << ") OR ? = '') ";
      params.emplace_back("");
      params.emplace_back("");
    }

    if (!tagFilter.empty()) {
      query << " AND t.name IN (" << createPlaceholders(tagFilter.size())
            << ") ";
      for (const auto &tag : tagFilter) {
        params.emplace_back(tag);
      }
    } else {
      query << " AND (t.name IN (" << createPlaceholders(1) << ") OR ? = '') ";
      params.emplace_back("");
      params.emplace_back("");
    }

    unique_ptr<sql::ResultSet> result;
    try {
      result = db.query(query.str(), params);
    } catch (const exception &e) {
      throw;
    }

    vector<ArticleRecord> articles;
    while (result->next()) {
      ArticleRecord record;

      record.setArticleId(result->getInt("article_id"));
      record.setTitle(result->getString("title"));
      record.setSummary(result->getString("summary"));
      record.setContent(result->getString("content"));
      record.setStatus(result->getString("status"));
      record.setReleaseDate(result->getString("release_date"));
      record.setSlug(result->getString("slug"));
      record.setSeoTitle(result->getString("seo_title"));
      record.setSeoDescription(result->getString("seo_description"));
      record.setPreviewImage(result->getString("preview_image"));
      record.setViews(result->getInt("views"));

      articles.emplace_back(record);
    }

    return articles;
  }

private:
  string createPlaceholders(size_t count) {
    stringstream placeholders;
    for (size_t i = 0; i < count; ++i) {
      if (i != 0) {
        placeholders << ", ";
      }
      placeholders << "?";
    }
    return placeholders.str();
  }
};

#endif // ARTICLES_TABLE_H