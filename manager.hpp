#ifndef TICKET_SYSTEM_MANAGER_HPP
#define TICKET_SYSTEM_MANAGER_HPP

#include <cstdlib>
#include <functional>
#include <iostream>
#include <cstring>

#include "user.hpp"
#include "train.hpp"
#include "order.hpp"
#include "linked_hashmap.hpp"
#include "utils.hpp"
#include "storage.hpp"
#include "BPlustree.hpp"

// TODO
#include <string>
#include <vector>
#include <algorithm> // sort

using std::cout;
using std::endl;

class Manager {
private:
    linked_hashmap<std::string, int> online;
    // TODO:
    lailai::map<Varchar, User> user_;
    lailai::map<Varchar, Train> train_;
    // lailai::map<std::pair<Varchar, Date>, dayTrain> daytrain_;
    lailai::map<size_t, dayTrain> daytrain_;
    lailai::map<std::pair<Varchar, Varchar>, stationTrain> stationtrain_;
    lailai::map<std::pair<std::pair<Varchar, Date>, int>, int> pending_order_;
    lailai::map<std::pair<Varchar, int>, int> user_order_;
    Storage<Order> order_;
    int usercnt;

public:

    Manager() : user_("user"),//, 0),
                train_("train"),//, 1),
                daytrain_("daytrain"),//, 1),
                order_("order", 1),//, 0),
                stationtrain_("stationtrain"),//, 0),
                pending_order_("pendingorder"),//, 0),
                user_order_("userorder") { //, 0), {
        order_.get_info(usercnt, 1);
    }

    ~Manager() {
        order_.write_info(usercnt, 1);
    }

    std::string add_user(Cmd info) {
        bool isfirst = !usercnt;
        if (!isfirst && !online.count(info.get('c')))
            return "add_user: user is not online";
        if (!isfirst && online.at(info.get('c')) <= strtoint(info.get('g')))
            return "add_user: have not permission";

        if (user_.count(Varchar(info.get('u'))))
            return "add_user: user already exist";

        if (isfirst)
            info.set('g', "10");

        User tmp(info);
        user_.Insert(Varchar(info.get('u')), tmp);
        cout << '0' << endl;
        
        usercnt++;

        return "okk";
    }

    std::string login(const Cmd &info) {
        if (online.count(info.get('u')))
            return "login: user is online";
        if (!user_.count(Varchar(info.get('u'))))
            return "login: user do not exist";

        User tmp;
        user_.Getone(Varchar(info.get('u')), tmp);
        if (std::hash<std::string>()(info.get('p')) != tmp.passwordHash)
            return "login: password wrong";

        online.insert(pair(info.get('u'), tmp.privilege));
        cout << '0' << endl;

        return "okk";
    }

    std::string logout(const Cmd &info) {
        if (!online.count(info.get('u')))
            return "logout: user is not online";
        online.erase(info.get('u'));
        cout << '0' << endl;

        return "okk";
    }

    std::string query_profile(const Cmd &info) {
        if (!online.count(info.get('c'))) 
            return "query_profile: user-c is not online";
        if (!user_.count(Varchar(info.get('u'))))
            return "query_profile: user-u do not exist";

        int p = online.at(info.get('c'));
        User tmp;
        user_.Getone(Varchar(info.get('u')), tmp);
    
        if (!(tmp.privilege < p || (tmp.privilege == p && info.get('u') == info.get('c'))))
            return "query_profile: have not premission";
        
        cout << tmp.username << ' ' << tmp.name << ' ' << tmp.mailAddr << ' ' << tmp.privilege << endl;

        return "okk";
    }

    std::string modify_profile(const Cmd &info) {
        if (!online.count(info.get('c'))) 
            return "modify_profile: user-c is not online";
        if (!user_.count(Varchar(info.get('u'))))
            return "modify_profile: user-u do not exist";

        int p = online.at(info.get('c'));
        if (info.have('g') && strtoint(info.get('g')) >= p)
            return "modify_profile: can not give more previlege, i.e. " + tod2(p) + " can not give " + info.get('g');

        User tmp;
        user_.Getone(Varchar(info.get('u')), tmp);



        if (!(tmp.privilege < p || (tmp.privilege == p && info.get('u') == info.get('c'))))
            return "modify_profile: have not premission";

        if (info.have('p')) tmp.passwordHash = std::hash<std::string>()(info.get('p'));
        if (info.have('n')) strcpy(tmp.name, info.get('n').c_str());
        if (info.have('m')) strcpy(tmp.mailAddr, info.get('m').c_str());
        if (info.have('g')) tmp.privilege = strtoint(info.get('g'));
        if (info.have('g') && online.count(info.get('u'))) online.at(info.get('u')) = strtoint(info.get('g'));

        user_.Modify(tmp.usernameHash, tmp);

        cout << tmp.username << ' ' << tmp.name << ' ' << tmp.mailAddr << ' ' << tmp.privilege << endl;
    
        return "okk";
    }

    std::string add_train(const Cmd &info) {
        if (train_.count(Varchar(info.get('i'))))
            return "add_train: trainID already exist";

        Train tmp;
        tmp.set(info);
        train_.Insert(Varchar(info.get('i')), tmp);
        cout << '0' << endl;

        return "okk";
    }

    std::string delete_train(const Cmd &info) {
        if (!train_.count(Varchar(info.get('i'))))
            return "delete_train: train do not exist";
        Train tmp;
        train_.Getone(Varchar(info.get('i')), tmp);
        if (tmp.released) 
            return "delete_train: train has already released";
        train_.Remove(Varchar(info.get('i')), tmp);
        cout << '0' << endl;

        return "okk";
    }

    std::string release_train(const Cmd &info) {
        if (!train_.count(Varchar(info.get('i'))))
            return "release_train: train do not exist";
        Train tmp;
        train_.Getone(Varchar(info.get('i')), tmp);
        if (tmp.released)
            return "release_train: train has already released";
        tmp.released = 1;
        train_.Modify(tmp.trainIDhash, tmp);

        for (Date i = tmp.startDate; i <= tmp.endDate; i.addDay())
            daytrain_.Insert(Varchar(info.get('i')).var ^ i.now, dayTrain(tmp.seatNum));

        for (int i = 0; i < tmp.stationNum; ++i) {
            stationTrain stmp;
            strcpy(stmp.trainID, tmp.trainID);
            stmp.trainIDhash = tmp.trainIDhash;
            stmp.idx = i;
            stmp.price = tmp.prices[i];
            stmp.arriveTime = tmp.arriveTime[i];
            stmp.leaveTime = tmp.leaveTime[i];
            stmp.startDate = tmp.startDate;
            stmp.endDate = tmp.endDate;
            stmp.startTime = tmp.startTime;
            stationtrain_.Insert(std::make_pair(Varchar(tmp.stationID[i]), tmp.trainIDhash), stmp);
        }

        cout << '0' << endl;

        return "okk";
    }

    std::string query_train(const Cmd &info) {
        if (!train_.count(Varchar(info.get('i'))))
            return "query_train: train does not exist";
        Train tmp;
        train_.Getone(Varchar(info.get('i')), tmp);
        Date start;
        start.set_mmdd(info.get('d'));
        if (start < tmp.startDate || start > tmp.endDate)
            return "query_train: not in train run range";
        dayTrain dtmp;
        if (tmp.released)
            daytrain_.Getone(Varchar(info.get('i')).var ^ start.now, dtmp);
        else dtmp = dayTrain(tmp.seatNum);

        cout << tmp.trainID << ' ' << tmp.type << endl;
        for (int i = 0; i < tmp.stationNum; ++i) {
            cout << tmp.stationID[i] << ' ';
            if (i == 0) cout << "xx-xx xx:xx";
            else cout << (start + tmp.startTime + tmp.arriveTime[i]).get_mmddhrmi();
            cout << " -> ";
            if (i == tmp.stationNum - 1) cout << "xx-xx xx:xx";
            else cout << (start + tmp.startTime + tmp.leaveTime[i]).get_mmddhrmi();
            cout << ' ' << tmp.prices[i] << ' ';
            if (i == tmp.stationNum - 1) cout << 'x';
            else cout << dtmp.getseat(i);
            cout << endl;
        }
    
        return "okk";
    }

    std::string query_ticket(const Cmd &info) {

        std::vector<stationTrain> strain, ttrain;
        std::vector<ticketTrain> atrain;

        stationtrain_.Get(std::make_pair(Varchar(info.get('s')), 0), std::make_pair(Varchar(info.get('s')), SIZE_MAX), strain);
        stationtrain_.Get(std::make_pair(Varchar(info.get('t')), 0), std::make_pair(Varchar(info.get('t')), SIZE_MAX), ttrain);

        if (strain.empty() || ttrain.empty())
            return cout << 0 << endl, "okk";


        sort(strain.begin(), strain.end());
        sort(ttrain.begin(), ttrain.end());

        Date today;
        today.set_mmdd(info.get('d'));

        for (int i = 0, j = 0; i < strain.size(); ++i) {
            const stationTrain &si = strain[i];
            while (j < ttrain.size() && ttrain[j].trainIDhash < si.trainIDhash) ++j;
            if (j == ttrain.size()) break;
            if (si.trainIDhash < ttrain[j].trainIDhash) continue;
            if (si.leaveTime > ttrain[j].leaveTime) continue;
            Date startDay(((today.now + (si.startTime + si.leaveTime) % 1440 - si.leaveTime) / 1440) * 1440); // 始发站发车日期

            if (startDay < si.startDate || si.endDate < startDay) continue;

            ticketTrain tt;
            strcpy(tt.second, si.trainID);
            tt.price = ttrain[j].price - si.price;
            tt.leaveTime = startDay + si.startTime + si.leaveTime;
            tt.arriveTime = startDay + si.startTime + ttrain[j].arriveTime;
            tt.first = info.get('p') == "cost" ? tt.price : tt.arriveTime - tt.leaveTime;
            dayTrain dt;
            bool tmp = daytrain_.Getone(si.trainIDhash ^ startDay.now, dt);

            tt.seat = dt.getmin(si.idx, ttrain[j].idx);
            atrain.push_back(tt);
        }

        sort(atrain.begin(), atrain.end());

        std::cout << atrain.size() << std::endl;
        for (const auto &i : atrain)
            std::cout << i.second << ' '
                      << info.get('s') << ' ' << i.leaveTime.get_mmddhrmi() << " -> "
                      << info.get('t') << ' ' << i.arriveTime.get_mmddhrmi() << ' '
                      << i.price << ' ' << i.seat << std::endl;

        return "okk";
    }

    std::string query_transfer(const Cmd &info) {
        std::vector<stationTrain> strain, ttrain;
        
        stationtrain_.Get(std::make_pair(Varchar(info.get('s')), 0), std::make_pair(Varchar(info.get('s')), SIZE_MAX), strain);
        stationtrain_.Get(std::make_pair(Varchar(info.get('t')), 0), std::make_pair(Varchar(info.get('t')), SIZE_MAX), ttrain);

        Date today;
        today.set_mmdd(info.get('d'));

        transferTrain ans;
        ans.first = INF;

        Train *ttt = new Train[ttrain.size()];

        for (int i = 0; i < ttrain.size(); ++i)
            train_.Getone(ttrain[i].trainIDhash, ttt[i]);

        for (int i = 0; i < strain.size(); ++i) {
            Date startDay(((today.now + (strain[i].startTime + strain[i].leaveTime) % 1440 - strain[i].leaveTime) / 1440) * 1440); // 车 1 从始发站发车的日期
            if (startDay < strain[i].startDate || strain[i].endDate < startDay) continue;
            Train st;
            train_.Getone(strain[i].trainIDhash, st);
            const int &si = strain[i].idx;


            for (int j = 0; j < ttrain.size(); ++j) {
                if (strain[i].trainIDhash == ttrain[j].trainIDhash) continue;
                const Train &tt = ttt[j];
                // train_.Getone(ttrain[j].trainIDhash, tt);
                const int &ti = ttrain[j].idx;

                for (int sk = si + 1; sk < st.stationNum; ++sk)
                    for (int tk = 0; tk < ti; ++tk) {
                        

                        if (strcmp(st.stationID[sk], tt.stationID[tk]))
                            continue;


                        Date transfer = startDay + st.startTime + st.arriveTime[sk]; // 到达中转站的时间
                    
                        Date startDay2((transfer.now / 1440 * 1440 + (tt.startTime + tt.leaveTime[tk]) % 1440 - tt.leaveTime[tk]) / 1440 * 1440); // 车 2 从始发站发车的日期
                        if ((tt.startTime + tt.leaveTime[tk]) % 1440 < transfer.now % 1440) startDay2.addDay(); // 车 1 到达时间晚于车 2 发车时间，故发车日期 + 1

                        if (startDay2 > tt.endDate) continue;
                        if (startDay2 < tt.startDate) startDay2 = tt.startDate; 
                        
                        transferTrain tmp;
                        tmp.arrive1 = transfer;
                        tmp.arrive2 = startDay2 + tt.startTime + tt.arriveTime[ti];
                        tmp.cost1 = st.prices[sk] - st.prices[si];
                        tmp.cost2 = tt.prices[ti] - tt.prices[tk];
                        tmp.day1 = startDay;
                        tmp.day2 = startDay2;
                        tmp.leave1 = startDay + st.startTime + st.leaveTime[si];
                        tmp.leave2 = startDay2 + tt.startTime + tt.leaveTime[tk];
                        strcpy(tmp.station, st.stationID[sk]);
                        strcpy(tmp.train1, st.trainID);
                        strcpy(tmp.train2, tt.trainID);
                        tmp.second = tmp.cost1 + tmp.cost2;
                        tmp.first = tmp.arrive2 - tmp.leave1;
                        tmp.ids1 = si;
                        tmp.idt1 = sk;
                        tmp.ids2 = tk;
                        tmp.idt2 = ti;

                        if (info.get('p') == "cost") 
                            std::swap(tmp.first, tmp.second);
                        
                        if (tmp < ans)
                            ans = tmp;
                    }
            }
        }

        delete []ttt;

        if (ans.first == INF)
            return std::cout << 0 << std::endl, "okk";
        
        dayTrain dt1, dt2;

        daytrain_.Getone(Varchar(ans.train1).var ^ ans.day1.now, dt1);
        int seat1 = dt1.getmin(ans.ids1, ans.idt1);

        daytrain_.Getone(Varchar(ans.train2).var ^ ans.day2.now, dt2);
        int seat2 = dt2.getmin(ans.ids2, ans.idt2);

        std::cout << ans.train1 << ' '
                  << info.get('s') << ' ' << ans.leave1.get_mmddhrmi() << " -> "
                  << ans.station << ' ' << ans.arrive1.get_mmddhrmi() << ' '
                  << ans.cost1 << ' ' << seat1 << std::endl;

        std::cout << ans.train2 << ' '
                  << ans.station << ' ' << ans.leave2.get_mmddhrmi() << " -> "
                  << info.get('t') << ' ' << ans.arrive2.get_mmddhrmi() << ' '
                  << ans.cost2 << ' ' << seat2 << std::endl;

        return "okk";
    }

    std::string buy_ticket(const Cmd &info) {
        if (!online.count(info.get('u')))
            return "buy_ticket: user not login";
        if (!train_.count(info.get('i')))
            return "buy_ticket: train not exist";
        Train t;
        train_.Getone(Varchar(info.get('i')), t);
        if (!t.released)
            return "buy_ticket: train not released"; 

        int buyNum = strtoint(info.get('n'));
        if (buyNum > t.seatNum)
            return "buy_ticket: buy num is greater than train set num";

        int si = -1, ti = -1;
        for (int i = 0; i < t.stationNum; ++i) {
            if (!strcmp(t.stationID[i], info.get('f').c_str())) si = i;
            if (!strcmp(t.stationID[i], info.get('t').c_str())) ti = i;
        }

        if (si == -1 || ti == -1)
            return "buy_ticket: train not stop at s or t";
        if (si >= ti)
            return "buy_ticket: train direction is not right";
            
        Date today;
        today.set_mmdd(info.get('d'));
        Date startDay(((today.now + (t.startTime + t.leaveTime[si]) % 1440 - t.leaveTime[si]) / 1440) * 1440); // 始发站发车日期
        Date leavingTime = startDay + t.startTime + t.leaveTime[si];
        Date arrivingTime = startDay + t.startTime + t.arriveTime[ti];

        if (startDay < t.startDate || t.endDate < startDay)   
            return "buy_ticket: not in train run range";

        dayTrain dt;
        daytrain_.Getone(t.trainIDhash ^ startDay.now, dt);
    
        Order porder;
        int prices = (t.prices[ti] - t.prices[si]) * buyNum;

        if (dt.getmin(si, ti) >= buyNum) {
            dt.minus(si, ti, buyNum);
            daytrain_.Modify(Varchar(t.trainIDhash).var ^ startDay.now, dt);
            porder.set(startDay, info.get('i').c_str(), si, ti, info.get('f').c_str(), info.get('t').c_str(), 0, leavingTime, arrivingTime, prices, buyNum);
            int idx = order_.write(porder);
            pending_order_.Insert(std::make_pair(std::make_pair(Varchar(t.trainIDhash), startDay), idx), idx);
            user_order_.Insert(std::make_pair(Varchar(info.get('u')), idx), idx);
            std::cout << prices << std::endl;
        } else if (info.get('q') == "true") {
            porder.set(startDay, info.get('i').c_str(), si, ti, info.get('f').c_str(), info.get('t').c_str(), 1, leavingTime, arrivingTime, prices, buyNum);
            int idx = order_.write(porder);
            pending_order_.Insert(std::make_pair(std::make_pair(Varchar(t.trainIDhash), startDay), idx), idx);
            user_order_.Insert(std::make_pair(Varchar(info.get('u')), idx), idx);
            std::cout << "queue" << std::endl;
        } else return "buy_ticket: buy ticket failed";

        return "okk";
    }

    std::string query_order(const Cmd &info) {
        if (!online.count(info.get('u')))
            return "query_order: user not online";
        std::vector<int> range;
        user_order_.Get(std::make_pair(Varchar(info.get('u')), 0),
                        std::make_pair(Varchar(info.get('u')), INF),
                        range);
        std::cout << range.size() << std::endl;
        for (int i = range.size() - 1; i >= 0; --i) {
            Order o;
            order_.read(o, range[i]);
            if (o.status == 0) std::cout << "[success] ";
            if (o.status == 1) std::cout << "[pending] ";
            if (o.status == 2) std::cout << "[refunded] ";
            std::cout << o.trainID << ' ' \
                      << o.startStation << ' ' << o.leavingTime.get_mmddhrmi() << " -> " \
                      << o.endStation << ' ' << o.arrivingTime.get_mmddhrmi() << ' ' \
                      << o.prices / o.num << ' ' << o.num << std::endl;
        }
    
        return "okk";
    }

    std::string refund_ticket(const Cmd &info) {
        if (!online.count(info.get('u')))
            return "refund_ticket: user not online";

        std::vector<int> range;
        user_order_.Get(std::make_pair(Varchar(info.get('u')), 0),
                        std::make_pair(Varchar(info.get('u')), INF),
                        range);
        int idx = info.have('n') ? strtoint(info.get('n')) : 1;
        
        idx = range.size() - idx;

        if (idx < 0 || idx >= range.size())
            return "refund_ticket: -n must > 0";

        Order o;
        order_.read(o, range[idx]);
        
        if (o.status == 2)
            return "refund_ticket: order has been refunded";

        if (o.status == 0) {
            dayTrain dt;
            daytrain_.Getone(Varchar(o.trainID).var ^ o.day.now, dt);
            dt.minus(o.ids, o.idt, -o.num);
            std::vector<int> range2;
            pending_order_.Get(std::make_pair(std::make_pair(Varchar(o.trainID), o.day), 0),
                               std::make_pair(std::make_pair(Varchar(o.trainID), o.day), INF),
                               range2);
            for (auto it2 = range2.begin(); it2 != range2.end(); ) {
                Order to;
                order_.read(to, *it2);
                if (to.status != 1) {
                    ++it2;
                    continue;
                }
                if (dt.getmin(to.ids, to.idt) >= to.num) {
                    dt.minus(to.ids, to.idt, to.num);
                    to.status = 0;
                    order_.update(to, *it2);
                    pending_order_.Remove(std::make_pair(std::make_pair(Varchar(o.trainID), o.day), *it2), *it2);
                } else ++it2;
            }
            daytrain_.Modify(Varchar(o.trainID).var ^ o.day.now, dt);
        }

        o.status = 2;
        order_.update(o, range[idx]);

        std::cout << '0' << std::endl;

        return "okk";
    }

    std::string rollback() {
        return "okk";
    }

    std::string clean() {
        std::system("rm -f *.idx");
        std::system("rm -f *.dat");
        usercnt = 0;
        return "okk";
    }

    void offline() {
        online.clear();
    }
};

#endif