#include "routes.h"
#include "http_request.h"
#include "http_response.h"
#include "session_repository.h"
#include "template_renderer.h"

#include "records/article_category_record.h"
#include "records/article_keyword_record.h"
#include "records/article_record.h"
#include "records/article_tag_record.h"
#include "records/category_record.h"
#include "records/keyword_record.h"
#include "records/role_record.h"
#include "records/tag_record.h"
#include "records/user_record.h"
#include "records/user_role_record.h"

#include "tables/article_categories_table.h"
#include "tables/article_keywords_table.h"
#include "tables/article_tags_table.h"
#include "tables/articles_table.h"
#include "tables/categories_table.h"
#include "tables/keywords_table.h"
#include "tables/roles_table.h"
#include "tables/tags_table.h"
#include "tables/user_roles_table.h"
#include "tables/users_table.h"

#include "bcrypt.h"
#include <base64.h>
#include <fstream>
#include <nlohmann/json.hpp>
#include <regex>
#include <stdexcept>
#include <string>
#include <vector>

using nlohmann::json;
using std::exception, std::runtime_error;
using std::string, std::to_string;
using std::vector;

namespace fs = std::filesystem;

SessionRepository sessionRepository;

UsersTable usersTable;
RolesTable rolesTable;
UserRolesTable userRolesTable;
ArticlesTable articlesTable;
KeywordsTable keywordsTable;
ArticleKeywordsTable articleKeywordsTable;
CategoriesTable categoriesTable;
ArticleCategoriesTable articleCategoriesTable;
TagsTable tagsTable;
ArticleTagsTable articleTagsTable;

bool saveBase64Image(const string &base64Image, const string &outputPath) {
  string decodedImage = base64_decode(base64Image);

  fs::path path(outputPath);
  if (!fs::exists(path.parent_path())) {
    fs::create_directories(path.parent_path());
  }

  std::ofstream outFile(outputPath, std::ios::binary);
  if (!outFile) {
    return false;
  }

  outFile.write(decodedImage.data(), decodedImage.size());
  outFile.close();

  return true;
}

vector<string> parseJsonArray(const string &jsonArrayStr) {
  vector<string> result;

  try {
    json jsonArray = json::parse(jsonArrayStr);

    if (jsonArray.is_array()) {
      for (const auto &item : jsonArray) {
        result.push_back(item.get<string>());
      }
    }
  } catch (const json::parse_error &e) {
    cout << "Ошибка парсинга JSON: " << e.what() << endl;
    throw;
  } catch (const json::type_error &e) {
    cout << "Ошибка типа данных JSON: " << e.what() << endl;
    throw;
  }

  return result;
}

auto isSessionOk = [](HttpRequest &request, HttpResponse &response) {
  unordered_map<string, string> cookie = request.getCookie();
  auto it = cookie.find("session_id");

  if (it == cookie.end()) {
    return false;
  }

  if (!sessionRepository.isSessionValid(it->second)) {
    return false;
  }

  sessionRepository.extendSessionTime(it->second);
  return true;
};

bool saveFilesFromBody(const string &body, const string &contentType) {
  std::regex boundaryRegex(R"--(boundary=(.+))--");
  std::smatch boundaryMatch;

  if (std::regex_search(contentType, boundaryMatch, boundaryRegex)) {
    string boundary = "--" + boundaryMatch[1].str();

    size_t pos = body.find(boundary);
    while (pos != string::npos) {
      size_t endPos = body.find(boundary, pos + boundary.size());
      if (endPos == string::npos) {
        break;
      }

      string part =
          body.substr(pos + boundary.size(), endPos - pos - boundary.size());

      size_t headerEnd = part.find("\r\n\r\n");
      if (headerEnd != string::npos) {
        string headers = part.substr(0, headerEnd);
        string content = part.substr(headerEnd + 4);

        std::regex filenameRegex(R"--(filename="(.+?)")--");
        std::smatch filenameMatch;
        if (std::regex_search(headers, filenameMatch, filenameRegex)) {
          string filename = filenameMatch[1].str();

          std::ofstream outFile(filename, std::ios::binary);
          if (outFile) {
            outFile.write(content.data(), content.size());
            outFile.close();
          } else {
            cout << "Ошибка сохранения файла: " << filename << endl;
            return false;
          }
        }
      }

      pos = body.find(boundary, endPos);
    }
    return true;
  } else {
    cout << "Ошибка: boundary не найден" << endl;
    return false;
  }
}

bool canUserCreateArticles(int userId) {
  try {
    vector<int> roleIds = userRolesTable.getRoleIdsByUserId(userId);
    if (roleIds.empty()) {
      return false;
    }

    for (const auto &roleId : roleIds) {
      RoleRecord roleRecord = rolesTable.getRecordById(roleId);
      if (roleRecord.getCanCreateArticles()) {
        return true;
      }
    }
    return false;
  } catch (const exception &e) {
    return false;
  }
}

bool canUserEditArticles(int userId) {
  try {
    vector<int> roleIds = userRolesTable.getRoleIdsByUserId(userId);
    if (roleIds.empty()) {
      return false;
    }

    for (const auto &roleId : roleIds) {
      RoleRecord roleRecord = rolesTable.getRecordById(roleId);
      if (roleRecord.getCanEditArticles()) {
        return true;
      }
    }
    return false;
  } catch (const exception &e) {
    return false;
  }
}

bool canUserDeleteArticles(int userId) {
  try {
    vector<int> roleIds = userRolesTable.getRoleIdsByUserId(userId);
    if (roleIds.empty()) {
      return false;
    }

    for (const auto &roleId : roleIds) {
      RoleRecord roleRecord = rolesTable.getRecordById(roleId);
      if (roleRecord.getCanDeleteArticles()) {
        return true;
      }
    }
    return false;
  } catch (const exception &e) {
    return false;
  }
}

bool canUserArchiveArticles(int userId) {
  try {
    vector<int> roleIds = userRolesTable.getRoleIdsByUserId(userId);
    if (roleIds.empty()) {
      return false;
    }

    for (const auto &roleId : roleIds) {
      RoleRecord roleRecord = rolesTable.getRecordById(roleId);
      if (roleRecord.getCanArchiveArticles()) {
        return true;
      }
    }
    return false;
  } catch (const exception &e) {
    return false;
  }
}

bool canUserRestorArticles(int userId) {
  try {
    vector<int> roleIds = userRolesTable.getRoleIdsByUserId(userId);
    if (roleIds.empty()) {
      return false;
    }

    for (const auto &roleId : roleIds) {
      RoleRecord roleRecord = rolesTable.getRecordById(roleId);
      if (roleRecord.getCanRestoreArticles()) {
        return true;
      }
    }
    return false;
  } catch (const exception &e) {
    return false;
  }
}

string truncateUTF8(const string &text, size_t max_text_size) {
  size_t i = 0;
  size_t len = 0;

  while (i < text.size() && len < max_text_size) {
    unsigned char c = text[i];
    size_t char_len = 1;

    if ((c & 0x80) == 0x00)
      char_len = 1;
    else if ((c & 0xE0) == 0xC0)
      char_len = 2;
    else if ((c & 0xF0) == 0xE0)
      char_len = 3;
    else if ((c & 0xF8) == 0xF0)
      char_len = 4;

    if (i + char_len > text.size() || len + 1 > max_text_size)
      break;

    i += char_len;
    len++;
  }

  return text.substr(0, i);
}

string renderSpanText(const string &text, int max_text_size) {
  if (max_text_size <= 0) {
    return "";
  }

  string truncated_text = truncateUTF8(text, max_text_size);

  stringstream stream(truncated_text);
  string word;
  string result;

  while (stream >> word) {
    result.append(R"(<span>)" + word + R"(</span>)");
  }

  if ((truncated_text.size() < text.size()) && !word.empty() &&
      !result.empty()) {
    auto last_span_pos = result.rfind(R"(</span>)");
    if (last_span_pos != string::npos) {
      auto insert_pos = last_span_pos - 1;

      if (insert_pos >= 0 && (unsigned char)result[insert_pos] > 0x7F) {
        while (insert_pos > 0 && (result[insert_pos] & 0xC0) == 0x80) {
          --insert_pos;
        }

        result.insert(last_span_pos, "...");
      }
    }
  }

  cout << result << endl;

  return result;
}

bool isUserAdmin(int userId) {
  try {
    vector<int> roleIds = userRolesTable.getRoleIdsByUserId(userId);

    for (const auto &roleId : roleIds) {
      if (rolesTable.getRecordById(roleId).getName() == "Admin") {
        return true;
      }
    }
    return false;
  } catch (const exception &e) {
    return false;
  }
}

void setupRoutes(Router &router) {
  // data getters

  router.addStaticRoute(
      "/main_page/styles.css", [](HttpRequest &req, HttpResponse &res) {
        res.setStatus(200);
        res.addHeader("Content-Type", "text/css; charset=utf-8");
        res.setBody(loadFileContent("./views/normal/main_page/css/styles.css"));
      });

  router.addStaticRoute(
      "/article_read_page/styles.css", [](HttpRequest &req, HttpResponse &res) {
        res.setStatus(200);
        res.addHeader("Content-Type", "text/css; charset=utf-8");
        res.setBody(
            loadFileContent("./views/normal/article_read_page/css/styles.css"));
      });

  router.addStaticRoute("/category_page/styles.css", [](HttpRequest &req,
                                                        HttpResponse &res) {
    res.setStatus(200);
    res.addHeader("Content-Type", "text/css; charset=utf-8");
    res.setBody(loadFileContent("./views/normal/category_page/css/styles.css"));
  });

  // Admin Routes

  router.addStaticRoute("/admin/login_page", [](HttpRequest &req,
                                                HttpResponse &res) {
    if (isSessionOk(req, res)) {
      res.setStatus(303); // redirect
      res.addHeader("Location", "/admin/main_page");
      return;
    }

    res.setStatus(200);
    res.addHeader("Content-Type", "text/html; charset=utf-8");
    res.setBody(loadFileContent("./views/admin/login_page/html/index.html"));
  });

  router.addStaticRoute(
      "/admin/login_handler", [](HttpRequest &req, HttpResponse &res) {
        json json = req.getJsonData();

        string username, password;
        try {
          username = json.at("username");
          password = json.at("password");
        } catch (const exception &e) {
          cout << e.what() << endl;
          // res.setStatus(500);
          res.setStatus(303); // redirect
          res.addHeader("Location", "/admin/login_page");
          return;
        }

        // login
        UserRecord userRecord;
        try {
          userRecord = usersTable.getRecordByUsername(username);
        } catch (const runtime_error &e) {
          cout << e.what() << endl;
          // res.setStatus(401); // неправильные данные для авторизации
          res.setStatus(303); // redirect
          res.addHeader("Location", "/admin/login_page");
          return;
        }

        // password
        string right_password = userRecord.getPasswordHash();
        if (!bcrypt::validatePassword(password, right_password)) {
          // res.setStatus(401); // неправильные данные для авторизации
          res.setStatus(303); // redirect
          res.addHeader("Location", "/admin/login_page");
          return;
        }

        // role
        if (!isUserAdmin(userRecord.getUserId())) {
          // res.setStatus(403); // у юзера нет прав
          res.setStatus(303); // redirect
          res.addHeader("Location", "/admin/login_page");
          return;
        }

        string userdata = to_string(userRecord.getUserId());
        string sessionID = sessionRepository.createSessionID(userdata);
        res.addCookie("session_id", sessionID);
        res.setStatus(302); // redirect
        res.addHeader("Location", "/admin/main_page");
        return;

        // try {
        //   UserRecord userRecord =
        //       usersTable.getRecordByUsernameAndPassword(username,
        //       password);
        //   vector<int> userRolesIds =
        //       userRolesTable.getRoleIdsByUserId(userRecord.getUserId());

        //   if (userRolesIds.empty()) {
        //     res.setStatus(403); // у юзера нет ролей, а
        //     значит нет прав return;
        //   }

        //   for (const auto &roleId : userRolesIds) {
        //     RoleRecord roleRecord =
        //     rolesTable.getRecordById(roleId);

        //     if (roleRecord.getName() == "Admin") { // у
        //     юзера есть роль Admin
        // string userdata = std::to_string(userRecord.getUserId());
        // string sessionID = sessionRepository.createSessionID(userdata);

        // res.addCookie("session_id", sessionID);
        // res.setStatus(302); // redirect
        // res.addHeader("Location", "/admin/main_page");
        // return;
        //     }
        //   }

        //   res.setStatus(403); // нет прав
        //   return;

        // } catch (const exception &e) {
        //   cout << e.what() << endl;
        //   res.setStatus(401); // неправильные данные для
        //   логинизации return;
        // }
      });

  router.addStaticRoute("/admin/main_page", [](HttpRequest &req,
                                               HttpResponse &res) {
    if (!isSessionOk(req, res)) {
      res.setStatus(303); // redirect
      res.addHeader("Location", "/admin/login_page");
      return;
    }

    string sessionId = req.getCookie().at("session_id");
    Session session = sessionRepository.getSessionData(sessionId);

    if (session.userData.empty()) {
      // res.setStatus(500); // server error
      res.setStatus(303); // redirect
      res.addHeader("Location", "/admin/login_page");
      return;
    }

    int userId;
    try {
      userId = std::stoi(session.userData);
    } catch (const runtime_error &e) {
      // res.setStatus(500); // server error
      res.setStatus(303); // redirect
      res.addHeader("Location", "/admin/login_page");
      return;
    }

    // внимание, плохой алгоритм

    vector<ArticleRecord> articles = articlesTable.getAll();
    vector<string> htmlData;
    for (const auto &article : articles) {
      htmlData.emplace_back(
          string(R"(<div class="sp-article-card" style="width: 100%">
                  <img
                    src=")") +
          "/images/" + article.getSlug() + string(R"("
                    alt="Мини-превью"
                    class="sp-article-preview"
                  />
                  <div class="sp-article-info">
                    <div
                      style="
                        display: flex;
                        flex-direction: column;
                        justify-content: space-between;
                        width: 80%;
                      "
                    >
                      <h3 class="sp-article-title" style="width: 100%">)") +
          article.getTitle() + string(R"(</h3>
                      <h5 style="opacity: 0.4; width: 100%">Дата и время публикации: )") +
          article.getReleaseDate().substr(0,
                                          article.getReleaseDate().size() - 3) +
          string(R"(</h5>
                    </div>
                    <div class="sp-article-buttons">
                      <div class="sp-article-buttons-row">)") +
          (canUserEditArticles(userId)
               ? string(
                     R"(<button class="sp-btn sp-edit-btn" data-article-id=")") +
                     to_string(article.getArticleId()) +
                     string(R"(" onclick="redirectToArticleEditor(this) ">
                          Редактировать
                        </button>)")
               : string(R"()")) +
          (canUserArchiveArticles(userId)
               ? string(
                     R"(<button class="sp-btn sp-archive-btn" data-article-id=")") +
                     to_string(article.getArticleId()) +
                     string(R"(" onclick="archiveArticle(this) ">
                          Архивировать
                        </button>)")
               : string(R"()")) +
          string(R"(</div>
                      <div class="sp-article-buttons-row">)") +
          (canUserDeleteArticles(userId)
               ? string(
                     R"(<button class="sp-btn sp-delete-btn" data-article-id=")") +
                     to_string(article.getArticleId()) +
                     string(R"(" onclick="deleteArticle(this) ">
                          Удалить
                        </button>)")
               : string(R"()")) +
          string(R"(
                        <button class="sp-btn sp-public-btn" data-article-id=")") +
          to_string(article.getArticleId()) +
          string(R"(" onclick="publishArticle(this) ">
                          Опубликовать сейчас
                        </button>
                      </div>
                    </div>
                  </div></div>)"));
    }

    unordered_map<string, vector<string>> data = {{"articles", htmlData}};

    res.setStatus(200);
    res.addHeader("Content-Type", "text/html; charset=utf-8");
    res.setBody(TemplateRenderer::render(
        "./views/admin/main_page/html/index.html", data));
  });

  router.addStaticRoute("/admin/main_page/filter_handler", [](HttpRequest &req,
                                                              HttpResponse
                                                                  &res) {
    if (!isSessionOk(req, res)) {
      res.setStatus(303); // redirect
      res.addHeader("Location", "/admin/login_page");
      return;
    }

    json data = req.getJsonData();

    string searchTitle = data.at("searchTitle");
    string statusFilter = data.at("statusFilter");
    string releaseDateStart = data.at("releaseDateStart");
    string releaseDateEnd = data.at("releaseDateEnd");
    vector<string> categoryFilter =
        data.at("categoryFilter").get<vector<string>>();
    vector<string> keywordFilter =
        data.at("keywordFilter").get<vector<string>>();
    vector<string> tagFilter = data.at("tagFilter").get<vector<string>>();

    vector<ArticleRecord> articles = articlesTable.filterArticles(
        searchTitle, statusFilter, releaseDateStart, releaseDateEnd,
        categoryFilter, keywordFilter, tagFilter);

    // внимание, плохой алгоритм

    string sessionId = req.getCookie().at("session_id");
    Session session = sessionRepository.getSessionData(sessionId);

    if (session.userData.empty()) {
      // res.setStatus(500); // server error
      res.setStatus(303); // redirect
      res.addHeader("Location", "/admin/login_page");
      return;
    }

    int userId;
    try {
      userId = std::stoi(session.userData);
    } catch (const runtime_error &e) {
      // res.setStatus(500); // server error
      res.setStatus(303); // redirect
      res.addHeader("Location", "/admin/login_page");
      return;
    }

    string htmlPrice;
    for (const auto &article : articles) {
      htmlPrice +=
          (string(R"(<div class="sp-article-card" style="width: 100%">
                  <img
                    src=")") +
           "/images/" + article.getSlug() + string(R"("
                    alt="Мини-превью"
                    class="sp-article-preview"
                  />
                  <div class="sp-article-info">
                    <div
                      style="
                        display: flex;
                        flex-direction: column;
                        justify-content: space-between;
                        width: 80%;
                      "
                    >
                      <h3 class="sp-article-title" style="width: 100%">)") +
           article.getTitle() + string(R"(</h3>
                      <h5 style="opacity: 0.4; width: 100%">Дата и время публикации: )") +
           article.getReleaseDate().substr(0, article.getReleaseDate().size() -
                                                  3) +
           string(R"(</h5>
                    </div>
                    <div class="sp-article-buttons">
                      <div class="sp-article-buttons-row">)") +
           (canUserEditArticles(userId)
                ? string(
                      R"(<button class="sp-btn sp-edit-btn" data-article-id=")") +
                      to_string(article.getArticleId()) +
                      string(R"(" onclick="redirectToArticleEditor(this) ">
                          Редактировать
                        </button>)")
                : string(R"()")) +
           (canUserArchiveArticles(userId)
                ? string(
                      R"(<button class="sp-btn sp-archive-btn" data-article-id=")") +
                      to_string(article.getArticleId()) +
                      string(R"(" onclick="archiveArticle(this) ">
                          Архивировать
                        </button>)")
                : string(R"()")) +
           string(R"(</div>
                      <div class="sp-article-buttons-row">)") +
           (canUserDeleteArticles(userId)
                ? string(
                      R"(<button class="sp-btn sp-delete-btn" data-article-id=")") +
                      to_string(article.getArticleId()) +
                      string(R"(" onclick="deleteArticle(this) ">
                          Удалить
                        </button>)")
                : string(R"()")) +
           string(R"(
                        <button class="sp-btn sp-public-btn" data-article-id=")") +
           to_string(article.getArticleId()) +
           string(R"(" onclick="publishArticle(this) ">
                          Опубликовать сейчас
                        </button>
                      </div>
                    </div>
                  </div></div>)"));
    }

    res.setStatus(200);
    res.addHeader("Content-Type", "text/html; charset=utf-8");
    res.setBody(htmlPrice);
  });

  router.addStaticRoute(
      "/admin/article_creator_page", [](HttpRequest &req, HttpResponse &res) {
        if (!isSessionOk(req, res)) {
          res.setStatus(303); // redirect
          res.addHeader("Location", "/admin/login_page");
          return;
        }

        string sessionId = req.getCookie().at("session_id");
        Session session = sessionRepository.getSessionData(sessionId);

        if (session.userData.empty()) {
          // res.setStatus(500); // server error
          res.setStatus(303); // redirect
          res.addHeader("Location", "/admin/login_page");
          return;
        }

        int userId;
        try {
          userId = std::stoi(session.userData);
        } catch (const runtime_error &e) {
          // res.setStatus(500); // server error
          res.setStatus(303); // redirect
          res.addHeader("Location", "/admin/login_page");
          return;
        }

        if (canUserCreateArticles(userId)) {
          res.setStatus(200);
          res.setBody(
              loadFileContent("./views/admin/article_creator/html/index.html"));
          return;
        }

        // нет прав
        res.setStatus(303); // redirect
        res.addHeader("Location", "/admin/main_page");
        return;
      });

  router.addStaticRoute(
      "/admin/article_creator_handler",
      [](HttpRequest &req, HttpResponse &res) {
        if (!isSessionOk(req, res)) {
          res.setStatus(303); // redirect
          res.addHeader("Location", "/admin/login_page");
          return;
        }

        string sessionId = req.getCookie().at("session_id");
        Session session = sessionRepository.getSessionData(sessionId);

        if (session.userData.empty()) {
          // res.setStatus(500); // server error
          res.setStatus(303); // redirect
          res.addHeader("Location", "/admin/login_page");
          return;
        }

        int userId;
        try {
          userId = std::stoi(session.userData);
        } catch (const runtime_error &e) {
          // res.setStatus(500); // server error
          res.setStatus(303); // redirect
          res.addHeader("Location", "/admin/login_page");
          return;
        }

        if (!canUserCreateArticles(userId)) {
          // res.setStatus(403); // нет прав
          res.setStatus(303); // redirect
          res.addHeader("Location", "/admin/login_page");
          return;
        }

        // save article

        try {
          const json &json = req.getJsonData();

          // cout << json.dump(4) << endl;

          vector<string> keywords = json.at("keywords").get<vector<string>>();
          vector<string> categories =
              json.at("categories").get<vector<string>>();
          vector<string> tags = json.at("tags").get<vector<string>>();

          const string &slug = json.at("slug");
          const string imagePath = "./images/" + slug + ".jpg";
          if (!saveBase64Image(json.at("previewImage"), imagePath)) {
            throw runtime_error("Ошибка сохранения изображения");
          }

          const string content = json.at("content").dump();

          articlesTable.insert(json.at("title"), json.at("summary"), content,
                               json.at("status"), json.at("publishDate"), slug,
                               json.at("seoTitle"), json.at("seoDescription"),
                               imagePath);

          ArticleRecord articleRecord = articlesTable.getRecordBySlug(slug);

          if (!keywords.empty())
            for (const auto &keyword : keywords) {
              try {
                keywordsTable.insert(keyword);
              } catch (const exception &e) {
              }

              articleKeywordsTable.insert(
                  articleRecord.getArticleId(),
                  keywordsTable.searchByKeyword(keyword).at(0).getKeywordId());
            }

          if (!categories.empty())
            for (const auto &category : categories) {
              try {
                categoriesTable.insert(category, ""); // its empty description
              } catch (const exception &e) {
              }

              articleCategoriesTable.insert(
                  articleRecord.getArticleId(),
                  categoriesTable.searchByName(category).at(0).getCategoryId());
            }

          if (!tags.empty())
            for (const auto &tag : tags) {
              try {
                tagsTable.insert(tag);
              } catch (const exception &e) {
              }

              articleTagsTable.insert(
                  articleRecord.getArticleId(),
                  tagsTable.searchByName(tag).at(0).getTagId());
            }

        } catch (const exception &e) {
          cout << e.what() << endl;
          res.setStatus(500); // server error
          return;
        }

        res.setStatus(200);
        return;
      });

  router.addDynamicRoute(
      "/admin/article_editor_page/<article_id>",
      [](HttpRequest &req, HttpResponse &res) {
        if (!isSessionOk(req, res)) {
          res.setStatus(303); // redirect
          res.addHeader("Location", "/admin/login_page");
          return;
        }

        string sessionId = req.getCookie().at("session_id");
        Session session = sessionRepository.getSessionData(sessionId);

        if (session.userData.empty()) {
          // res.setStatus(500); // server error
          res.setStatus(303); // redirect
          res.addHeader("Location", "/admin/login_page");
          return;
        }

        int userId;
        try {
          userId = std::stoi(session.userData);
        } catch (const runtime_error &e) {
          // res.setStatus(500); // server error
          res.setStatus(303); // redirect
          res.addHeader("Location", "/admin/login_page");
          return;
        }

        if (canUserEditArticles(userId)) {
          res.setStatus(200);
          res.setBody(
              loadFileContent("./views/admin/article_editor/html/index.html"));
          return;
        }

        // нет прав
        res.setStatus(303); // redirect
        res.addHeader("Location", "/admin/main_page");
        return;
      });

  router.addDynamicRoute(
      "/admin/get_json_of_article/<article_id>",
      [](HttpRequest &req, HttpResponse &res) {
        // if (!isSessionOk(req, res)) {
        //   res.setStatus(303); // redirect
        //   res.addHeader("Location", "/admin/login_page");
        //   return;
        // }

        try {
          int article_id = std::stoi(req.getPathParams().at(0));
          ArticleRecord article = articlesTable.getRecordById(article_id);

          vector<int> keywordIds =
              articleKeywordsTable.getKeywordIdsForArticle(article_id);
          vector<int> categoriesIds =
              articleCategoriesTable.getCategoryIdsForArticle(article_id);
          vector<int> tagIds = articleTagsTable.getTagIdsForArticle(article_id);

          vector<string> keywords;
          for (const auto &id : keywordIds) {
            string keyword = keywordsTable.getRecordById(id).getKeyword();
            keywords.emplace_back(keyword);
          }

          vector<string> categories;
          for (const auto &id : categoriesIds) {
            string name = categoriesTable.getRecordById(id).getName();
            categories.emplace_back(name);
          }

          vector<string> tags;
          for (const auto &id : tagIds) {
            string name = tagsTable.getRecordById(id).getName();
            tags.emplace_back(name);
          }

          json articleJson = articlesTable.getRecordById(article_id).to_json();

          articleJson["keywords"] = keywords;
          articleJson["categories"] = categories;
          articleJson["tags"] = tags;

          res.setStatus(200);
          res.addHeader("Content-Type", "application/javascript");
          cout << articleJson.dump(2) << endl;
          res.setBody(articleJson.dump());
          return;

        } catch (const exception &e) {
          cout << e.what() << endl;
          res.setStatus(500);
          return;
        }
      });

  router.addStaticRoute(
      "/admin/get_json_of_all_categories",
      [](HttpRequest &req, HttpResponse &res) {
        if (!isSessionOk(req, res)) {
          res.setStatus(303); // redirect
          res.addHeader("Location", "/admin/login_page");
          return;
        }

        try {
          vector<CategoryRecord> categoriesList = categoriesTable.getAll();

          json categoriesJson = json::array();

          for (const auto &category : categoriesList) {
            json categoryJson;
            categoryJson["id"] = category.getCategoryId();
            categoryJson["name"] = category.getName();
            categoryJson["descripton"] = category.getDescription();

            categoriesJson.push_back(categoryJson);
          }

          json responseJson;
          responseJson["categories"] = categoriesJson;

          cout << "result: " << responseJson.dump(4) << endl;

          res.setStatus(200);
          res.addHeader("Content-Type", "application/javascript");
          res.setBody(responseJson.dump());
          return;

        } catch (const exception &e) {
          res.setStatus(500);
          return;
        }
      });

  router.addStaticRoute(
      "/admin/get_json_of_all_tags", [](HttpRequest &req, HttpResponse &res) {
        if (!isSessionOk(req, res)) {
          res.setStatus(303); // redirect
          res.addHeader("Location", "/admin/login_page");
          return;
        }

        try {
          vector<TagRecord> tagsList = tagsTable.getAll();

          json tagsJson = json::array();

          for (const auto &tag : tagsList) {
            json tagJson;
            tagJson["id"] = tag.getTagId();
            tagJson["name"] = tag.getName();

            tagsJson.push_back(tagJson);
          }

          json responseJson;
          responseJson["tags"] = tagsJson;

          res.setStatus(200);
          res.addHeader("Content-Type", "application/javascript");
          res.setBody(responseJson.dump());
          return;

        } catch (const exception &e) {
          cout << "get_json_of_all_article_categories: " << e.what() << endl;
          res.setStatus(500);
          return;
        }
      });

  // archive methond
  router.addDynamicRoute(
      "/admin/get_all_categories_of_aticle/<article_id>",
      [](HttpRequest &req, HttpResponse &res) {
        if (!isSessionOk(req, res)) {
          res.setStatus(303); // redirect
          res.addHeader("Location", "/admin/login_page");
          return;
        }

        try {
          int article_id = std::stoi(req.getPathParams().at(0));
          ArticleRecord article = articlesTable.getRecordById(article_id);

          vector<int> tagIds =
              articleTagsTable.getTagIdsForArticle(article.getArticleId());

          json tags = json::array();

          for (const auto &tagId : tagIds) {

            json tagJson;
            tagJson["tag_id"] = tagJson;
            tagJson["name"] = tagsTable.getRecordById(tagJson).getName();

            tags.push_back(tagJson);
          }

          json responseJson;
          responseJson["tags"] = tags;

          res.setStatus(200);
          res.addHeader("Content-Type", "application/javascript");
          res.setBody(responseJson.dump());
          return;

        } catch (const exception &e) {
          cout << "get_json_of_all_article_categories: " << e.what() << endl;
          res.setStatus(500);
          return;
        }
      });

  // archive methond
  router.addDynamicRoute(
      "/admin/get_all_tags_of_aticle/<article_id>",
      [](HttpRequest &req, HttpResponse &res) {
        if (!isSessionOk(req, res)) {
          res.setStatus(303); // redirect
          res.addHeader("Location", "/admin/login_page");
          return;
        }

        try {
          int article_id = std::stoi(req.getPathParams().at(0));
          ArticleRecord article = articlesTable.getRecordById(article_id);

          vector<int> tagIds =
              articleTagsTable.getTagIdsForArticle(article.getArticleId());

          json tags = json::array();

          for (const auto &tagId : tagIds) {

            json tagJson;
            tagJson["tag_id"] = tagJson;
            tagJson["name"] = tagsTable.getRecordById(tagJson).getName();

            tags.push_back(tagJson);
          }

          json responseJson;
          responseJson["tags"] = tags;

          res.setStatus(200);
          res.addHeader("Content-Type", "application/javascript");
          res.setBody(responseJson.dump());
          return;

        } catch (const exception &e) {
          cout << "get_json_of_all_article_categories: " << e.what() << endl;
          res.setStatus(500);
          return;
        }
      });

  router.addStaticRoute("/admin/article_editor_handler", [](HttpRequest &req,
                                                            HttpResponse &res) {
    if (!isSessionOk(req, res)) {
      res.setStatus(303); // redirect
      res.addHeader("Location", "/admin/login_page");
      return;
    }

    string sessionId = req.getCookie().at("session_id");
    Session session = sessionRepository.getSessionData(sessionId);

    if (session.userData.empty()) {
      // res.setStatus(500); // server error
      res.setStatus(303); // redirect
      res.addHeader("Location", "/admin/login_page");
      return;
    }

    int userId;
    try {
      userId = std::stoi(session.userData);
    } catch (const runtime_error &e) {
      // res.setStatus(500); // server error
      res.setStatus(303); // redirect
      res.addHeader("Location", "/admin/login_page");
      return;
    }

    if (!canUserCreateArticles(userId)) {
      // res.setStatus(403); // нет прав
      res.setStatus(303); // redirect
      res.addHeader("Location", "/admin/login_page");
      return;
    }

    try {
      const json &json = req.getJsonData();

      vector<string> keywords = json.at("keywords").get<vector<string>>();
      vector<string> categories = json.at("categories").get<vector<string>>();
      vector<string> tags = json.at("tags").get<vector<string>>();

      const string &slug = json.at("slug");
      const string imagePath = "./images/" + slug + ".jpg";
      cout << "new image:" << json.at("previewImage").get<string>() << "]"
           << endl;
      if (!json.at("previewImage").get<string>().empty() &&
          !saveBase64Image(json.at("previewImage"), imagePath)) {
        // throw runtime_error("Ошибка сохранения изображения");
      }

      ArticleRecord updatetArticle(articlesTable.getRecordBySlug(slug));

      const string content = json.at("content");

      updatetArticle.setTitle(json.at("title"));
      updatetArticle.setSummary(json.at("summary"));
      updatetArticle.setContent(content);
      updatetArticle.setStatus(json.at("status"));
      updatetArticle.setReleaseDate(json.at("publishDate"));
      updatetArticle.setSeoTitle(json.at("seoTitle"));
      updatetArticle.setSeoDescription(json.at("seoDescription"));
      // updatetArticle.setPreviewImage(imagePath);

      articlesTable.update(updatetArticle);

      int article_id = updatetArticle.getArticleId();

      if (!keywords.empty()) {
        vector<int> keywordIds =
            articleKeywordsTable.getKeywordIdsForArticle(article_id);

        for (const auto &keywordId : keywordIds) {
          articleKeywordsTable.remove(article_id, keywordId);
        }

        for (const auto &keyword : keywords) {
          try {
            keywordsTable.insert(keyword);
          } catch (const exception &e) {
          }

          try {
            articleKeywordsTable.insert(
                article_id,
                keywordsTable.searchByKeyword(keyword).at(0).getKeywordId());
          } catch (const exception &e) {
          }
        }
      }

      if (!categories.empty()) {
        vector<int> categoryIds =
            articleCategoriesTable.getCategoryIdsForArticle(article_id);

        for (const auto &categoryId : categoryIds) {
          articleCategoriesTable.remove(article_id, categoryId);
        }

        for (const auto &category : categories) {
          try {
            categoriesTable.insert(category, ""); // its empty description
          } catch (const exception &e) {
          }

          try {
            articleCategoriesTable.insert(
                article_id,
                categoriesTable.searchByName(category).at(0).getCategoryId());
          } catch (const exception &e) {
          }
        }
      }

      if (!tags.empty()) {
        vector<int> tagIds = articleTagsTable.getTagIdsForArticle(article_id);

        for (const auto &tagId : tagIds) {
          articleTagsTable.remove(article_id, tagId);
        }

        for (const auto &tag : tags) {
          try {
            tagsTable.insert(tag);
          } catch (const exception &e) {
          }

          try {
            articleTagsTable.insert(
                article_id, tagsTable.searchByName(tag).at(0).getTagId());
          } catch (const exception &e) {
          }
        }
      }
    } catch (const exception &e) {
      cout << e.what() << endl;
      res.setStatus(500); // server error
      return;
    }

    res.setStatus(200);
    return;
  });

  router.addDynamicRoute(
      "/admin/article_editor_page/publish/<article_id>",
      [](HttpRequest &req, HttpResponse &res) {
        if (!isSessionOk(req, res)) {
          res.setStatus(303); // redirect
          res.addHeader("Location", "/admin/login_page");
          return;
        }

        try {
          if (!articlesTable.publish(std::stoi(req.getPathParams().at(0)))) {
            res.setStatus(500);
            return;
          }
          res.setStatus(200);
          return;
        } catch (const exception &e) {
          cout << e.what() << endl;
          res.setStatus(500);
          return;
        }
      });

  router.addDynamicRoute(
      "/admin/article_editor_page/delete/<article_id>",
      [](HttpRequest &req, HttpResponse &res) {
        if (!isSessionOk(req, res)) {
          res.setStatus(303); // redirect
          res.addHeader("Location", "/admin/login_page");
          return;
        }

        try {
          if (!articlesTable.remove(std::stoi(req.getPathParams().at(0)))) {
            res.setStatus(500);
            return;
          }
          res.setStatus(200);
          return;
        } catch (const exception &e) {
          cout << e.what() << endl;
          res.setStatus(500);
          return;
        }
      });

  router.addDynamicRoute(
      "/admin/article_editor_page/archive/<article_id>",
      [](HttpRequest &req, HttpResponse &res) {
        if (!isSessionOk(req, res)) {
          res.setStatus(303); // redirect
          res.addHeader("Location", "/admin/login_page");
          return;
        }

        try {
          if (!articlesTable.archive(std::stoi(req.getPathParams().at(0)))) {
            res.setStatus(500);
            return;
          }
          res.setStatus(200);
          return;
        } catch (const exception &e) {
          cout << e.what() << endl;
          res.setStatus(500);
          return;
        }
      });

  // data getters

  router.addStaticRoute("/get_filter_data", [](HttpRequest &req,
                                               HttpResponse &res) {
    vector<CategoryRecord> categories = categoriesTable.getAll();
    vector<KeywordRecord> keywords = keywordsTable.getAll();
    vector<TagRecord> tags = tagsTable.getAll();

    json json;

    json["categories"] = json::array();
    for (const auto &category : categories) {
      json["categories"].push_back(
          {{"id", category.getCategoryId()},
           {"name", category.getName()},
           {"description", category.getDescription()}});
    }

    json["keywords"] = json::array();
    for (const auto &keyword : keywords) {
      json["keywords"].push_back(
          {{"id", keyword.getKeyword()}, {"keyword", keyword.getKeyword()}});
    }

    json["tags"] = json::array();
    for (const auto &tag : tags) {
      json["tags"].push_back({{"id", tag.getTagId()}, {"name", tag.getName()}});
    }

    res.setStatus(200);
    res.addHeader("Content-Type", "application/json; charset=utf-8");
    res.setBody(json.dump(2));
  });

  router.addDynamicRoute(
      "/images/<slug>", [](HttpRequest &req, HttpResponse &res) {
        try {
          string slug = req.getPathParams().at(0);
          res.setStatus(200);
          res.addHeader("Content-Type", "image/jpeg");
          res.setBody(loadFileContent("./images/" + slug + ".jpg"));
        } catch (const exception &e) {
          cout << e.what() << endl;
          res.setStatus(404);
        }
      });

  router.addStaticRoute("/admin/article_creator/css/styles.css",
                        [](HttpRequest &req, HttpResponse &res) {
                          res.setStatus(200);
                          res.addHeader("Content-Type", "text/css");

                          res.setBody(loadFileContent(
                              "./views/admin/article_creator/css/styles.css"));
                        });

  router.addStaticRoute(
      "/admin/article_creator/js/functions.js",
      [](HttpRequest &request, HttpResponse &response) {
        response.setStatus(200);
        response.addHeader("Content-Type", "application/javascript");

        response.setBody(
            loadFileContent("./views/admin/article_creator/js/functions.js"));
      });

  router.addStaticRoute(
      "/admin/article_creator/js/quill_settings.js",
      [](HttpRequest &request, HttpResponse &response) {
        response.setStatus(200);
        response.addHeader("Content-Type", "application/javascript");

        response.setBody(loadFileContent("./views/admin/article_creator/"
                                         "js/quill_settings.js"));
      });

  router.addStaticRoute(
      "/admin/article_creator/js/handlers.js",
      [](HttpRequest &request, HttpResponse &response) {
        response.setStatus(200);
        response.addHeader("Content-Type", "application/javascript");

        response.setBody(
            loadFileContent("./views/admin/article_creator/js/handlers.js"));
      });

  router.addStaticRoute("/admin/article_editor/css/styles.css",
                        [](HttpRequest &req, HttpResponse &res) {
                          res.setStatus(200);
                          res.addHeader("Content-Type", "text/css");

                          res.setBody(loadFileContent(
                              "./views/admin/article_editor/css/styles.css"));
                        });

  router.addStaticRoute(
      "/admin/article_editor/js/functions.js",
      [](HttpRequest &request, HttpResponse &response) {
        response.setStatus(200);
        response.addHeader("Content-Type", "application/javascript");

        response.setBody(
            loadFileContent("./views/admin/article_editor/js/functions.js"));
      });

  router.addStaticRoute(
      "/admin/article_editor/js/quill_settings.js",
      [](HttpRequest &request, HttpResponse &response) {
        response.setStatus(200);
        response.addHeader("Content-Type", "application/javascript");

        response.setBody(loadFileContent("./views/admin/article_editor/"
                                         "js/quill_settings.js"));
      });

  router.addStaticRoute(
      "/admin/article_editor/js/handlers.js",
      [](HttpRequest &request, HttpResponse &response) {
        response.setStatus(200);
        response.addHeader("Content-Type", "application/javascript");

        response.setBody(
            loadFileContent("./views/admin/article_editor/js/handlers.js"));
      });

  router.addStaticRoute("/admin/login_page/css/style.css",
                        [](HttpRequest &request, HttpResponse &response) {
                          response.setStatus(200);
                          response.addHeader("Content-Type", "text/css");

                          response.setBody(loadFileContent(
                              "./views/admin/login_page/css/style.css"));
                        });

  router.addStaticRoute("/admin/main_page/css/styles.css",
                        [](HttpRequest &request, HttpResponse &response) {
                          response.setStatus(200);
                          response.addHeader("Content-Type", "text/css");

                          response.setBody(loadFileContent(
                              "./views/admin/main_page/css/styles.css"));
                        });

  router.addStaticRoute(
      "/admin/main_page/css/fp.css",
      [](HttpRequest &request, HttpResponse &response) {
        response.setStatus(200);
        response.addHeader("Content-Type", "text/css");

        response.setBody(loadFileContent("./views/admin/main_page/css/fp.css"));
      });

  router.addStaticRoute(
      "/admin/main_page/css/sp.css",
      [](HttpRequest &request, HttpResponse &response) {
        response.setStatus(200);
        response.addHeader("Content-Type", "text/css");

        response.setBody(loadFileContent("./views/admin/main_page/css/sp.css"));
      });

  router.addStaticRoute(
      "/admin/main_page/js/functions.js",
      [](HttpRequest &request, HttpResponse &response) {
        response.setStatus(200);
        response.addHeader("Content-Type", "application/javascript");

        response.setBody(
            loadFileContent("./views/admin/main_page/js/functions.js"));
      });

  router.addStaticRoute(
      "/admin/main_page/js/quill_settings.js",
      [](HttpRequest &request, HttpResponse &response) {
        response.setStatus(200);
        response.addHeader("Content-Type", "application/javascript");

        response.setBody(
            loadFileContent("./views/admin/main_page/js/quill_settings.js"));
      });

  router.addStaticRoute(
      "/admin/main_page/js/handlers.js",
      [](HttpRequest &request, HttpResponse &response) {
        response.setStatus(200);
        response.addHeader("Content-Type", "application/javascript");

        response.setBody(
            loadFileContent("./views/admin/main_page/js/handlers.js"));
      });

  router.addStaticRoute(
      "/admin/main_page/js/fp.js",
      [](HttpRequest &request, HttpResponse &response) {
        response.setStatus(200);
        response.addHeader("Content-Type", "application/javascript");

        response.setBody(loadFileContent("./views/admin/main_page/js/fp.js"));
      });

  router.addStaticRoute("/", [](HttpRequest &req, HttpResponse &res) {
    res.setStatus(200);
    res.addHeader("Content-Type", "text/html; charset=utf-8");

    string templatePath = "./views/normal/main_page/html/index.html";

    ArticleRecord latestPublishedArticle =
        articlesTable.getLatestPublishedArticle();
    //
    vector<ArticleRecord> animeRecords =
        articlesTable.getAllRecordsByCategory("Аніме");
    int articlesSize = animeRecords.size();
    string html;

    for (size_t i = 0; i < articlesSize; ++i) {
      if (i + 4 < articlesSize) {
        std::ostringstream stream;
        stream
            << R"(
        <div class="scope">
          <div class="bottom">
            <div class="mini-column">
              <a href="/article_read_page/)"
            << animeRecords.at(i).getArticleId() << R"("
                ><div class="bild-bord">
                  <div class="image-container">
                    <img src=")"
            << "/images/" << animeRecords.at(i).getSlug()
            << R"(" class="image" />
                  </div>

                  <div class="title">)"
            << animeRecords.at(i).getTitle() << R"(</div>

                  <div class="description">)"
            << nlohmann::json::parse(
                   animeRecords.at(i).getSeoDescription())["ops"][0]["insert"]
                   .get<string>()
            << R"(</div>
                </div></a
              >
              <a href="/article_read_page/)"
            << animeRecords.at(++i).getArticleId() << R"(">
                <div class="bild-bord">
                  <div class="image-container">
                    <img src=")"
            << "/images/" << animeRecords.at(i).getSlug()
            << R"(" class="image" />
                  </div>

                  <div class="title">)"
            << animeRecords.at(i).getTitle() << R"(</div>

                  <div class="description">)"
            << nlohmann::json::parse(
                   animeRecords.at(i).getSeoDescription())["ops"][0]["insert"]
                   .get<string>()
            << R"(</div>
                </div></a
              >
            </div>

            <div class="big-column">
              <a href="/article_read_page/)"
            << animeRecords.at(++i).getArticleId() << R"(">
                <div class="bild-bord">
                  <div class="title">)"
            << animeRecords.at(i).getTitle() << R"(</div>

                  <div class="description">)"
            << nlohmann::json::parse(
                   animeRecords.at(i).getSeoDescription())["ops"][0]["insert"]
                   .get<string>()
            << R"(</div>

                  <div class="image-container">
                    <img src=")"
            << "/images/" << animeRecords.at(i).getSlug()
            << R"(" class="image" />
                  </div>
                </div>
              </a>
            </div>

            <div class="mini-column">
              <a href="/article_read_page/)"
            << animeRecords.at(++i).getArticleId() << R"(">
                <div class="bild-bord">
                  <div class="image-container">
                    <img src=")"
            << "/images/" << animeRecords.at(i).getSlug()
            << R"(" class="image" />
                  </div>

                  <div class="title">)"
            << animeRecords.at(i).getTitle() << R"(</div>

                  <div class="description">)"
            << nlohmann::json::parse(
                   animeRecords.at(i).getSeoDescription())["ops"][0]["insert"]
                   .get<string>()
            << R"(</div>
                </div></a
              >

              <a href="/article_read_page/)"
            << animeRecords.at(++i).getArticleId() << R"(">
                <div class="bild-bord">
                  <div class="image-container">
                    <img src=")"
            << "/images/" << animeRecords.at(i).getSlug()
            << R"(" class="image" />
                  </div>

                  <div class="title">)"
            << animeRecords.at(i).getTitle() << R"(</div>

                  <div class="description">)"
            << nlohmann::json::parse(
                   animeRecords.at(i).getSeoDescription())["ops"][0]["insert"]
                   .get<string>()
            << R"(</div>
                </div></a
              >
            </div>
          </div>
        </div>
              )";

        html += stream.str();
      } else if (i + 3 < articlesSize) {
        std::ostringstream stream;
        stream
            << R"(
        <div class="scope">
          <div class="bottom">
            <div class="mini-column">
              <a href="/article_read_page/)"
            << animeRecords.at(i).getArticleId() << R"("
                ><div class="bild-bord">
                  <div class="image-container">
                    <img src=")"
            << "/images/" << animeRecords.at(i).getSlug()
            << R"(" class="image" />
                  </div>

                  <div class="title">)"
            << animeRecords.at(i).getTitle() << R"(</div>

                  <div class="description">)"
            << nlohmann::json::parse(
                   animeRecords.at(i).getSeoDescription())["ops"][0]["insert"]
                   .get<string>()
            << R"(</div>
                </div></a
              >
              <a href="/article_read_page/)"
            << animeRecords.at(++i).getArticleId() << R"(">
                <div class="bild-bord">
                  <div class="image-container">
                    <img src=")"
            << "/images/" << animeRecords.at(i).getSlug()
            << R"(" class="image" />
                  </div>

                  <div class="title">)"
            << animeRecords.at(i).getTitle() << R"(</div>

                  <div class="description">)"
            << nlohmann::json::parse(
                   animeRecords.at(i).getSeoDescription())["ops"][0]["insert"]
                   .get<string>()
            << R"(</div>
                </div></a
              >
            </div>

            <div class="big-column">
              <a href="/article_read_page/)"
            << animeRecords.at(++i).getArticleId() << R"(">
                <div class="bild-bord">
                  <div class="title">)"
            << animeRecords.at(i).getTitle() << R"(</div>

                  <div class="description">)"
            << nlohmann::json::parse(
                   animeRecords.at(i).getSeoDescription())["ops"][0]["insert"]
                   .get<string>()
            << R"(</div>

                  <div class="image-container">
                    <img src=")"
            << "/images/" << animeRecords.at(i).getSlug()
            << R"(" class="image" />
                  </div>
                </div>
              </a>
            </div>

            <div class="mini-column">
              <a href="/article_read_page/)"
            << animeRecords.at(++i).getArticleId() << R"(">
                <div class="bild-bord">
                  <div class="image-container">
                    <img src=")"
            << "/images/" << animeRecords.at(i).getSlug()
            << R"(" class="image" />
                  </div>

                  <div class="title">)"
            << animeRecords.at(i).getTitle() << R"(</div>

                  <div class="description">)"
            << nlohmann::json::parse(
                   animeRecords.at(i).getSeoDescription())["ops"][0]["insert"]
                   .get<string>()
            << R"(</div>
                </div></a
              >
            </div>
          </div>
        </div>
              )";

        html += stream.str();
      } else if (i + 2 < articlesSize) {
        std::ostringstream stream;
        stream
            << R"(
        <div class="scope">
          <div class="bottom">
            <div class="mini-column">
              <a href="/article_read_page/)"
            << animeRecords.at(i).getArticleId() << R"("
                ><div class="bild-bord">
                  <div class="image-container">
                    <img src=")"
            << "/images/" << animeRecords.at(i).getSlug()
            << R"(" class="image" />
                  </div>

                  <div class="title">)"
            << animeRecords.at(i).getTitle() << R"(</div>

                  <div class="description">)"
            << nlohmann::json::parse(
                   animeRecords.at(i).getSeoDescription())["ops"][0]["insert"]
                   .get<string>()
            << R"(</div>
                </div></a
              >
              <a href="/article_read_page/)"
            << animeRecords.at(++i).getArticleId() << R"(">
                <div class="bild-bord">
                  <div class="image-container">
                    <img src=")"
            << "/images/" << animeRecords.at(i).getSlug()
            << R"(" class="image" />
                  </div>

                  <div class="title">)"
            << animeRecords.at(i).getTitle() << R"(</div>

                  <div class="description">)"
            << nlohmann::json::parse(
                   animeRecords.at(i).getSeoDescription())["ops"][0]["insert"]
                   .get<string>()
            << R"(</div>
                </div></a
              >
            </div>

            <div class="big-column">
              <a href="/article_read_page/)"
            << animeRecords.at(++i).getArticleId() << R"(">
                <div class="bild-bord">
                  <div class="title">)"
            << animeRecords.at(i).getTitle() << R"(</div>

                  <div class="description">)"
            << nlohmann::json::parse(
                   animeRecords.at(i).getSeoDescription())["ops"][0]["insert"]
                   .get<string>()
            << R"(</div>

                  <div class="image-container">
                    <img src=")"
            << "/images/" << animeRecords.at(i).getSlug()
            << R"(" class="image" />
                  </div>
                </div>
              </a>
            </div>
          </div>
        </div>
              )";

        html += stream.str();
      } else if (i + 1 < articlesSize) {
        std::ostringstream stream;
        stream
            << R"(
        <div class="scope">
          <div class="bottom">
            <div class="mini-column">
              <a href="/article_read_page/)"
            << animeRecords.at(i).getArticleId() << R"("
                ><div class="bild-bord">
                  <div class="image-container">
                    <img src=")"
            << "/images/" << animeRecords.at(i).getSlug()
            << R"(" class="image" />
                  </div>

                  <div class="title">)"
            << animeRecords.at(i).getTitle() << R"(</div>

                  <div class="description">)"
            << nlohmann::json::parse(
                   animeRecords.at(i).getSeoDescription())["ops"][0]["insert"]
                   .get<string>()
            << R"(</div>
                </div></a
              >
              <a href="/article_read_page/)"
            << animeRecords.at(++i).getArticleId() << R"(">
                <div class="bild-bord">
                  <div class="image-container">
                    <img src=")"
            << "/images/" << animeRecords.at(i).getSlug()
            << R"(" class="image" />
                  </div>

                  <div class="title">)"
            << animeRecords.at(i).getTitle() << R"(</div>

                  <div class="description">)"
            << nlohmann::json::parse(
                   animeRecords.at(i).getSeoDescription())["ops"][0]["insert"]
                   .get<string>()
            << R"(</div>
                </div></a
              >
            </div>
          </div>
        </div>
              )";

        html += stream.str();
      } else if (i == articlesSize - 1) {
        std::ostringstream stream;
        stream
            << R"(
        <div class="scope">
          <div class="bottom">
            <div class="mini-column">
              <a href="/article_read_page/)"
            << animeRecords.at(i).getArticleId() << R"("
                ><div class="bild-bord">
                  <div class="image-container">
                    <img src=")"
            << "/images/" << animeRecords.at(i).getSlug()
            << R"(" class="image" />
                  </div>

                  <div class="title">)"
            << animeRecords.at(i).getTitle() << R"(</div>

                  <div class="description">)"
            << nlohmann::json::parse(
                   animeRecords.at(i).getSeoDescription())["ops"][0]["insert"]
                   .get<string>()
            << R"(</div>
                </div></a
              >
            </div>
          </div>
        </div>
              )";

        html += stream.str();
      }
    }

    vector<ArticleRecord> allRecord = articlesTable.getAll();

    unordered_map<string, vector<string>> data = {
        {"piece-1_image_url", {"./images/" + latestPublishedArticle.getSlug()}},
        {"piece-1_image_text",
         {renderSpanText(latestPublishedArticle.getTitle(), 70)}},
        {"piece-1_article_link",
         {"/article_read_page/" +
          to_string(latestPublishedArticle.getArticleId())}},
        {"scopes", {html}},
    };

    try {
      data.insert({"piece-3-articleLink-1",
                   {"/article_read_page/" +
                    to_string(allRecord.at(0).getArticleId())}});
      data.insert({"piece-3-articleTitle-1", {allRecord.at(0).getTitle()}});

      data.insert({"piece-3-articleLink-2",
                   {"/article_read_page/" +
                    to_string(allRecord.at(1).getArticleId())}});
      data.insert({"piece-3-articleTitle-2", {allRecord.at(1).getTitle()}});

      data.insert({"piece-3-articleLink-3",
                   {"/article_read_page/" +
                    to_string(allRecord.at(2).getArticleId())}});
      data.insert({"piece-3-articleTitle-3", {allRecord.at(2).getTitle()}});

      data.insert({"piece-3-articleLink-4",
                   {"/article_read_page/" +
                    to_string(allRecord.at(3).getArticleId())}});
      data.insert({"piece-3-articleTitle-4", {allRecord.at(3).getTitle()}});

      data.insert({"piece-3-articleLink-5",
                   {"/article_read_page/" +
                    to_string(allRecord.at(4).getArticleId())}});
      data.insert({"piece-3-articleTitle-5", {allRecord.at(4).getTitle()}});

      data.insert({"piece-3-articleLink-6",
                   {"/article_read_page/" +
                    to_string(allRecord.at(5).getArticleId())}});
      data.insert({"piece-3-articleTitle-6", {allRecord.at(5).getTitle()}});

    } catch (const exception &e) {
    }

    res.setBody(TemplateRenderer::render(templatePath, data));
  });

  router.addDynamicRoute(
      "/article_read_page/<article_id>",
      [](HttpRequest &req, HttpResponse &res) {
        try {
          int article_id = std::stoi(req.getPathParams().at(0));

          res.setStatus(200);
          res.addHeader("Content-Type", "text/html; charset=utf-8");
          res.setBody(loadFileContent(
              "./views/normal/article_read_page/html/index.html"));

          articlesTable.incrementViews(article_id);
        } catch (const exception &e) {
          res.setStatus(303); // redirect
          res.addHeader("Location", "/");
          return;
        }
      });

  router.addDynamicRoute("/category_page/<category_name>", [](HttpRequest &req,
                                                              HttpResponse
                                                                  &res) {
    try {
      string category_name = req.getPathParams().at(0);
      vector<ArticleRecord> articles;
      string menuButtons;

      if (category_name == "all") {
        articles = articlesTable.getAll();

        menuButtons = R"(
        <div class="list">
          <a href="/category_page/all"
            ><div class="red-border selected-category">Усі</div></a
          >
          <a href="/category_page/weather"
            ><div class="red-border">Погода</div></a
          >
          <a href="/category_page/anime"><div class="red-border">Аніме</div></a>
          <a href="/category_page/science"
            ><div class="red-border">Наука</div></a
          >
          <a href="/category_page/it"
            ><div class="red-border">Інформаційні технологїї</div></a
          >
          <a href="/category_page/community"
            ><div class="red-border">Суспільство</div></a
          >
          <a href="/category_page/self-education"
            ><div class="red-border">Самоосвіта</div></a
          >
          <a href="/category_page/health"
            ><div class="red-border">Здоровʼє</div></a
          >
          <a href="/category_page/culture"
            ><div class="red-border">Культура</div></a
          >
          <a href="/category_page/new"><div class="red-border">Новинки</div></a>
        </div>
        )";
      } else if (category_name == "weather") {
        articles = articlesTable.getAllRecordsByCategory("Погода");

        menuButtons = R"(
        <div class="list">
          <a href="/category_page/weather"
            ><div class="red-border selected-category">Погода</div></a
          >
          <a href="/category_page/all"
            ><div class="red-border">Усі</div></a
          >
          <a href="/category_page/anime"><div class="red-border">Аніме</div></a>
          <a href="/category_page/science"
            ><div class="red-border">Наука</div></a
          >
          <a href="/category_page/it"
            ><div class="red-border">Інформаційні технологїї</div></a
          >
          <a href="/category_page/community"
            ><div class="red-border">Суспільство</div></a
          >
          <a href="/category_page/self-education"
            ><div class="red-border">Самоосвіта</div></a
          >
          <a href="/category_page/health"
            ><div class="red-border">Здоровʼє</div></a
          >
          <a href="/category_page/culture"
            ><div class="red-border">Культура</div></a
          >
          <a href="/category_page/new"><div class="red-border">Новинки</div></a>
        </div>
        )";
      } else if (category_name == "anime") {
        articles = articlesTable.getAllRecordsByCategory("Аніме");

        menuButtons = R"(
        <div class="list">
        <a href="/category_page/anime"><div class="red-border selected-category">Аніме</div></a>
          <a href="/category_page/all"
            ><div class="red-border">Усі</div></a
          >
          <a href="/category_page/weather"
            ><div class="red-border">Погода</div></a
          >
          <a href="/category_page/science"
            ><div class="red-border">Наука</div></a
          >
          <a href="/category_page/it"
            ><div class="red-border">Інформаційні технологїї</div></a
          >
          <a href="/category_page/community"
            ><div class="red-border">Суспільство</div></a
          >
          <a href="/category_page/self-education"
            ><div class="red-border">Самоосвіта</div></a
          >
          <a href="/category_page/health"
            ><div class="red-border">Здоровʼє</div></a
          >
          <a href="/category_page/culture"
            ><div class="red-border">Культура</div></a
          >
          <a href="/category_page/new"><div class="red-border">Новинки</div></a>
        </div>
        )";
      } else if (category_name == "science") {
        articles = articlesTable.getAllRecordsByCategory("Наука");

        menuButtons = R"(
        <div class="list">
        <a href="/category_page/science"
            ><div class="red-border selected-category">Наука</div></a
          >
          <a href="/category_page/all"
            ><div class="red-border">Усі</div></a
          >
          <a href="/category_page/weather"
            ><div class="red-border">Погода</div></a
          >
          <a href="/category_page/anime"><div class="red-border">Аніме</div></a>
          <a href="/category_page/it"
            ><div class="red-border">Інформаційні технологїї</div></a
          >
          <a href="/category_page/community"
            ><div class="red-border">Суспільство</div></a
          >
          <a href="/category_page/self-education"
            ><div class="red-border">Самоосвіта</div></a
          >
          <a href="/category_page/health"
            ><div class="red-border">Здоровʼє</div></a
          >
          <a href="/category_page/culture"
            ><div class="red-border">Культура</div></a
          >
          <a href="/category_page/new"><div class="red-border">Новинки</div></a>
        </div>
        )";
      } else if (category_name == "it") {
        articles =
            articlesTable.getAllRecordsByCategory("Інформаційні технології");

        menuButtons = R"(
        <div class="list">
          <a href="/category_page/it"
            ><div class="red-border selected-category">Інформаційні технологїї</div></a
          >
          <a href="/category_page/all"
            ><div class="red-border">Усі</div></a
          >
          <a href="/category_page/weather"
            ><div class="red-border">Погода</div></a
          >
          <a href="/category_page/anime"><div class="red-border">Аніме</div></a>
          <a href="/category_page/science"
            ><div class="red-border">Наука</div></a
          >
          <a href="/category_page/community"
            ><div class="red-border">Суспільство</div></a
          >
          <a href="/category_page/self-education"
            ><div class="red-border">Самоосвіта</div></a
          >
          <a href="/category_page/health"
            ><div class="red-border">Здоровʼє</div></a
          >
          <a href="/category_page/culture"
            ><div class="red-border">Культура</div></a
          >
          <a href="/category_page/new"><div class="red-border">Новинки</div></a>
        </div>
        )";
      } else if (category_name == "community") {
        articles = articlesTable.getAllRecordsByCategory("Суспільство");

        menuButtons = R"(
        <div class="list">
        <a href="/category_page/community"
            ><div class="red-border selected-category">Суспільство</div></a
          >
          <a href="/category_page/all"
            ><div class="red-border">Усі</div></a
          >
          <a href="/category_page/weather"
            ><div class="red-border">Погода</div></a
          >
          <a href="/category_page/anime"><div class="red-border">Аніме</div></a>
          <a href="/category_page/science"
            ><div class="red-border">Наука</div></a
          >
          <a href="/category_page/it"
            ><div class="red-border">Інформаційні технологїї</div></a
          >
          <a href="/category_page/self-education"
            ><div class="red-border">Самоосвіта</div></a
          >
          <a href="/category_page/health"
            ><div class="red-border">Здоровʼє</div></a
          >
          <a href="/category_page/culture"
            ><div class="red-border">Культура</div></a
          >
          <a href="/category_page/new"><div class="red-border">Новинки</div></a>
        </div>
        )";
      } else if (category_name == "self-education") {
        articles = articlesTable.getAllRecordsByCategory("Самоосвіта");

        menuButtons = R"(
        <div class="list">
        <a href="/category_page/self-education"
            ><div class="red-border selected-category">Самоосвіта</div></a
          >
          <a href="/category_page/all"
            ><div class="red-border">Усі</div></a
          >
          <a href="/category_page/weather"
            ><div class="red-border">Погода</div></a
          >
          <a href="/category_page/anime"><div class="red-border">Аніме</div></a>
          <a href="/category_page/science"
            ><div class="red-border">Наука</div></a
          >
          <a href="/category_page/it"
            ><div class="red-border">Інформаційні технологїї</div></a
          >
          <a href="/category_page/community"
            ><div class="red-border">Суспільство</div></a
          >
          <a href="/category_page/health"
            ><div class="red-border">Здоровʼє</div></a
          >
          <a href="/category_page/culture"
            ><div class="red-border">Культура</div></a
          >
          <a href="/category_page/new"><div class="red-border">Новинки</div></a>
        </div>
        )";
      } else if (category_name == "health") {
        articles = articlesTable.getAllRecordsByCategory("Здоров’я");

        menuButtons = R"(
        <div class="list">
        <a href="/category_page/health"
            ><div class="red-border selected-category">Здоровʼє</div></a
          >
          <a href="/category_page/all"
            ><div class="red-border">Усі</div></a
          >
          <a href="/category_page/weather"
            ><div class="red-border">Погода</div></a
          >
          <a href="/category_page/anime"><div class="red-border">Аніме</div></a>
          <a href="/category_page/science"
            ><div class="red-border">Наука</div></a
          >
          <a href="/category_page/it"
            ><div class="red-border">Інформаційні технологїї</div></a
          >
          <a href="/category_page/community"
            ><div class="red-border">Суспільство</div></a
          >
          <a href="/category_page/self-education"
            ><div class="red-border">Самоосвіта</div></a
          >
          <a href="/category_page/culture"
            ><div class="red-border">Культура</div></a
          >
          <a href="/category_page/new"><div class="red-border">Новинки</div></a>
        </div>
        )";
      } else if (category_name == "culture") {
        articles = articlesTable.getAllRecordsByCategory("Культура");

        menuButtons = R"(
        <div class="list">
        <a href="/category_page/culture"
            ><div class="red-border selected-category">Культура</div></a
          >
          <a href="/category_page/all"
            ><div class="red-border">Усі</div></a
          >
          <a href="/category_page/weather"
            ><div class="red-border">Погода</div></a
          >
          <a href="/category_page/anime"><div class="red-border">Аніме</div></a>
          <a href="/category_page/science"
            ><div class="red-border">Наука</div></a
          >
          <a href="/category_page/it"
            ><div class="red-border">Інформаційні технологїї</div></a
          >
          <a href="/category_page/community"
            ><div class="red-border">Суспільство</div></a
          >
          <a href="/category_page/self-education"
            ><div class="red-border">Самоосвіта</div></a
          >
          <a href="/category_page/health"
            ><div class="red-border">Здоровʼє</div></a
          >
          <a href="/category_page/new"><div class="red-border">Новинки</div></a>
        </div>
        )";
      } else if (category_name == "new") {
        res.setStatus(303); // redirect
        res.addHeader("Location", "/");
        return;
      }

      cout << "Size Find category: " << articles.size() << endl;

      // плохой алгоритм

      string html;
      int articlesSize = articles.size();
      int templateVariable = 1;

      for (size_t i = 0; i < articlesSize; ++i, ++templateVariable) {
        if (templateVariable == 1) {
          if (i + 4 < articlesSize) {
            std::ostringstream stream;
            stream
                << R"(
        <div class="scope">
          <div class="bottom">
            <div class="mini-column">
              <a href="/article_read_page/)"
                << articles.at(i).getArticleId() << R"("
                ><div class="bild-bord">
                  <div class="image-container">
                    <img src=")"
                << "/images/" << articles.at(i).getSlug()
                << R"(" class="image" />
                  </div>

                  <div class="title">)"
                << articles.at(i).getTitle() << R"(</div>

                  <div class="description">)"
                << nlohmann::json::parse(
                       articles.at(i).getSeoDescription())["ops"][0]["insert"]
                       .get<string>()
                << R"(</div>
                </div></a
              >
              <a href="/article_read_page/)"
                << articles.at(++i).getArticleId() << R"(">
                <div class="bild-bord">
                  <div class="image-container">
                    <img src=")"
                << "/images/" << articles.at(i).getSlug()
                << R"(" class="image" />
                  </div>

                  <div class="title">)"
                << articles.at(i).getTitle() << R"(</div>

                  <div class="description">)"
                << nlohmann::json::parse(
                       articles.at(i).getSeoDescription())["ops"][0]["insert"]
                       .get<string>()
                << R"(</div>
                </div></a
              >
            </div>

            <div class="big-column">
              <a href="/article_read_page/)"
                << articles.at(++i).getArticleId() << R"(">
                <div class="bild-bord">
                  <div class="title">)"
                << articles.at(i).getTitle() << R"(</div>

                  <div class="description">)"
                << nlohmann::json::parse(
                       articles.at(i).getSeoDescription())["ops"][0]["insert"]
                       .get<string>()
                << R"(</div>

                  <div class="image-container">
                    <img src=")"
                << "/images/" << articles.at(i).getSlug()
                << R"(" class="image" />
                  </div>
                </div>
              </a>
            </div>

            <div class="mini-column">
              <a href="/article_read_page/)"
                << articles.at(++i).getArticleId() << R"(">
                <div class="bild-bord">
                  <div class="image-container">
                    <img src=")"
                << "/images/" << articles.at(i).getSlug()
                << R"(" class="image" />
                  </div>

                  <div class="title">)"
                << articles.at(i).getTitle() << R"(</div>

                  <div class="description">)"
                << nlohmann::json::parse(
                       articles.at(i).getSeoDescription())["ops"][0]["insert"]
                       .get<string>()
                << R"(</div>
                </div></a
              >

              <a href="/article_read_page/)"
                << articles.at(++i).getArticleId() << R"(">
                <div class="bild-bord">
                  <div class="image-container">
                    <img src=")"
                << "/images/" << articles.at(i).getSlug()
                << R"(" class="image" />
                  </div>

                  <div class="title">)"
                << articles.at(i).getTitle() << R"(</div>

                  <div class="description">)"
                << nlohmann::json::parse(
                       articles.at(i).getSeoDescription())["ops"][0]["insert"]
                       .get<string>()
                << R"(</div>
                </div></a
              >
            </div>
          </div>
        </div>
              )";

            html += stream.str();
          } else if (i + 3 < articlesSize) {
            std::ostringstream stream;
            stream
                << R"(
        <div class="scope">
          <div class="bottom">
            <div class="mini-column">
              <a href="/article_read_page/)"
                << articles.at(i).getArticleId() << R"("
                ><div class="bild-bord">
                  <div class="image-container">
                    <img src=")"
                << "/images/" << articles.at(i).getSlug()
                << R"(" class="image" />
                  </div>

                  <div class="title">)"
                << articles.at(i).getTitle() << R"(</div>

                  <div class="description">)"
                << nlohmann::json::parse(
                       articles.at(i).getSeoDescription())["ops"][0]["insert"]
                       .get<string>()
                << R"(</div>
                </div></a
              >
              <a href="/article_read_page/)"
                << articles.at(++i).getArticleId() << R"(">
                <div class="bild-bord">
                  <div class="image-container">
                    <img src=")"
                << "/images/" << articles.at(i).getSlug()
                << R"(" class="image" />
                  </div>

                  <div class="title">)"
                << articles.at(i).getTitle() << R"(</div>

                  <div class="description">)"
                << nlohmann::json::parse(
                       articles.at(i).getSeoDescription())["ops"][0]["insert"]
                       .get<string>()
                << R"(</div>
                </div></a
              >
            </div>

            <div class="big-column">
              <a href="/article_read_page/)"
                << articles.at(++i).getArticleId() << R"(">
                <div class="bild-bord">
                  <div class="title">)"
                << articles.at(i).getTitle() << R"(</div>

                  <div class="description">)"
                << nlohmann::json::parse(
                       articles.at(i).getSeoDescription())["ops"][0]["insert"]
                       .get<string>()
                << R"(</div>

                  <div class="image-container">
                    <img src=")"
                << "/images/" << articles.at(i).getSlug()
                << R"(" class="image" />
                  </div>
                </div>
              </a>
            </div>

            <div class="mini-column">
              <a href="/article_read_page/)"
                << articles.at(++i).getArticleId() << R"(">
                <div class="bild-bord">
                  <div class="image-container">
                    <img src=")"
                << "/images/" << articles.at(i).getSlug()
                << R"(" class="image" />
                  </div>

                  <div class="title">)"
                << articles.at(i).getTitle() << R"(</div>

                  <div class="description">)"
                << nlohmann::json::parse(
                       articles.at(i).getSeoDescription())["ops"][0]["insert"]
                       .get<string>()
                << R"(</div>
                </div></a
              >
            </div>
          </div>
        </div>
              )";

            html += stream.str();
          } else if (i + 2 < articlesSize) {
            std::ostringstream stream;
            stream
                << R"(
        <div class="scope">
          <div class="bottom">
            <div class="mini-column">
              <a href="/article_read_page/)"
                << articles.at(i).getArticleId() << R"("
                ><div class="bild-bord">
                  <div class="image-container">
                    <img src=")"
                << "/images/" << articles.at(i).getSlug()
                << R"(" class="image" />
                  </div>

                  <div class="title">)"
                << articles.at(i).getTitle() << R"(</div>

                  <div class="description">)"
                << nlohmann::json::parse(
                       articles.at(i).getSeoDescription())["ops"][0]["insert"]
                       .get<string>()
                << R"(</div>
                </div></a
              >
              <a href="/article_read_page/)"
                << articles.at(++i).getArticleId() << R"(">
                <div class="bild-bord">
                  <div class="image-container">
                    <img src=")"
                << "/images/" << articles.at(i).getSlug()
                << R"(" class="image" />
                  </div>

                  <div class="title">)"
                << articles.at(i).getTitle() << R"(</div>

                  <div class="description">)"
                << nlohmann::json::parse(
                       articles.at(i).getSeoDescription())["ops"][0]["insert"]
                       .get<string>()
                << R"(</div>
                </div></a
              >
            </div>

            <div class="big-column">
              <a href="/article_read_page/)"
                << articles.at(++i).getArticleId() << R"(">
                <div class="bild-bord">
                  <div class="title">)"
                << articles.at(i).getTitle() << R"(</div>

                  <div class="description">)"
                << nlohmann::json::parse(
                       articles.at(i).getSeoDescription())["ops"][0]["insert"]
                       .get<string>()
                << R"(</div>

                  <div class="image-container">
                    <img src=")"
                << "/images/" << articles.at(i).getSlug()
                << R"(" class="image" />
                  </div>
                </div>
              </a>
            </div>
          </div>
        </div>
              )";

            html += stream.str();
          } else if (i + 1 < articlesSize) {
            std::ostringstream stream;
            stream
                << R"(
        <div class="scope">
          <div class="bottom">
            <div class="mini-column">
              <a href="/article_read_page/)"
                << articles.at(i).getArticleId() << R"("
                ><div class="bild-bord">
                  <div class="image-container">
                    <img src=")"
                << "/images/" << articles.at(i).getSlug()
                << R"(" class="image" />
                  </div>

                  <div class="title">)"
                << articles.at(i).getTitle() << R"(</div>

                  <div class="description">)"
                << nlohmann::json::parse(
                       articles.at(i).getSeoDescription())["ops"][0]["insert"]
                       .get<string>()
                << R"(</div>
                </div></a
              >
              <a href="/article_read_page/)"
                << articles.at(++i).getArticleId() << R"(">
                <div class="bild-bord">
                  <div class="image-container">
                    <img src=")"
                << "/images/" << articles.at(i).getSlug()
                << R"(" class="image" />
                  </div>

                  <div class="title">)"
                << articles.at(i).getTitle() << R"(</div>

                  <div class="description">)"
                << nlohmann::json::parse(
                       articles.at(i).getSeoDescription())["ops"][0]["insert"]
                       .get<string>()
                << R"(</div>
                </div></a
              >
            </div>
          </div>
        </div>
              )";

            html += stream.str();
          } else if (i == articlesSize - 1) {
            std::ostringstream stream;
            stream
                << R"(
        <div class="scope">
          <div class="bottom">
            <div class="mini-column">
              <a href="/article_read_page/)"
                << articles.at(i).getArticleId() << R"("
                ><div class="bild-bord">
                  <div class="image-container">
                    <img src=")"
                << "/images/" << articles.at(i).getSlug()
                << R"(" class="image" />
                  </div>

                  <div class="title">)"
                << articles.at(i).getTitle() << R"(</div>

                  <div class="description">)"
                << nlohmann::json::parse(
                       articles.at(i).getSeoDescription())["ops"][0]["insert"]
                       .get<string>()
                << R"(</div>
                </div></a
              >
            </div>
          </div>
        </div>
              )";

            html += stream.str();
          }
        } else if (templateVariable == 2) {
          if (i + 4 < articlesSize) {
            std::ostringstream stream;
            stream
                << R"(
        <div class="scope">
          <div class="bottom">
          <div class="big-column">
              <a href="/article_read_page/)"
                << articles.at(i).getArticleId() << R"(">
                <div class="bild-bord">
                  <div class="title">)"
                << articles.at(i).getTitle() << R"(</div>

                  <div class="description">)"
                << nlohmann::json::parse(
                       articles.at(i).getSeoDescription())["ops"][0]["insert"]
                       .get<string>()
                << R"(</div>

                  <div class="image-container">
                    <img src=")"
                << "/images/" << articles.at(i).getSlug()
                << R"(" class="image" />
                  </div>
                </div>
              </a>
            </div>
            <div class="mini-column">
              <a href="/article_read_page/)"
                << articles.at(++i).getArticleId() << R"("
                ><div class="bild-bord">
                  <div class="image-container">
                    <img src=")"
                << "/images/" << articles.at(i).getSlug()
                << R"(" class="image" />
                  </div>

                  <div class="title">)"
                << articles.at(i).getTitle() << R"(</div>

                  <div class="description">)"
                << nlohmann::json::parse(
                       articles.at(i).getSeoDescription())["ops"][0]["insert"]
                       .get<string>()
                << R"(</div>
                </div></a
              >
              <a href="/article_read_page/)"
                << articles.at(++i).getArticleId() << R"(">
                <div class="bild-bord">
                  <div class="image-container">
                    <img src=")"
                << "/images/" << articles.at(i).getSlug()
                << R"(" class="image" />
                  </div>

                  <div class="title">)"
                << articles.at(i).getTitle() << R"(</div>

                  <div class="description">)"
                << nlohmann::json::parse(
                       articles.at(i).getSeoDescription())["ops"][0]["insert"]
                       .get<string>()
                << R"(</div>
                </div></a
              >
            </div>

            <div class="mini-column">
              <a href="/article_read_page/)"
                << articles.at(++i).getArticleId() << R"(">
                <div class="bild-bord">
                  <div class="image-container">
                    <img src=")"
                << "/images/" << articles.at(i).getSlug()
                << R"(" class="image" />
                  </div>

                  <div class="title">)"
                << articles.at(i).getTitle() << R"(</div>

                  <div class="description">)"
                << nlohmann::json::parse(
                       articles.at(i).getSeoDescription())["ops"][0]["insert"]
                       .get<string>()
                << R"(</div>
                </div></a
              >

              <a href="/article_read_page/)"
                << articles.at(++i).getArticleId() << R"(">
                <div class="bild-bord">
                  <div class="image-container">
                    <img src=")"
                << "/images/" << articles.at(i).getSlug()
                << R"(" class="image" />
                  </div>

                  <div class="title">)"
                << articles.at(i).getTitle() << R"(</div>

                  <div class="description">)"
                << nlohmann::json::parse(
                       articles.at(i).getSeoDescription())["ops"][0]["insert"]
                       .get<string>()
                << R"(</div>
                </div></a
              >
            </div>
          </div>
        </div>
              )";

            html += stream.str();
          } else if (i + 3 < articlesSize) {
            std::ostringstream stream;
            stream
                << R"(
        <div class="scope">
          <div class="bottom">
          <div class="big-column">
              <a href="/article_read_page/)"
                << articles.at(i).getArticleId() << R"(">
                <div class="bild-bord">
                  <div class="title">)"
                << articles.at(i).getTitle() << R"(</div>

                  <div class="description">)"
                << nlohmann::json::parse(
                       articles.at(i).getSeoDescription())["ops"][0]["insert"]
                       .get<string>()
                << R"(</div>

                  <div class="image-container">
                    <img src=")"
                << "/images/" << articles.at(i).getSlug()
                << R"(" class="image" />
                  </div>
                </div>
              </a>
            </div>
            <div class="mini-column">
              <a href="/article_read_page/)"
                << articles.at(++i).getArticleId() << R"("
                ><div class="bild-bord">
                  <div class="image-container">
                    <img src=")"
                << "/images/" << articles.at(i).getSlug()
                << R"(" class="image" />
                  </div>

                  <div class="title">)"
                << articles.at(i).getTitle() << R"(</div>

                  <div class="description">)"
                << nlohmann::json::parse(
                       articles.at(i).getSeoDescription())["ops"][0]["insert"]
                       .get<string>()
                << R"(</div>
                </div></a
              >
              <a href="/article_read_page/)"
                << articles.at(++i).getArticleId() << R"(">
                <div class="bild-bord">
                  <div class="image-container">
                    <img src=")"
                << "/images/" << articles.at(i).getSlug()
                << R"(" class="image" />
                  </div>

                  <div class="title">)"
                << articles.at(i).getTitle() << R"(</div>

                  <div class="description">)"
                << nlohmann::json::parse(
                       articles.at(i).getSeoDescription())["ops"][0]["insert"]
                       .get<string>()
                << R"(</div>
                </div></a
              >
            </div>

            <div class="mini-column">
              <a href="/article_read_page/)"
                << articles.at(++i).getArticleId() << R"(">
                <div class="bild-bord">
                  <div class="image-container">
                    <img src=")"
                << "/images/" << articles.at(i).getSlug()
                << R"(" class="image" />
                  </div>

                  <div class="title">)"
                << articles.at(i).getTitle() << R"(</div>

                  <div class="description">)"
                << nlohmann::json::parse(
                       articles.at(i).getSeoDescription())["ops"][0]["insert"]
                       .get<string>()
                << R"(</div>
                </div></a
              >
            </div>
          </div>
        </div>
              )";

            html += stream.str();
          } else if (i + 2 < articlesSize) {
            std::ostringstream stream;
            stream
                << R"(
        <div class="scope">
          <div class="bottom">
          <div class="big-column">
              <a href="/article_read_page/)"
                << articles.at(++i).getArticleId() << R"(">
                <div class="bild-bord">
                  <div class="title">)"
                << articles.at(i).getTitle() << R"(</div>

                  <div class="description">)"
                << nlohmann::json::parse(
                       articles.at(i).getSeoDescription())["ops"][0]["insert"]
                       .get<string>()
                << R"(</div>

                  <div class="image-container">
                    <img src=")"
                << "/images/" << articles.at(i).getSlug()
                << R"(" class="image" />
                  </div>
                </div>
              </a>
            </div>
            <div class="mini-column">
              <a href="/article_read_page/)"
                << articles.at(++i).getArticleId() << R"("
                ><div class="bild-bord">
                  <div class="image-container">
                    <img src=")"
                << "/images/" << articles.at(i).getSlug()
                << R"(" class="image" />
                  </div>

                  <div class="title">)"
                << articles.at(i).getTitle() << R"(</div>

                  <div class="description">)"
                << nlohmann::json::parse(
                       articles.at(i).getSeoDescription())["ops"][0]["insert"]
                       .get<string>()
                << R"(</div>
                </div></a
              >
              <a href="/article_read_page/)"
                << articles.at(++i).getArticleId() << R"(">
                <div class="bild-bord">
                  <div class="image-container">
                    <img src=")"
                << "/images/" << articles.at(i).getSlug()
                << R"(" class="image" />
                  </div>

                  <div class="title">)"
                << articles.at(i).getTitle() << R"(</div>

                  <div class="description">)"
                << nlohmann::json::parse(
                       articles.at(i).getSeoDescription())["ops"][0]["insert"]
                       .get<string>()
                << R"(</div>
                </div></a
              >
            </div>
          </div>
        </div>
              )";

            html += stream.str();
          } else if (i + 1 < articlesSize) {
            std::ostringstream stream;
            stream
                << R"(
        <div class="scope">
          <div class="bottom">
            <div class="mini-column">
              <a href="/article_read_page/)"
                << articles.at(++i).getArticleId() << R"("
                ><div class="bild-bord">
                  <div class="image-container">
                    <img src=")"
                << "/images/" << articles.at(i).getSlug()
                << R"(" class="image" />
                  </div>

                  <div class="title">)"
                << articles.at(i).getTitle() << R"(</div>

                  <div class="description">)"
                << nlohmann::json::parse(
                       articles.at(i).getSeoDescription())["ops"][0]["insert"]
                       .get<string>()
                << R"(</div>
                </div></a
              >
              <a href="/article_read_page/)"
                << articles.at(++i).getArticleId() << R"(">
                <div class="bild-bord">
                  <div class="image-container">
                    <img src=")"
                << "/images/" << articles.at(i).getSlug()
                << R"(" class="image" />
                  </div>

                  <div class="title">)"
                << articles.at(i).getTitle() << R"(</div>

                  <div class="description">)"
                << nlohmann::json::parse(
                       articles.at(i).getSeoDescription())["ops"][0]["insert"]
                       .get<string>()
                << R"(</div>
                </div></a
              >
            </div>
          </div>
        </div>
              )";

            html += stream.str();
          } else if (i == articlesSize - 1) {
            std::ostringstream stream;
            stream
                << R"(
        <div class="scope">
          <div class="bottom">
            <div class="mini-column">
              <a href="/article_read_page/)"
                << articles.at(i).getArticleId() << R"("
                ><div class="bild-bord">
                  <div class="image-container">
                    <img src=")"
                << "/images/" << articles.at(i).getSlug()
                << R"(" class="image" />
                  </div>

                  <div class="title">)"
                << articles.at(i).getTitle() << R"(</div>

                  <div class="description">)"
                << nlohmann::json::parse(
                       articles.at(i).getSeoDescription())["ops"][0]["insert"]
                       .get<string>()
                << R"(</div>
                </div></a
              >
            </div>
          </div>
        </div>
              )";

            html += stream.str();
          }
        } else if (templateVariable == 3) {
          if (i + 4 < articlesSize) {
            std::ostringstream stream;
            stream
                << R"(
        <div class="scope">
          <div class="bottom">
            <div class="mini-column">
              <a href="/article_read_page/)"
                << articles.at(i).getArticleId() << R"("
                ><div class="bild-bord">
                  <div class="image-container">
                    <img src=")"
                << "/images/" << articles.at(i).getSlug()
                << R"(" class="image" />
                  </div>

                  <div class="title">)"
                << articles.at(i).getTitle() << R"(</div>

                  <div class="description">)"
                << nlohmann::json::parse(
                       articles.at(i).getSeoDescription())["ops"][0]["insert"]
                       .get<string>()
                << R"(</div>
                </div></a
              >
              <a href="/article_read_page/)"
                << articles.at(++i).getArticleId() << R"(">
                <div class="bild-bord">
                  <div class="image-container">
                    <img src=")"
                << "/images/" << articles.at(i).getSlug()
                << R"(" class="image" />
                  </div>

                  <div class="title">)"
                << articles.at(i).getTitle() << R"(</div>

                  <div class="description">)"
                << nlohmann::json::parse(
                       articles.at(i).getSeoDescription())["ops"][0]["insert"]
                       .get<string>()
                << R"(</div>
                </div></a
              >
            </div>

            <div class="mini-column">
              <a href="/article_read_page/)"
                << articles.at(++i).getArticleId() << R"(">
                <div class="bild-bord">
                  <div class="image-container">
                    <img src=")"
                << "/images/" << articles.at(i).getSlug()
                << R"(" class="image" />
                  </div>

                  <div class="title">)"
                << articles.at(i).getTitle() << R"(</div>

                  <div class="description">)"
                << nlohmann::json::parse(
                       articles.at(i).getSeoDescription())["ops"][0]["insert"]
                       .get<string>()
                << R"(</div>
                </div></a
              >

              <a href="/article_read_page/)"
                << articles.at(++i).getArticleId() << R"(">
                <div class="bild-bord">
                  <div class="image-container">
                    <img src=")"
                << "/images/" << articles.at(i).getSlug()
                << R"(" class="image" />
                  </div>

                  <div class="title">)"
                << articles.at(i).getTitle() << R"(</div>

                  <div class="description">)"
                << nlohmann::json::parse(
                       articles.at(i).getSeoDescription())["ops"][0]["insert"]
                       .get<string>()
                << R"(</div>
                </div></a
              >
            </div>

            <div class="big-column">
              <a href="/article_read_page/)"
                << articles.at(++i).getArticleId() << R"(">
                <div class="bild-bord">
                  <div class="title">)"
                << articles.at(i).getTitle() << R"(</div>

                  <div class="description">)"
                << nlohmann::json::parse(
                       articles.at(i).getSeoDescription())["ops"][0]["insert"]
                       .get<string>()
                << R"(</div>

                  <div class="image-container">
                    <img src=")"
                << "/images/" << articles.at(i).getSlug()
                << R"(" class="image" />
                  </div>
                </div>
              </a>
            </div>
          </div>
        </div>
              )";

            html += stream.str();
          } else if (i + 3 < articlesSize) {
            std::ostringstream stream;
            stream
                << R"(
        <div class="scope">
          <div class="bottom">
            <div class="mini-column">
              <a href="/article_read_page/)"
                << articles.at(i).getArticleId() << R"("
                ><div class="bild-bord">
                  <div class="image-container">
                    <img src=")"
                << "/images/" << articles.at(i).getSlug()
                << R"(" class="image" />
                  </div>

                  <div class="title">)"
                << articles.at(i).getTitle() << R"(</div>

                  <div class="description">)"
                << nlohmann::json::parse(
                       articles.at(i).getSeoDescription())["ops"][0]["insert"]
                       .get<string>()
                << R"(</div>
                </div></a
              >
              <a href="/article_read_page/)"
                << articles.at(++i).getArticleId() << R"(">
                <div class="bild-bord">
                  <div class="image-container">
                    <img src=")"
                << "/images/" << articles.at(i).getSlug()
                << R"(" class="image" />
                  </div>

                  <div class="title">)"
                << articles.at(i).getTitle() << R"(</div>

                  <div class="description">)"
                << nlohmann::json::parse(
                       articles.at(i).getSeoDescription())["ops"][0]["insert"]
                       .get<string>()
                << R"(</div>
                </div></a
              >
            </div>

            <div class="mini-column">
              <a href="/article_read_page/)"
                << articles.at(++i).getArticleId() << R"(">
                <div class="bild-bord">
                  <div class="image-container">
                    <img src=")"
                << "/images/" << articles.at(i).getSlug()
                << R"(" class="image" />
                  </div>

                  <div class="title">)"
                << articles.at(i).getTitle() << R"(</div>

                  <div class="description">)"
                << nlohmann::json::parse(
                       articles.at(i).getSeoDescription())["ops"][0]["insert"]
                       .get<string>()
                << R"(</div>
                </div></a
              >
            </div>

            <div class="big-column">
              <a href="/article_read_page/)"
                << articles.at(++i).getArticleId() << R"(">
                <div class="bild-bord">
                  <div class="title">)"
                << articles.at(i).getTitle() << R"(</div>

                  <div class="description">)"
                << nlohmann::json::parse(
                       articles.at(i).getSeoDescription())["ops"][0]["insert"]
                       .get<string>()
                << R"(</div>

                  <div class="image-container">
                    <img src=")"
                << "/images/" << articles.at(i).getSlug()
                << R"(" class="image" />
                  </div>
                </div>
              </a>
            </div>
          </div>
        </div>
              )";

            html += stream.str();
          } else if (i + 2 < articlesSize) {
            std::ostringstream stream;
            stream
                << R"(
        <div class="scope">
          <div class="bottom">
            <div class="mini-column">
              <a href="/article_read_page/)"
                << articles.at(i).getArticleId() << R"("
                ><div class="bild-bord">
                  <div class="image-container">
                    <img src=")"
                << "/images/" << articles.at(i).getSlug()
                << R"(" class="image" />
                  </div>

                  <div class="title">)"
                << articles.at(i).getTitle() << R"(</div>

                  <div class="description">)"
                << nlohmann::json::parse(
                       articles.at(i).getSeoDescription())["ops"][0]["insert"]
                       .get<string>()
                << R"(</div>
                </div></a
              >
              <a href="/article_read_page/)"
                << articles.at(++i).getArticleId() << R"(">
                <div class="bild-bord">
                  <div class="image-container">
                    <img src=")"
                << "/images/" << articles.at(i).getSlug()
                << R"(" class="image" />
                  </div>

                  <div class="title">)"
                << articles.at(i).getTitle() << R"(</div>

                  <div class="description">)"
                << nlohmann::json::parse(
                       articles.at(i).getSeoDescription())["ops"][0]["insert"]
                       .get<string>()
                << R"(</div>
                </div></a
              >
            </div>

            <div class="big-column">
              <a href="/article_read_page/)"
                << articles.at(++i).getArticleId() << R"(">
                <div class="bild-bord">
                  <div class="title">)"
                << articles.at(i).getTitle() << R"(</div>

                  <div class="description">)"
                << nlohmann::json::parse(
                       articles.at(i).getSeoDescription())["ops"][0]["insert"]
                       .get<string>()
                << R"(</div>

                  <div class="image-container">
                    <img src=")"
                << "/images/" << articles.at(i).getSlug()
                << R"(" class="image" />
                  </div>
                </div>
              </a>
            </div>
          </div>
        </div>
              )";

            html += stream.str();
          } else if (i + 1 < articlesSize) {
            std::ostringstream stream;
            stream
                << R"(
        <div class="scope">
          <div class="bottom">
            <div class="mini-column">
              <a href="/article_read_page/)"
                << articles.at(i).getArticleId() << R"("
                ><div class="bild-bord">
                  <div class="image-container">
                    <img src=")"
                << "/images/" << articles.at(i).getSlug()
                << R"(" class="image" />
                  </div>

                  <div class="title">)"
                << articles.at(i).getTitle() << R"(</div>

                  <div class="description">)"
                << nlohmann::json::parse(
                       articles.at(i).getSeoDescription())["ops"][0]["insert"]
                       .get<string>()
                << R"(</div>
                </div></a
              >
              <a href="/article_read_page/)"
                << articles.at(++i).getArticleId() << R"(">
                <div class="bild-bord">
                  <div class="image-container">
                    <img src=")"
                << "/images/" << articles.at(i).getSlug()
                << R"(" class="image" />
                  </div>

                  <div class="title">)"
                << articles.at(i).getTitle() << R"(</div>

                  <div class="description">)"
                << nlohmann::json::parse(
                       articles.at(i).getSeoDescription())["ops"][0]["insert"]
                       .get<string>()
                << R"(</div>
                </div></a
              >
            </div>
          </div>
        </div>
              )";

            html += stream.str();
          } else if (i == articlesSize - 1) {
            std::ostringstream stream;
            stream
                << R"(
        <div class="scope">
          <div class="bottom">
            <div class="mini-column">
              <a href="/article_read_page/)"
                << articles.at(i).getArticleId() << R"("
                ><div class="bild-bord">
                  <div class="image-container">
                    <img src=")"
                << "/images/" << articles.at(i).getSlug()
                << R"(" class="image" />
                  </div>

                  <div class="title">)"
                << articles.at(i).getTitle() << R"(</div>

                  <div class="description">)"
                << nlohmann::json::parse(
                       articles.at(i).getSeoDescription())["ops"][0]["insert"]
                       .get<string>()
                << R"(</div>
                </div></a
              >
            </div>
          </div>
        </div>
              )";

            html += stream.str();
          }
          templateVariable = 1;
        }
      }

      unordered_map<string, vector<string>> data = {
          {"scopes", {html}}, {"menuButtons", {menuButtons}}};

      res.setStatus(200);
      res.addHeader("Content-Type", "text/html; charset=utf-8");
      res.setBody(TemplateRenderer::render(
          "./views/normal/category_page/html/index.html", data));

    } catch (const exception &e) {
      cout << endl << e.what() << endl;
      res.setStatus(303); // redirect
      res.addHeader("Location", "/");
      return;
    }
  });
}

// // router.addDynamicRoute("/company/<companyid>/user/<userid>",
// [](HttpRequest& request, HttpResponse& response) {
// //     string companyid = request.getPathParams().at(0);
// //     string userid = request.getPathParams().at(1);
// //     response.addHeader("Content-Type", "text/html; charset=utf-8");
// //     response.setBody("<h1>Company ID: " + companyid + ", User ID: "
// + userid + "</h1>");
// // });

// router.addStaticRoute("/", [](HttpRequest& request, HttpResponse&
// response)
// {
//     response.setStatus(200);
//     response.addHeader("Content-Type", "text/html; charset=utf-8");
//     response.setBody("main page");

//     // UserModel user;

//     // unordered_map<string, vector<string>> data = {
//     //     {"pageName", {"Главная страница"}},
//     //     {"newsList", {"Новость 1", "Новость 2", "Новость 3"}}
//     // };

//     //
//     response.setBody(TemplateRenderer::render("../src/views/index.html",
//     data));
// });

// router.addStaticRoute("/styles.css", [](HttpRequest& request,
// HttpResponse& response) {
//     response.setStatus(200);
//     response.addHeader("Content-Type", "text/css");

//     response.setBody(loadFileContent("../src/static/css/styles.css"));
// });

// router.addStaticRoute("/script.js", [](HttpRequest& request,
// HttpResponse& response) {
//     response.setStatus(200);
//     response.addHeader("Content-Type", "application/javascript");

//     response.setBody(loadFileContent("../src/static/js/script.js"));
// });

// router.addStaticRoute("/login/page", [](HttpRequest& request,
// HttpResponse& response) {
//     response.setStatus(200);
//     response.addHeader("Content-Type", "text/html");

//     response.setBody(loadFileContent("../src/views/index.html"));
// });

// router.addStaticRoute("/register", [](HttpRequest& request,
// HttpResponse& response) {
//     unordered_map<string, string> json = request.getJsonData();
//     auto posUsername = json.find("username");
//     auto posPassword = json.find("password");

//     // for(const auto& [key, value]: json) {
//     //     std::cout << key << std::endl;
//     //     std::cout << value << std::endl;
//     // }

//     if (posUsername == json.end() || posPassword == json.end()) {
//         response.setStatus(400);
//         response.addHeader("Content-Type", "application/json;
//         charset=utf-8"); response.setBody(R"({"message": "Неправильный
//         запрос на создание сессии"})"); return;
//     }

//     const string username = posUsername->second;
//     const string password = posPassword->second;

//     if (username == "Illia" && password == "1234") {
//         string userData = R"({
//             "username": "admin panel for Illia"
//         })";
//         string sessionID = sessionRepository.createSession(userData);

//         response.addCookie("session_id", sessionID);
//         response.setStatus(302);
//         response.addHeader("Location", "/admin/page");
//         // std::cout << response.getResponse() << std::endl;
//         return;
//     } else if (username == "Valera" && password == "5678") {
//         string userData = R"({
//             "username": "admin panel for Valera"
//         })";
//         string sessionID = sessionRepository.createSession(userData);

//         response.addCookie("session_id", sessionID);
//         response.setStatus(302);
//         response.addHeader("Location", "/admin/page");
//         return;
//     } else {
//         response.setStatus(401);
//         response.addHeader("Content-Type", "application/json;
//         charset=utf-8"); response.setBody(R"({"message": "Неправильный
//         логин или пароль"})"); return;
//     }
// });

// router.addStaticRoute("/admin/page", [](HttpRequest& request,
// HttpResponse& response) {
//     if (!isSessionOk(request, response)) {
//         return;
//     }

//     unordered_map<string, string> cookie = request.getCookie();
//     auto it = cookie.find("session_id");
//     if (it != cookie.end()) {
//         string sessionId = it->second;

//         Session sessioData =
//         sessionRepository.getSessionData(sessionId);

//         response.setStatus(200);
//         response.addHeader("Content-Type", "text/html; charset=utf-8");
//         response.setBody(sessioData.userData);
//     }
// });

// router.addStaticRoute("/upload/page", [](HttpRequest& request,
// HttpResponse& response) {
//     response.setStatus(200);
//     response.addHeader("Content-Type", "text/html; charset=utf-8");

//     response.setBody(loadFileContent("../src/views/upload_video.html"));
// });

// router.addStaticRoute("/upload", [](HttpRequest& request, HttpResponse&
// response) {
//     string body = request.getBody();
//     // std::cout << body << std::endl;

//     auto it = request.getHeaders().find("Content-Type");
//     if (it == request.getHeaders().end()) {
//         response.setStatus(500);
//         response.setBody(R"({"message": "В запросе отсувствует
//         Content-Type"})"); return;
//     }

//     string contentType = it->second;

//     if (saveFilesFromBody(body, contentType)) {
//         response.setStatus(200);
//         response.setBody(R"({"message": "Файлы загружен"})");
//     } else {
//         response.setStatus(500);
//         response.setBody(R"({"message": "Ошибка сохранения файлов"})");
//     }
// });

// router.addStaticRoute("/path/to/your/video.mp4", [](HttpRequest&
// request, HttpResponse& response) {
//     std::ifstream file("video.mp4", std::ios::binary);
//     if (file) {
//         response.addHeader("Content-Type", "video/mp4");
//         response.addHeader("Content-Disposition", "attachment;
//         filename=video.mp4");

//         response.setBody(string((std::istreambuf_iterator<char>(file)),
//         std::istreambuf_iterator<char>())); response.setStatus(200);
//     } else {
//         response.setStatus(404);
//         response.setBody(R"({"message": "Файл не найден"})");
//     }
// });
