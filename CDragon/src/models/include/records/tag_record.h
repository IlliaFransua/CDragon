#ifndef TAG_RECORD_H
#define TAG_RECORD_H

#include "base_models/base_record_model.h"
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>

using std::cout, std::endl;
using std::invalid_argument;
using std::to_string;
using std::unordered_map;

class TagRecord : public BaseRecordModel {
private:
  int tag_id;
  string name;

public:
  TagRecord() {}

  TagRecord(int tag_id, const string &name) : tag_id(tag_id), name(name) {}

  void print() const override {
    cout << "tag_id: " << tag_id << ", name: " << name << endl;
  }

  unordered_map<string, string> to_unordered_map() override {
    unordered_map<string, string> data;
    data.insert({"tag_id", to_string(tag_id)});
    data.insert({"name", name});
    return data;
  }

  // Getters
  int getTagId() const { return tag_id; }
  string getName() const { return name; }

  // Setters
  void setTagId(int tag_id) {
    if (tag_id <= 0) {
      throw invalid_argument("tag_id must be positive");
    }
    this->tag_id = tag_id;
  }

  void setName(const string &name) {
    if (name.empty()) {
      throw invalid_argument("name cannot be empty");
    }
    this->name = name;
  }
};

#endif // TAG_RECORD_H