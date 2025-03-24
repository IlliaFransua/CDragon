#ifndef DATABASE_H
#define DATABASE_H

#include <jdbc/cppconn/prepared_statement.h>
#include <jdbc/cppconn/resultset.h>
#include <jdbc/cppconn/statement.h>
#include <jdbc/mysql_connection.h>
#include <jdbc/mysql_driver.h>

#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

using std::runtime_error;
using std::string, std::vector;
using std::unique_ptr;

class Database {
private:
  sql::mysql::MySQL_Driver *driver;
  unique_ptr<sql::Connection> connection;

public:
  Database() { driver = sql::mysql::get_mysql_driver_instance(); }

  void connect(const string &host, const string &user, const string &password,
               const string &dbname) {
    connection.reset(driver->connect(host, user, password));
    connection->setSchema(dbname);
  }

  void execute(const string &query, const vector<string> &params = {}) {
    unique_ptr<sql::PreparedStatement> stmt(
        connection->prepareStatement(query));

    for (size_t i = 0; i < params.size(); ++i) {
      try {
        stmt->setString(i + 1, params.at(i));
      } catch (const std::out_of_range &e) {
        throw runtime_error("Индекс выходит за пределы диапазона");
      } catch (const sql::SQLException &e) {
        throw runtime_error(
            "SQL не находит плейхолдер для вставки значения параметра");
      }
    }

    stmt->execute();
  }

  unique_ptr<sql::ResultSet> query(const string &query,
                                   const vector<string> &params = {}) {
    unique_ptr<sql::PreparedStatement> stmt(
        connection->prepareStatement(query));

    for (size_t i = 0; i < params.size(); ++i) {
      try {
        stmt->setString(i + 1, params.at(i));
      } catch (const std::out_of_range &e) {
        throw runtime_error("Индекс выходит за пределы диапазона");
      } catch (const sql::SQLException &e) {
        throw runtime_error(
            "SQL не находит плейхолдер для вставки значения параметра");
      }
    }

    return unique_ptr<sql::ResultSet>(stmt->executeQuery());
  }
};

#endif // DATABASE_H
