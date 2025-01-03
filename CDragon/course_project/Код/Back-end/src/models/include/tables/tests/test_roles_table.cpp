#include "records/role_record.h"
#include "tables/roles_table.h"
#include <gtest/gtest.h>
#include <stdexcept>
#include <vector>

using std::runtime_error;
using std::vector;

static RolesTable rolesTable;

TEST(RolesTableTest, InsertAndGetRoleByName) {
  EXPECT_TRUE(rolesTable.insert("Admin", true, true, true, true, true));
  vector<RoleRecord> roles = rolesTable.searchByName("Admin");
  ASSERT_FALSE(roles.empty());

  RoleRecord role = roles.at(0);

  EXPECT_EQ(role.getName(), "Admin");
  EXPECT_TRUE(role.getCanCreateArticles());
  EXPECT_TRUE(role.getCanEditArticles());
  EXPECT_TRUE(rolesTable.remove(role.getRoleId()));
}

TEST(RolesTableTest, GetRoleById) {
  rolesTable.insert("Editor", true, true, false, false, false);
  vector<RoleRecord> roles = rolesTable.searchByName("Editor");
  ASSERT_FALSE(roles.empty());

  RoleRecord role = rolesTable.getRecordById(roles.at(0).getRoleId());

  EXPECT_EQ(role.getName(), "Editor");

  EXPECT_TRUE(role.getCanCreateArticles());
  EXPECT_TRUE(role.getCanEditArticles());

  EXPECT_FALSE(role.getCanDeleteArticles());
  EXPECT_FALSE(role.getCanArchiveArticles());
  EXPECT_FALSE(role.getCanRestoreArticles());

  EXPECT_TRUE(rolesTable.remove(role.getRoleId()));
}

TEST(RolesTableTest, RemoveRole) {
  rolesTable.insert("Someone", false, false, false, false, false);
  vector<RoleRecord> roles = rolesTable.searchByName("Someone");
  ASSERT_FALSE(roles.empty());

  RoleRecord role = roles.at(0);
  EXPECT_TRUE(rolesTable.remove(role.getRoleId()));

  EXPECT_THROW(rolesTable.getRecordById(role.getRoleId()), runtime_error);
}

TEST(RolesTableTest, UpdateRole) {
  rolesTable.insert("Junior Author", true, true, false, false, false);
  vector<RoleRecord> roles = rolesTable.searchByName("Junior Author");
  ASSERT_FALSE(roles.empty());

  RoleRecord role = roles.at(0);
  EXPECT_TRUE(rolesTable.update(role.getRoleId(), "Senior Author", true, true,
                                false, false, false));

  RoleRecord updatedRole = rolesTable.getRecordById(role.getRoleId());

  EXPECT_EQ(updatedRole.getName(), "Senior Author");

  EXPECT_TRUE(updatedRole.getCanCreateArticles());
  EXPECT_TRUE(updatedRole.getCanEditArticles());
  EXPECT_FALSE(updatedRole.getCanDeleteArticles());

  EXPECT_TRUE(rolesTable.remove(updatedRole.getRoleId()));
}

TEST(RolesTableTest, GetAllRoles) {
  rolesTable.insert("Someone", false, false, false, false, false);
  rolesTable.insert("Admin", true, true, true, true, true);

  vector<RoleRecord> allRoles = rolesTable.getAll();
  ASSERT_EQ(allRoles.size(), 2);

  bool foundSomeone = false;
  bool foundAdmin = false;

  for (const RoleRecord &role : allRoles) {
    if (role.getName() == "Someone") {
      foundSomeone = true;
    }

    if (role.getName() == "Admin") {
      foundAdmin = true;
    }

    rolesTable.remove(role.getRoleId());
  }

  EXPECT_TRUE(foundSomeone);
  EXPECT_TRUE(foundAdmin);
}

TEST(RolesTableTest, SearchByName) {
  rolesTable.insert("SuperAdmin", true, true, true, true, true);
  rolesTable.insert("SomeAdmin", false, true, false, false, false);

  vector<RoleRecord> matches = rolesTable.searchByName("Admin");

  EXPECT_EQ(matches.size(), 2);

  for (const RoleRecord &role : matches) {
    EXPECT_TRUE(role.getName().find("Admin") != std::string::npos);
    rolesTable.remove(role.getRoleId());
  }
}

TEST(RolesTableTest, GetNonExistentRoleByIdThrowsException) {
  EXPECT_THROW(rolesTable.getRecordById(9999), runtime_error);
}

// RoleRecord

TEST(RoleRecordTest, SettersAndGetters) {
  RoleRecord role(1, "Viewer", false, false, false, false, false);

  EXPECT_EQ(role.getRoleId(), 1);
  EXPECT_EQ(role.getName(), "Viewer");

  role.setName("UpdatedViewer");
  EXPECT_EQ(role.getName(), "UpdatedViewer");

  role.setCanCreateArticles(true);
  EXPECT_TRUE(role.getCanCreateArticles());

  role.setCanDeleteArticles(true);
  EXPECT_TRUE(role.getCanDeleteArticles());
}

TEST(RoleRecordTest, ToUnorderedMap) {
  RoleRecord role(1, "SomeRole", true, false, true, true, true);
  auto data = role.to_unordered_map();

  EXPECT_EQ(data.at("role_id"), "1");
  EXPECT_EQ(data.at("name"), "SomeRole");
  EXPECT_EQ(data.at("can_create_articles"), "1");
  EXPECT_EQ(data.at("can_edit_articles"), "0");
}
