#ifndef TICKET_SYSTEM_UTILS_HPP
#define TICKET_SYSTEM_UTILS_HPP

#include <iostream>

constexpr int ZH = 3; // 中文字符所占字节数
constexpr int EPS = 3;
constexpr int STATION_NUM = 100 + EPS;
constexpr int TRAINID_LEN = 20 + EPS;
constexpr int STATIONID_LEN = ZH * 10 + EPS;
constexpr int USERNAME_LEN = 20 + EPS;
constexpr int NAME_LEN = ZH * 5 + EPS;
constexpr int PASSWORD_LEN = 30 + EPS;
constexpr int INF = 2e9;
std::string TIMESTAMP;

/* 储存各种类型的命令字符串 */
class Cmd {
private:
    std::string type[26];
public:
    Cmd() { reset(); }
    std::string get(char ch) const {
        return type[ch - 'a'];
    }
    void set(char ch, const std::string &str) {
        type[ch - 'a'] = str;
    }
    bool have(char ch) const {
        return !type[ch - 'a'].empty();
    }
    void reset() {
        for (int i = 0; i < 26; ++i)
            type[i].clear();
    }
};

/* 解析带有分隔符的命令 */
class Parse {
private:
    std::string str;
    int pos = 0, len;
    char c;
public:
    void read(char c_) {
        getline(std::cin, str);
        len = str.size(), c = c_, pos = 0;
    }
    void set(char c_, const std::string &str_) {
        c = c_, str = str_, len = str.size(), pos = 0;
    }
    std::string nextToken() {
        while (pos < len && str[pos] == c) ++pos;
        if (pos == len) return "";
        std::string res;
        while (pos < len && str[pos] != c) res += (str[pos++]);
        return res;
    }
};

/* 存储字符串的 hash 值 */
struct Varchar {
    size_t var;
    Varchar() { }
    Varchar(const std::string &str) {
        var = std::hash<std::string>()(str);
    }
    Varchar(const size_t &var_) { var = var_; }
    bool operator<(const Varchar &that) const {
        return var < that.var;
    }
    bool operator==(const Varchar &that) const {
        return var == that.var;
    }
    bool operator>(const Varchar &that) const {
        return var > that.var;
    }
};

/* string to int */
int strtoint(const std::string &str) {
    int res = 0;
    for (int i = 0, len = str.size(); i < len; ++i)
        res = res * 10 + str[i] - '0';
    return res;
}

/* hr:mi 形式的字符串转为数字 */
int hhmmtomin(const std::string &str) {
    int res = (str[0] - '0') * 10 + (str[1] - '0');
    res *= 60;
    res += (str[3] - '0') * 10 + str[4] - '0';
    return res;
}

/* int 转为两位数 string */
std::string tod2(int x) {
    std::string res;
    res += char(x / 10 + '0');
    res += char(x % 10 + '0');
    return res;
}

/* 手写 pair */
template<class T1, class T2>
class pair {
public:
	T1 first;
	T2 second;
	constexpr pair() : first(), second() {}
	pair(const pair &other) = default;
	pair(pair &&other) = default;
	pair(const T1 &x, const T2 &y) : first(x), second(y) {}
	template<class U1, class U2>
	pair(U1 &&x, U2 &&y) : first(x), second(y) {}
	template<class U1, class U2>
	pair(const pair<U1, U2> &other) : first(other.first), second(other.second) {}
	template<class U1, class U2>
	pair(pair<U1, U2> &&other) : first(other.first), second(other.second) {}
};

/* 读入一行命令并进行处理 */
bool readline(std::string &opt, std::string &timestamp, Cmd & info) {
    Parse cmd;
    cmd.read(' ');
    TIMESTAMP = timestamp = cmd.nextToken();
    opt = cmd.nextToken();
    if (opt == "") return false;

    while (true) {
        const std::string &pos = cmd.nextToken();
        if (pos.empty()) break;
        const std::string &var = cmd.nextToken();
        info.set(pos[1], var);
    }

    return true;
}

/*
 * Class `Date` can work in range of 01-01 00:00 - 12-31 23:59
 * The value `now` represent the number of minutes from 01-01 00:00 to now
 */
class Date {
public:
    const static int daysum[13];
    int now;
    Date() { }
    Date(int now_) { now = now_; }
    Date(int mm, int dd, int hr = 0, int mi = 0) {
        dd += daysum[mm - 1];
        now = ((dd - 1) * 24 + hr) * 60 + mi;
    }
    void set_mmdd(const std::string &str) {
        *this = Date((str[0] - '0') * 10 + str[1] - '0', (str[3] - '0') * 10 + str[4] - '0');
    }
    void addDay(int x = 1) {
        now += x * 1440;
    }
    Date operator+(const int &rhs) const {
        return Date(now + rhs);
    }
    Date operator-(const int &rhs) const {
        return Date(now - rhs);
    }
    int operator-(const Date &rhs) const {
        return now - rhs.now;
    }
    Date &operator+=(const int &rhs) {
        now += rhs;
        return *this;
    }
    bool operator<(const Date &rhs) const {
        return now < rhs.now;
    }
    bool operator<=(const Date &rhs) const {
        return now <= rhs.now;
    }
    bool operator>(const Date &rhs) const {
        return now > rhs.now;
    }
    bool operator>=(const Date &rhs) const {
        return now >= rhs.now;
    }
    std::string get_mmdd() const {
        int dd = now / 1440, mm;
        mm = std::lower_bound(daysum + 1, daysum + 13, dd) - daysum;
        if (daysum[mm] > dd) --mm;
        dd -= daysum[mm];
        return tod2(mm + 1) + '-' + tod2(dd + 1);
    }
    std::string get_hrmi() const {
        return tod2((now / 60) % 24) + ':' + tod2(now % 60);
    }
    std::string get_mmddhrmi() const {
        return get_mmdd() + ' ' + get_hrmi();
    }
};

const int Date::daysum[13] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};

#endif