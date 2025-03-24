#ifndef SESSION_REPOSITORY_H
#define SESSION_REPOSITORY_H

#include <unordered_map>
#include <string>
#include <sstream>
#include <chrono>
#include <random>

using std::string;
using std::unordered_map;
using std::chrono::system_clock, std::chrono::time_point, std::chrono::duration_cast, std::chrono::seconds;

struct Session {
    string userData;
    time_point<system_clock> creationTime;
};

class SessionRepository {
public:
    string createSessionID(const string& userData) {
        string sessionID = generateSessionID();
        sessions.insert({sessionID, {userData, system_clock::now()}});
        return sessionID;
    }

    bool isSessionValid(const string& sessionID) {
        unordered_map<string, Session>::const_iterator it = sessions.find(sessionID);
        if (it == sessions.end()) {
            return false;
        }

        Session session = it->second;
        auto now = system_clock::now();

        auto sessionDuration = duration_cast<seconds>(now - session.creationTime).count();
        if (sessionDuration < sessionTTL) {
            return true;
        } else {
            sessions.erase(it);
            return false;
        }
    }

    void extendSessionTime(const string& sessionID) {
        auto it = sessions.find(sessionID);
        if (it != sessions.end()) {
            it->second.creationTime = system_clock::now();
        }
    }

    const Session& getSessionData(const string& sessionID) {
        auto it = sessions.find(sessionID);
        if (it != sessions.end()) {
            return it->second;
        }
    }

    void setSessionTTL(const int sessionTTL) {
        this->sessionTTL = sessionTTL;
    }

private:
    unordered_map<string, Session> sessions;

    int sessionTTL = 1800; // 30 min (TTL - Time To Live)

    string generateSessionID() {
        std::stringstream ss;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 15);
        
        // UUID-like
        for (int i = 0; i < 16; i++) {
            ss << std::hex << dis(gen);
        }
        return ss.str();
    }
};

#endif // SESSION_REPOSITORY_H
