#ifndef ARTICLE_TAG_RECORD_H
#define ARTICLE_TAG_RECORD_H

#include "base_models/base_record_model.h"
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>

using std::cout, std::endl;
using std::invalid_argument;
using std::to_string;
using std::unordered_map;

class ArticleTagRecord : public BaseRecordModel {
private:
  int article_id;
  int tag_id;

public:
  ArticleTagRecord() {}

  ArticleTagRecord(int article_id, int tag_id)
      : article_id(article_id), tag_id(tag_id) {}

  void print() const override {
    cout << "article_id: " << article_id << ", tag_id: " << tag_id << endl;
  }

  unordered_map<string, string> to_unordered_map() override {
    unordered_map<string, string> data;
    data.insert({"article_id", to_string(article_id)});
    data.insert({"tag_id", to_string(tag_id)});
    return data;
  }

  // Getters
  int getArticleId() const { return article_id; }
  int getTagId() const { return tag_id; }

  // Setters
  void setArticleId(int article_id) {
    if (article_id <= 0) {
      throw invalid_argument("article_id must be positive");
    }
    this->article_id = article_id;
  }

  void setTagId(int tag_id) {
    if (tag_id <= 0) {
      throw invalid_argument("tag_id must be positive");
    }
    this->tag_id = tag_id;
  }
};

#endif // ARTICLE_TAG_RECORD_H