#ifndef TICKET_SYSTEM_USER_HPP
#define TICKET_SYSTEM_USER_HPP

#include <iostream>
#include <cstring>

#include "utils.hpp"

class User {
public:
    char username[USERNAME_LEN];
    char name[NAME_LEN]; //储存中文姓名，假设每个汉字占 4 字节 
    char mailAddr[PASSWORD_LEN];
    int privilege;
    size_t usernameHash, passwordHash;  //存储密码的 hash 值 
    User() { }
    User(const Cmd &i) {
        strcpy(username, i.get('u').c_str());
        strcpy(name, i.get('n').c_str());
        strcpy(mailAddr, i.get('m').c_str());
        privilege = strtoint(i.get('g'));
        usernameHash = Varchar(i.get('u')).var;
        passwordHash = std::hash<std::string>()(i.get('p'));
    }
    bool operator<(const User &rhs) const {
        return usernameHash < rhs.usernameHash;
    }
    User &operator=(const User &rhs) {
        strcpy(username, rhs.username);
        strcpy(name, rhs.name);
        strcpy(mailAddr, rhs.mailAddr);
        privilege = rhs.privilege;
        usernameHash = rhs.usernameHash;
        passwordHash = rhs.passwordHash;
        return *this;
    }
};

#endif