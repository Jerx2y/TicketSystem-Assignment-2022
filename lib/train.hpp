#ifndef TICKET_SYSTEM_TRAIN_HPP
#define TICKET_SYSTEM_TRAIN_HPP

#include <cstring>

#include "utils.hpp"

// 存储火车信息
class Train {
public:
    char trainID[TRAINID_LEN];
    size_t trainIDhash;
    int stationNum;
    int seatNum;
    char stationID[STATION_NUM][STATIONID_LEN]; // 储存每个站的中文名，假设每个汉字占 3 字节 
    int prices[STATION_NUM]; // 储存站 i 到站 i+1 的票价 
    int startTime; // 始发站发车的时间离当天 0 点经过的分钟数
    int arriveTime[STATION_NUM], leaveTime[STATION_NUM]; // 第 i 站的到达和发车时间
    Date startDate, endDate;
    char type;
    bool released;

    bool operator<(const Train &rhs) const {
        return strcmp(trainID, rhs.trainID) < 0;
    }

    void set(const Cmd &i) {
        strcpy(trainID, i.get('i').c_str());
        trainIDhash = std::hash<std::string>()(i.get('i'));
        stationNum = strtoint(i.get('n'));
        seatNum = strtoint(i.get('m'));
        type = i.get('y')[0];

        Parse tmp, tmp2;

        tmp.set('|', i.get('s'));
        for (int j = 0; j < stationNum; ++j)
            strcpy(stationID[j], tmp.nextToken().c_str());

        tmp.set('|', i.get('p'));
        prices[0] = 0;
        for (int j = 1; j < stationNum; ++j)
            prices[j] = prices[j - 1] + strtoint(tmp.nextToken());
        
        startTime = hhmmtomin(i.get('x'));

        tmp.set('|', i.get('t'));
        tmp2.set('|', i.get('o'));

        leaveTime[0] = 0;
        for (int j = 1; j < stationNum; ++j) {
            arriveTime[j] = leaveTime[j - 1] + strtoint(tmp.nextToken());
            if (j < stationNum - 1) leaveTime[j] = arriveTime[j] + strtoint(tmp2.nextToken());
            else leaveTime[j] = INF;
        }

        tmp.set('|', i.get('d'));
        startDate.set_mmdd(tmp.nextToken());
        endDate.set_mmdd(tmp.nextToken());

        released = 0;
    }

};

// train 和 day 的组合，存储每一辆车的剩余座位
class dayTrain {
private:
public:
    int ticket[STATION_NUM]; // 第 i 站到第 i+1 站的票余量
    dayTrain() { ; }
    dayTrain(int x) {
        for (int i = 0; i < 100; ++i)
            ticket[i] = x;
    }
    void minus(int l, int r, int x) {
        for (int i = l; i < r; ++i)
            ticket[i] -= x;
    }
    int getmin(int l, int r) const {
        int res = INF;
        for (int i = l; i < r; ++i)
            res = std::min(res, ticket[i]);
        return res;
    }
    int getseat(int x) {
        return ticket[x];
    }
    bool operator<(const dayTrain &rhs) const {
        return false;
    }
};

// 经过这个站的火车信息，避免在 query_ticket 时读取整个 train 结构体
class stationTrain {
public:
    char trainID[TRAINID_LEN];
    size_t trainIDhash = 0;
    int idx, price;
    Date startDate, endDate;
    int startTime, arriveTime, leaveTime;
    bool operator<(const stationTrain &rhs) const {
        return trainIDhash < rhs.trainIDhash;
    }
};

// query_ticket 中统计答案时所用的结构体
struct ticketTrain {
    int first;
    char second[TRAINID_LEN];
    int price, seat;
    Date leaveTime, arriveTime;
    bool operator<(const ticketTrain &rhs) const {
        return first < rhs.first || (first == rhs.first && strcmp(second, rhs.second) < 0);
    }
};

// query_transfer 中统计答案时所用的结构体
struct transferTrain {
    int first, second;
    char train1[TRAINID_LEN], train2[TRAINID_LEN];
    char station[STATIONID_LEN];
    Date day1, day2;
    Date leave1, arrive1, leave2, arrive2;
    int cost1, cost2;
    int ids1, ids2, idt1, idt2;
    bool operator<(const transferTrain &rhs) const {
        if (first != rhs.first)
            return first < rhs.first;
        else if (second != rhs.second)
            return second < rhs.second;
        else if (int tmp = strcmp(train1, rhs.train1))
            return tmp < 0;
        else return tmp < 0;
    }
};

#endif