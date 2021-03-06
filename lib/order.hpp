#ifndef TICKET_SYSTEM_ORDER_HPP
#define TICKET_SYSTEM_ORDER_HPP

#include <cstring>

#include "utils.hpp"

struct Order {
    char trainID[TRAINID_LEN];
    char startStation[STATIONID_LEN];
    char endStation[STATIONID_LEN];
    Date day;
    Date leavingTime;
    Date arrivingTime;
    int ids;
    int idt;
    int prices, num;
    int status; // 0 success ; 1 pending ; 2 refunded
    void set(Date day_, const char *trainID_, int ids_, int idt_, const char *sS_, const char *eS_, int status_, Date lT_, Date aT_, int prices_, int num_) {
        day = day_;
        ids = ids_;
        idt = idt_;
        strcpy(trainID, trainID_);
        strcpy(startStation, sS_);
        strcpy(endStation, eS_);
        status = status_;
        leavingTime = lT_;
        arrivingTime = aT_;
        prices = prices_;
        num = num_;
    }
};

#endif