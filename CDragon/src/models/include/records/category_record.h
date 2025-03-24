#ifndef CATEGORY_RECORD_H
#define CATEGORY_RECORD_H

#include "base_models/base_record_model.h"
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>

using std::cout, std::endl;
using std::invalid_argument;
using std::to_string;
using std::unordered_map;

class CategoryRecord : public BaseRecordModel {
private:
  int category_id;
  string name;
  string description;

public:
  CategoryRecord() {}

  CategoryRecord(int category_id, const string &name, const string &description)
      : category_id(category_id), name(name), description(description) {}

  void print() const override {
    cout << "category_id: " << category_id << ", name: " << name
         << ", description: " << description << endl;
  }

  unordered_map<string, string> to_unordered_map() override {
    unordered_map<string, string> data;
    data.insert({"category_id", std::to_string(category_id)});
    data.insert({"name", name});
    data.insert({"description", description});
    return data;
  }

  // Getters
  int getCategoryId() const { return category_id; }
  string getName() const { return name; }
  string getDescription() const { return description; }

  // Setters
  void setCategoryId(int category_id) {
    if (category_id <= 0) {
      throw invalid_argument("category_id must be positive");
    }
    this->category_id = category_id;
  }

  void setName(const string &name) {
    if (name.empty()) {
      throw invalid_argument("name cannot be empty");
    }
    this->name = name;
  }

  void setDescription(const string &description) {
    this->description = description;
  }
};

#endif // CATEGORY_RECORD_H