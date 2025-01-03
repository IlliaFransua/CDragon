#include "routes.h"
#include "http_request.h"
#include "http_response.h"
#include <regex>
#include <fstream>
#include "template_renderer.h"

#include "user_model.h"

#include "session_repository.h"

using std::string;

SessionRepository sessionRepository;

std::function<bool(HttpRequest&, HttpResponse&)> isSessionOk = [](HttpRequest& request, HttpResponse& response){
    unordered_map<string, string> cookie = request.getCookie();
    auto it = cookie.find("session_id");

    if (it == cookie.end()) {
        response.setStatus(401);
        response.addHeader("Content-Type", "text/html; charset=utf-8");
        response.setBody("Для доступа к странице нужно залогинится");
        return false;
    }

    if (!sessionRepository.isSessionValid(it->second)) {
        response.setStatus(401);
        response.addHeader("Content-Type", "text/html; charset=utf-8");
        response.setBody("Сессия истекла, нужно залогинется заново");
        return false;
    }

    sessionRepository.extendSessionTime(it->second);
    return true;
};

bool saveFilesFromBody(const std::string& body, const std::string& contentType) {
    std::regex boundaryRegex(R"--(boundary=(.+))--");
    std::smatch boundaryMatch;

    if (std::regex_search(contentType, boundaryMatch, boundaryRegex)) {
        std::string boundary = "--" + boundaryMatch[1].str();

        size_t pos = body.find(boundary);
        while (pos != std::string::npos) {
            size_t endPos = body.find(boundary, pos + boundary.size());
            if (endPos == std::string::npos) {
                break;
            }

            std::string part = body.substr(pos + boundary.size(), endPos - pos - boundary.size());

            size_t headerEnd = part.find("\r\n\r\n");
            if (headerEnd != std::string::npos) {
                std::string headers = part.substr(0, headerEnd);
                std::string content = part.substr(headerEnd + 4);

                std::regex filenameRegex(R"--(filename="(.+?)")--");
                std::smatch filenameMatch;
                if (std::regex_search(headers, filenameMatch, filenameRegex)) {
                    std::string filename = filenameMatch[1].str();

                    std::ofstream outFile(filename, std::ios::binary);
                    if (outFile) {
                        outFile.write(content.data(), content.size());
                        outFile.close();
                    } else {
                        std::cerr << "Ошибка сохранения файла: " << filename << std::endl;
                        return false;
                    }
                }
            }

            pos = body.find(boundary, endPos);
        }
        return true;
    } else {
        std::cerr << "Ошибка: boundary не найден" << std::endl;
        return false;
    }
}

void setupRoutes(Router& router) {
    // // router.addStaticRoute("/about", [](HttpRequest& request, HttpResponse& response) {
    // //     response.setStatus(200);
    // //     response.addHeader("Content-Type", "text/html; charset=utf-8");
    // //     response.setBody("О нас");
    // // });

    // // router.addStaticRoute("/user", [](HttpRequest& request, HttpResponse& response) {
    // //     // а всоооо, раньше надо было

    // //     response.addHeader("Content-Type", "text/html; charset=utf-8");
    // //     // response.setBody(userInfo);
    // // });

    // // router.addDynamicRoute("/company/<companyid>", [](HttpRequest& request, HttpResponse& response) {
    // //     string companyid = request.getPathParams().at(0);
    // //     response.addHeader("Content-Type", "text/html; charset=utf-8");
    // //     response.setBody("<h1>Company ID: " + companyid + "</h1>");
    // // });

    // // router.addDynamicRoute("/company/<companyid>/<adress>", [](HttpRequest& request, HttpResponse& response) {
    // //     string companyid = request.getPathParams().at(0);
    // //     string adress = request.getPathParams().at(1);
    // //     response.addHeader("Content-Type", "text/html; charset=utf-8");
    // //     response.setBody("<h1>Company ID: " + companyid + ", adress: " + adress + "</h1>");
    // // });

    // // router.addDynamicRoute("/company/<companyid>/user/<userid>", [](HttpRequest& request, HttpResponse& response) {
    // //     string companyid = request.getPathParams().at(0);
    // //     string userid = request.getPathParams().at(1);
    // //     response.addHeader("Content-Type", "text/html; charset=utf-8");
    // //     response.setBody("<h1>Company ID: " + companyid + ", User ID: " + userid + "</h1>");
    // // });

    // router.addStaticRoute("/", [](HttpRequest& request, HttpResponse& response) {
    //     response.setStatus(200);
    //     response.addHeader("Content-Type", "text/html; charset=utf-8");
    //     response.setBody("main page");

    //     // UserModel user;

    //     // unordered_map<string, vector<string>> data = {
    //     //     {"pageName", {"Главная страница"}},
    //     //     {"newsList", {"Новость 1", "Новость 2", "Новость 3"}}
    //     // };
        
    //     // response.setBody(TemplateRenderer::render("../src/views/index.html", data));
    // });

    // router.addStaticRoute("/styles.css", [](HttpRequest& request, HttpResponse& response) {
    //     response.setStatus(200);
    //     response.addHeader("Content-Type", "text/css");
        
    //     response.setBody(loadFileContent("../src/static/css/styles.css"));
    // });

    // router.addStaticRoute("/script.js", [](HttpRequest& request, HttpResponse& response) {
    //     response.setStatus(200);
    //     response.addHeader("Content-Type", "application/javascript");
        
    //     response.setBody(loadFileContent("../src/static/js/script.js"));
    // });






    // router.addStaticRoute("/login/page", [](HttpRequest& request, HttpResponse& response) {
    //     response.setStatus(200);
    //     response.addHeader("Content-Type", "text/html");
        
    //     response.setBody(loadFileContent("../src/views/index.html"));
    // });

    // router.addStaticRoute("/register", [](HttpRequest& request, HttpResponse& response) {
    //     unordered_map<string, string> json = request.getJsonData();
    //     auto poSusername = json.find("username");
    //     auto poSpassword = json.find("password");

    //     // for(const auto& [key, value]: json) {
    //     //     std::cout << key << std::endl;
    //     //     std::cout << value << std::endl;
    //     // }
        
    //     if (poSusername == json.end() || poSpassword == json.end()) {
    //         response.setStatus(400);
    //         response.addHeader("Content-Type", "application/json; charset=utf-8");
    //         response.setBody(R"({"message": "Неправильный запрос на создание сессии"})");
    //         return;
    //     }

    //     const string username = poSusername->second;
    //     const string password = poSpassword->second;

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
    //         response.addHeader("Content-Type", "application/json; charset=utf-8");
    //         response.setBody(R"({"message": "Неправильный логин или пароль"})");
    //         return;
    //     }
    // });

    // router.addStaticRoute("/admin/page", [](HttpRequest& request, HttpResponse& response) {
    //     if (!isSessionOk(request, response)) {
    //         return;
    //     }

    //     unordered_map<string, string> cookie = request.getCookie();
    //     auto it = cookie.find("session_id");
    //     if (it != cookie.end()) {
    //         string sessionId = it->second;

    //         Session sessioData = sessionRepository.getSessionData(sessionId);

    //         response.setStatus(200);
    //         response.addHeader("Content-Type", "text/html; charset=utf-8");
    //         response.setBody(sessioData.userData);
    //     }
    // });

    // router.addStaticRoute("/upload/page", [](HttpRequest& request, HttpResponse& response) {
    //     response.setStatus(200);
    //     response.addHeader("Content-Type", "text/html; charset=utf-8");
    
    //     response.setBody(loadFileContent("../src/views/upload_video.html"));
    // });
    
    // router.addStaticRoute("/upload", [](HttpRequest& request, HttpResponse& response) {
    //     std::string body = request.getBody();
    //     // std::cout << body << std::endl;

    //     auto it = request.getHeaders().find("Content-Type");
    //     if (it == request.getHeaders().end()) {
    //         response.setStatus(500);
    //         response.setBody(R"({"message": "В запросе отсувствует Content-Type"})");
    //         return;
    //     }

    //     std::string contentType = it->second;

    //     if (saveFilesFromBody(body, contentType)) {
    //         response.setStatus(200);
    //         response.setBody(R"({"message": "Файлы загружен"})");
    //     } else {
    //         response.setStatus(500);
    //         response.setBody(R"({"message": "Ошибка сохранения файлов"})");
    //     }
    // });

    // router.addStaticRoute("/path/to/your/video.mp4", [](HttpRequest& request, HttpResponse& response) {
    //     std::ifstream file("video.mp4", std::ios::binary);
    //     if (file) {
    //         response.addHeader("Content-Type", "video/mp4");
    //         response.addHeader("Content-Disposition", "attachment; filename=video.mp4");
            
    //         response.setBody(std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>()));
    //         response.setStatus(200);
    //     } else {
    //         response.setStatus(404);
    //         response.setBody(R"({"message": "Файл не найден"})");
    //     }
    // });


}
