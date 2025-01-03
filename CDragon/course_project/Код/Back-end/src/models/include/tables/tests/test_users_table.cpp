#include "records/user_record.h"
#include "tables/users_table.h"
#include <gtest/gtest.h>
#include <stdexcept>

using std::runtime_error;

static UsersTable usersTable;

TEST(UsersTableTest, InsertUser) {
  EXPECT_TRUE(
      usersTable.insert("user0", "user0@gmail.com", "1234", "hashed_password"));
  UserRecord user = usersTable.getRecordByUsername("user0");

  EXPECT_EQ(user.getUsername(), "user0");
  EXPECT_EQ(user.getEmail(), "user0@gmail.com");
  EXPECT_TRUE(usersTable.remove(user.getUserId()));
}

TEST(UsersTableTest, GetUserById) {
  usersTable.insert("user1", "user1@gmail.com", "112233", "hashed_password1");
  UserRecord user = usersTable.getRecordByUsername("user1");
  UserRecord fetchedUser = usersTable.getRecordById(user.getUserId());

  EXPECT_EQ(fetchedUser.getUsername(), "user1");
  EXPECT_EQ(fetchedUser.getEmail(), "user1@gmail.com");
  EXPECT_TRUE(usersTable.remove(user.getUserId()));
}

TEST(UsersTableTest, RemoveUser) {
  usersTable.insert("user2", "user2@gmail.com", "445566", "hashed_password2");
  UserRecord user = usersTable.getRecordByUsername("user2");

  EXPECT_TRUE(usersTable.remove(user.getUserId()));
  EXPECT_THROW(usersTable.getRecordById(user.getUserId()), runtime_error);
}

TEST(UsersTableTest, UpdateUser) {
  usersTable.insert("user3", "user3@gmail.com", "778899", "hashed_password3");
  UserRecord user = usersTable.getRecordByUsername("user3");

  EXPECT_TRUE(usersTable.update(user.getUserId(), "updated_user",
                                "updated@gmail.com", "4445556666",
                                "new_hashed_password"));
  UserRecord updatedUser = usersTable.getRecordById(user.getUserId());

  EXPECT_EQ(updatedUser.getUsername(), "updated_user");
  EXPECT_EQ(updatedUser.getEmail(), "updated@gmail.com");
  EXPECT_TRUE(usersTable.remove(user.getUserId()));
}

TEST(UsersTableTest, GetNonExistentUserByIdThrowsException) {
  EXPECT_THROW(usersTable.getRecordById(9999), runtime_error);
}

// UserRecord

TEST(UserRecordTest, SettersAndGetters) {
  UserRecord user(1, "test_user", "test@gmail.com", "12345", "hashed_pass",
                  "2023-01-01 12:00:00");

  EXPECT_EQ(user.getUserId(), 1);
  EXPECT_EQ(user.getUsername(), "test_user");
  EXPECT_EQ(user.getEmail(), "test@gmail.com");

  user.setUsername("new_user");
  EXPECT_EQ(user.getUsername(), "new_user");

  user.setEmail("new_email@gmail.com");
  EXPECT_EQ(user.getEmail(), "new_email@gmail.com");

  user.setPhone("0987654321");
  EXPECT_EQ(user.getPhone(), "0987654321");
}

TEST(UserRecordTest, ToUnorderedMap) {
  UserRecord user(1, "test_user", "map@gmail.com", "4321", "hashed_pass_map",
                  "2023-02-02 14:00:00");
  auto data = user.to_unordered_map();

  EXPECT_EQ(data.at("user_id"), "1");
  EXPECT_EQ(data.at("username"), "test_user");
  EXPECT_EQ(data.at("email"), "map@gmail.com");
}
