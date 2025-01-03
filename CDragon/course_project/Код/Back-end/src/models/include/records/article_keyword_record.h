#ifndef ARTICLE_KEYWORD_RECORD_H
#define ARTICLE_KEYWORD_RECORD_H

#include "base_models/base_record_model.h"
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>

using std::cout, std::endl;
using std::invalid_argument;
using std::to_string;
using std::unordered_map;

class ArticleKeywordRecord : public BaseRecordModel {
private:
  int article_id;
  int keyword_id;

public:
  ArticleKeywordRecord() {}

  ArticleKeywordRecord(int article_id, int keyword_id)
      : article_id(article_id), keyword_id(keyword_id) {}

  void print() const override {
    cout << "article_id: " << article_id << ", keyword_id: " << keyword_id
         << endl;
  }

  unordered_map<string, string> to_unordered_map() override {
    unordered_map<string, string> data;
    data.insert({"article_id", std::to_string(article_id)});
    data.insert({"keyword_id", std::to_string(keyword_id)});
    return data;
  }

  // Getters
  int getArticleId() const { return article_id; }
  int getKeyWordId() const { return keyword_id; }

  // Setters
  void setArticleId(int article_id) {
    if (article_id <= 0) {
      throw invalid_argument("article_id must be positive");
    }
    this->article_id = article_id;
  }

  void setKeywordId(int keyword_id) {
    if (keyword_id <= 0) {
      throw invalid_argument("keyword_id must be positive");
    }
    this->keyword_id = keyword_id;
  }
};

#endif // ARTICLE_KEYWORD_RECORD_H