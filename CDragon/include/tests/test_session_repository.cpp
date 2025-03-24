#include <gtest/gtest.h>
#include "session_repository.h"
#include <string>
#include <iostream>
#include <thread>

using std::string;
using std::cout, std::endl;

TEST(SessionRepository, create_session) {
    SessionRepository session_repository;
    string userData = "user 1";
    string sessionID = session_repository.createSessionID(userData);
    
    EXPECT_EQ(!sessionID.empty(), true);
    EXPECT_EQ(session_repository.isSessionValid(sessionID), true);
}

TEST(SessionRepository, is_session_valid_or_invalid) {
    SessionRepository session_repository;
    string invalidSessionID = "invalidSessionId";

    EXPECT_EQ(!session_repository.isSessionValid(invalidSessionID), true);
}

TEST(SessionRepository, sessionTTL) {
    SessionRepository session_repository;
    string userData = "user 1";
    string sessionID = session_repository.createSessionID(userData);

    session_repository.setSessionTTL(10);

    std::this_thread::sleep_for(std::chrono::seconds(11));

    EXPECT_EQ(!session_repository.isSessionValid(sessionID), true);
}

TEST(SessionRepository, extend_session_time) {
    SessionRepository session_repository;
    string userData = "user 1";
    string sessionID = session_repository.createSessionID(userData);

    session_repository.setSessionTTL(10);

    std::this_thread::sleep_for(std::chrono::seconds(5));

    EXPECT_EQ(session_repository.isSessionValid(sessionID), true);

    session_repository.extendSessionTime(sessionID);

    std::this_thread::sleep_for(std::chrono::seconds(5));

    EXPECT_EQ(session_repository.isSessionValid(sessionID), true);
}

TEST(SessionRepository, get_session_data) {
    SessionRepository session_repository;
    string userData = "user 1";
    string sessionID = session_repository.createSessionID(userData);

    const Session& session = session_repository.getSessionData(sessionID);
    EXPECT_EQ(session.userData == userData, true);
}

TEST(SessionRepository, create_duplicate_session) {
    SessionRepository repo;
    string userData = "user 1";
    string sessionID1 = repo.createSessionID(userData);
    string sessionID2 = repo.createSessionID(userData);

    EXPECT_EQ(sessionID1 != sessionID2, true);
}