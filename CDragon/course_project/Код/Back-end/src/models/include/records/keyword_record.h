#ifndef KEYWORD_RECORD_H
#define KEYWORD_RECORD_H

#include "base_models/base_record_model.h"
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>

using std::cout, std::endl;
using std::invalid_argument;
using std::to_string;
using std::unordered_map;

class KeywordRecord : public BaseRecordModel {
private:
  int keyword_id;
  string keyword;

public:
  KeywordRecord() {}

  KeywordRecord(int keyword_id, const string &keyword)
      : keyword_id(keyword_id), keyword(keyword) {}

  void print() const override {
    cout << "keyword_id: " << keyword_id << ", keyword: " << keyword << endl;
  }

  unordered_map<string, string> to_unordered_map() override {
    unordered_map<string, string> data;
    data.insert({"keyword_id", std::to_string(keyword_id)});
    data.insert({"keyword", keyword});
    return data;
  }

  // Getters
  int getKeywordId() const { return keyword_id; }
  string getKeyword() const { return keyword; }

  // Setters
  void setKeywordId(int keyword_id) {
    if (keyword_id <= 0) {
      throw invalid_argument("keyword_id must be positive");
    }
    this->keyword_id = keyword_id;
  }

  void setKeyword(const string &keyword) {
    if (keyword.empty()) {
      throw invalid_argument("keyword cannot be empty");
    }
    this->keyword = keyword;
  }
};

#endif // KEYWORD_RECORD_H