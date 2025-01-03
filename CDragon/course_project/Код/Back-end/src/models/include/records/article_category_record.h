#ifndef ARTICLE_CATEGORY_RECORD_H
#define ARTICLE_CATEGORY_RECORD_H

#include "base_models/base_record_model.h"
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>

using std::cout, std::endl;
using std::invalid_argument;
using std::to_string;
using std::unordered_map;

class ArticleCategoryRecord : public BaseRecordModel {
private:
  int article_id;
  int category_id;

public:
  ArticleCategoryRecord() {}

  ArticleCategoryRecord(int article_id, int category_id)
      : article_id(article_id), category_id(category_id) {}

  void print() const override {
    cout << "article_id: " << article_id << ", category_id: " << category_id
         << endl;
  }

  unordered_map<string, string> to_unordered_map() override {
    unordered_map<string, string> data;
    data.insert({"article_id", std::to_string(article_id)});
    data.insert({"category_id", std::to_string(category_id)});
    return data;
  }

  // Getters
  int getArticleId() const { return article_id; }
  int getCategoryId() const { return category_id; }

  // Setters
  void setArticleId(int article_id) {
    if (article_id <= 0) {
      throw invalid_argument("article_id must be positive");
    }
    this->article_id = article_id;
  }

  void setCategoryId(int category_id) {
    if (category_id <= 0) {
      throw invalid_argument("category_id must be positive");
    }
    this->category_id = category_id;
  }
};

#endif // ARTICLE_CATEGORY_RECORD_H