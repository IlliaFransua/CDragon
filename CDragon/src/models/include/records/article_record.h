#ifndef ARTICLE_RECORD_H
#define ARTICLE_RECORD_H

#include "base_models/base_record_model.h"
#include <iostream>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <string>
#include <unordered_map>

using nlohmann::json;
using std::cout, std::endl;
using std::invalid_argument;
using std::string;
using std::unordered_map;

class ArticleRecord : public BaseRecordModel {
private:
  int article_id;
  string title;
  string summary;
  json content;
  string status;
  string release_date;
  string slug;
  string seo_title;
  string seo_description;
  string preview_image;
  int views;

public:
  ArticleRecord() {}

  ArticleRecord(int article_id, const string &title, const string &summary,
                const json &content, const string &status,
                const string &release_date, const string &slug,
                const string &seo_title, const string &seo_description,
                const string &preview_image, int views)
      : article_id(article_id), title(title), summary(summary),
        content(content), status(status), release_date(release_date),
        slug(slug), seo_title(seo_title), seo_description(seo_description),
        preview_image(preview_image), views(views) {}

  void print() const override {
    cout << "article_id: " << article_id << ", title: " << title
         << ", status: " << status << ", views: " << views
         << ", release_date: " << release_date << endl;
  }

  unordered_map<string, string> to_unordered_map() override {
    unordered_map<string, string> data;

    data.insert({"article_id", std::to_string(article_id)});
    data.insert({"title", title});
    data.insert({"summary", summary});
    data.insert({"content", content.dump()}); // to str
    data.insert({"status", status});
    data.insert({"release_date", release_date});
    data.insert({"slug", slug});
    data.insert({"seo_title", seo_title});
    data.insert({"seo_description", seo_description});
    data.insert({"preview_image", preview_image});
    data.insert({"views", std::to_string(views)});

    return data;
  }

  json to_json() {
    return json{{"article_id", article_id},
                {"title", title},
                {"summary", summary},
                {"content", content},
                {"status", status},
                {"release_date", release_date},
                {"slug", slug},
                {"seo_title", seo_title},
                {"seo_description", seo_description},
                {"preview_image", preview_image},
                {"views", views}};
  }

  // Getters
  int getArticleId() const { return article_id; }
  string getTitle() const { return title; }
  string getSummary() const { return summary; }
  json getContent() const { return content; }
  string getStatus() const { return status; }
  string getReleaseDate() const { return release_date; }
  string getSlug() const { return slug; }
  string getSeoTitle() const { return seo_title; }
  string getSeoDescription() const { return seo_description; }
  string getPreviewImage() const { return preview_image; }
  int getViews() const { return views; }

  // Setters
  void setArticleId(int article_id) {
    if (article_id <= 0) {
      throw invalid_argument("article_id must be positive");
    }
    this->article_id = article_id;
  }

  void setTitle(const string &title) {
    if (title.empty()) {
      throw invalid_argument("title cannot be empty");
    }
    this->title = title;
  }

  void setSummary(const string &summary) {
    if (summary.empty()) {
      throw invalid_argument("summary cannot be empty");
    }
    this->summary = summary;
  }

  void setContent(const json &content) { this->content = content; }

  void setStatus(const string &status) {
    if (status != "draft" && status != "published" && status != "archived") {
      throw invalid_argument(
          "status must be 'draft', 'published', or 'archived'");
    }
    this->status = status;
  }

  void setReleaseDate(const string &release_date) {
    this->release_date = release_date;
  }

  void setSlug(const string &slug) {
    if (slug.empty()) {
      throw invalid_argument("slug cannot be empty");
    }
    this->slug = slug;
  }

  void setSeoTitle(const string &seo_title) { this->seo_title = seo_title; }

  void setSeoDescription(const string &seo_description) {
    this->seo_description = seo_description;
  }

  void setPreviewImage(const string &preview_image) {
    this->preview_image = preview_image;
  }

  void setViews(int views) {
    if (views < 0) {
      throw invalid_argument("views cannot be negative");
    }
    this->views = views;
  }
};

#endif // ARTICLE_RECORD_H