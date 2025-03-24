#ifndef BASERECORDMODEL_H
#define BASERECORDMODEL_H

#include "database.h"
#include <string>
#include <unordered_map>

using std::string;
using std::unordered_map;

class BaseRecordModel {
public:
  virtual ~BaseRecordModel() = default;

  virtual void print() const {}
  virtual unordered_map<string, string> to_unordered_map() = 0;
};

#endif // BASERECORDMODEL_H