#include "records/role_record.h"
#include "records/user_record.h"
#include "records/user_role_record.h"
#include "tables/roles_table.h"
#include "tables/user_roles_table.h"
#include "tables/users_table.h"
#include <gtest/gtest.h>
#include <stdexcept>
#include <vector>

using std::invalid_argument, std::runtime_error;
using std::vector;

static UsersTable usersTable;
static RolesTable rolesTable;
static UserRolesTable userRolesTable;

TEST(UserRolesTableTest, InsertUserRole) {
  usersTable.insert("user1", "user1@gmail.com", "380671031040",
                    "hashed_password1");
  rolesTable.insert("admin", true, true, true, true, true);

  UserRecord user = usersTable.getRecordByUsername("user1");
  RoleRecord role = rolesTable.getRecordByName("admin");

  EXPECT_TRUE(userRolesTable.insert(user.getUserId(), role.getRoleId()));

  vector<int> roleIds = userRolesTable.getRoleIdsByUserId(user.getUserId());
  EXPECT_EQ(roleIds.size(), 1);
  EXPECT_EQ(roleIds.at(0), role.getRoleId());

  vector<int> userIds = userRolesTable.getUserIdsByRoleId(role.getRoleId());
  EXPECT_EQ(userIds.size(), 1);
  EXPECT_EQ(userIds.at(0), user.getUserId());

  EXPECT_TRUE(userRolesTable.remove(user.getUserId(), role.getRoleId()));
  EXPECT_TRUE(usersTable.remove(user.getUserId()));
  EXPECT_TRUE(rolesTable.remove(role.getRoleId()));
}

TEST(UserRolesTableTest, RemoveUserRole) {
  usersTable.insert("user2", "user2@gmail.com", "380671031040",
                    "hashed_password2");
  rolesTable.insert("editor", true, true, false, false, false);

  UserRecord user = usersTable.getRecordByUsername("user2");
  RoleRecord role = rolesTable.getRecordByName("editor");

  EXPECT_TRUE(userRolesTable.insert(user.getUserId(), role.getRoleId()));

  EXPECT_TRUE(userRolesTable.remove(user.getUserId(), role.getRoleId()));

  vector<int> roleIds = userRolesTable.getRoleIdsByUserId(user.getUserId());
  EXPECT_TRUE(roleIds.empty());

  vector<int> userIds = userRolesTable.getUserIdsByRoleId(role.getRoleId());
  EXPECT_TRUE(userIds.empty());

  EXPECT_TRUE(usersTable.remove(user.getUserId()));
  EXPECT_TRUE(rolesTable.remove(role.getRoleId()));
}

TEST(UserRolesTableTest, GetRecordByUserIdAndRoleId) {
  usersTable.insert("user3", "user3@gmail.com", "380671031040",
                    "hashed_password3");
  rolesTable.insert("moderator", true, false, false, true, false);

  UserRecord user = usersTable.getRecordByUsername("user3");
  RoleRecord role = rolesTable.getRecordByName("moderator");

  EXPECT_TRUE(userRolesTable.insert(user.getUserId(), role.getRoleId()));

  UserRoleRecord userRole = userRolesTable.getRecordByUserIdAndRoleId(
      user.getUserId(), role.getRoleId());

  EXPECT_EQ(userRole.getUserId(), user.getUserId());
  EXPECT_EQ(userRole.getRoleId(), role.getRoleId());

  EXPECT_TRUE(userRolesTable.remove(user.getUserId(), role.getRoleId()));
  EXPECT_TRUE(usersTable.remove(user.getUserId()));
  EXPECT_TRUE(rolesTable.remove(role.getRoleId()));
}

TEST(UserRolesTableTest, GetAll) {
  usersTable.insert("user4", "user4@gmail.com", "380970070535",
                    "hashed_password4");
  usersTable.insert("user5", "user5@gmail.com", "380671031040",
                    "hashed_password5");
  rolesTable.insert("viewer", false, false, false, false, false);

  UserRecord user1 = usersTable.getRecordByUsername("user4");
  UserRecord user2 = usersTable.getRecordByUsername("user5");

  RoleRecord role = rolesTable.getRecordByName("viewer");

  EXPECT_TRUE(userRolesTable.insert(user1.getUserId(), role.getRoleId()));
  EXPECT_TRUE(userRolesTable.insert(user2.getUserId(), role.getRoleId()));

  vector<UserRoleRecord> allRecords = userRolesTable.getAll();
  EXPECT_EQ(allRecords.size(), 2);

  EXPECT_TRUE(userRolesTable.remove(user1.getUserId(), role.getRoleId()));
  EXPECT_TRUE(userRolesTable.remove(user2.getUserId(), role.getRoleId()));
  EXPECT_TRUE(usersTable.remove(user1.getUserId()));
  EXPECT_TRUE(usersTable.remove(user2.getUserId()));
  EXPECT_TRUE(rolesTable.remove(role.getRoleId()));
}

TEST(UserRolesTableTest, GetNonExistentRecordThrowsException) {
  EXPECT_THROW(userRolesTable.getRecordByUserIdAndRoleId(9999, 9999),
               runtime_error);
}
