#ifndef ROLE_RECORD_H
#define ROLE_RECORD_H

#include "base_models/base_record_model.h"
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>

using std::cout, std::endl;
using std::invalid_argument;
using std::to_string;
using std::unordered_map;

class RoleRecord : public BaseRecordModel {
private:
  int role_id;
  string name;
  bool can_create_articles;
  bool can_edit_articles;
  bool can_delete_articles;
  bool can_archive_articles;
  bool can_restore_articles;

public:
  RoleRecord() {}

  RoleRecord(int role_id, const string &name, bool can_create_articles,
             bool can_edit_articles, bool can_delete_articles,
             bool can_archive_articles, bool can_restore_articles)
      : role_id(role_id), name(name), can_create_articles(can_create_articles),
        can_edit_articles(can_edit_articles),
        can_delete_articles(can_delete_articles),
        can_archive_articles(can_archive_articles),
        can_restore_articles(can_restore_articles) {}

  RoleRecord(const RoleRecord &other)
      : role_id(other.role_id), name(other.name),
        can_create_articles(other.can_create_articles),
        can_edit_articles(other.can_edit_articles),
        can_delete_articles(other.can_delete_articles),
        can_archive_articles(other.can_archive_articles),
        can_restore_articles(other.can_restore_articles) {}

  void print() const override {
    cout << "role_id: " << role_id << ", name: " << name
         << ", can_create_articles: " << can_create_articles
         << ", can_edit_articles: " << can_edit_articles
         << ", can_delete_articles: " << can_delete_articles
         << ", can_archive_articles: " << can_archive_articles
         << ", can_restore_articles: " << can_restore_articles << endl;
  }

  unordered_map<string, string> to_unordered_map() override {
    unordered_map<string, string> data;
    data.insert({"role_id", to_string(role_id)});
    data.insert({"name", name});
    data.insert({"can_create_articles", to_string(can_create_articles)});
    data.insert({"can_edit_articles", to_string(can_edit_articles)});
    data.insert({"can_delete_articles", to_string(can_delete_articles)});
    data.insert({"can_archive_articles", to_string(can_delete_articles)});
    data.insert({"can_restore_articles", to_string(can_delete_articles)});
    return data;
  }

  // Getters
  int getRoleId() const { return role_id; }
  string getName() const { return name; }
  bool getCanCreateArticles() const { return can_create_articles; }
  bool getCanEditArticles() const { return can_edit_articles; }
  bool getCanDeleteArticles() const { return can_delete_articles; }
  bool getCanArchiveArticles() const { return can_archive_articles; }
  bool getCanRestoreArticles() const { return can_restore_articles; }

  // Setters
  void setRoleId(int role_id) {
    if (role_id <= 0) {
      throw invalid_argument("role_id must be positive");
    }
    this->role_id = role_id;
  }

  void setName(const string &name) {
    if (name.empty()) {
      throw invalid_argument("name cannot be empty");
    }
    this->name = name;
  }

  void setCanCreateArticles(bool can_create) {
    this->can_create_articles = can_create;
  }

  void setCanEditArticles(bool can_edit) { this->can_edit_articles = can_edit; }

  void setCanDeleteArticles(bool can_delete) {
    this->can_delete_articles = can_delete;
  }

  void setCanArchiveArticles(bool can_archive) {
    this->can_archive_articles = can_archive;
  }

  void setCanRestoreArticles(bool can_restore) {
    this->can_restore_articles = can_restore;
  }
};

#endif // ROLE_RECORD_H