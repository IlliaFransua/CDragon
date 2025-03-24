#ifndef DATABASE_H
#define DATABASE_H

#include <jdbc/mysql_driver.h>
#include <jdbc/mysql_connection.h>
#include <jdbc/cppconn/statement.h>
#include <jdbc/cppconn/resultset.h>
#include <jdbc/cppconn/prepared_statement.h>

#include <string>
#include <memory>

using std::string;
using std::unique_ptr;

class Database {
private:
    sql::mysql::MySQL_Driver* driver;
    unique_ptr<sql::Connection> connection;

public:
    Database() {
        driver = sql::mysql::get_mysql_driver_instance();
    }
    ~Database() = default;

    void connect(
        const string& host, 
        const string& user, 
        const string& password, 
        const string& dbname
        ) {
        connection.reset(driver->connect(host, user, password));
        connection->setSchema(dbname);
    }

    void execute(const string& query) {
        unique_ptr<sql::Statement> stmt(connection->createStatement());
        stmt->execute(query);
    }

    unique_ptr<sql::ResultSet> query(const string& query) {
        unique_ptr<sql::Statement> stmt(connection->createStatement());
        return unique_ptr<sql::ResultSet>(stmt->executeQuery(query));
    }
};

#endif // DATABASE_H
