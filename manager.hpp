#ifndef TICKET_SYSTEM_MANAGER_HPP
#define TICKET_SYSTEM_MANAGER_HPP

#include <iostream>
#include <cstring>

#include "user.hpp"
#include "train.hpp"
#include "order.hpp"
#include "linked_hashmap.hpp"
#include "utils.hpp"

// TODO
#include <vector>
#include <map>
#include <algorithm> // sort

using std::cout;
using std::endl;

class Manager {
private:
    linked_hashmap<std::string, int> online;
    // TODO:
    std::map<Varchar, User> user_; // idx
    std::map<Varchar, Train> train_;
    std::map<std::pair<Varchar, Date>, dayTrain> daytrain_;
    std::map<int, Order> order_;
    std::multimap<Varchar, stationTrain> stationtrain_;
    std::multimap<std::pair<Varchar, Date>, int> pending_order_;
    std::multimap<Varchar, int> user_order_;

    int ordercnt;

public:

    Manager() {
        ordercnt = 0; // TODO read file !!!
    }

    std::string add_user(Cmd info) {
        bool isfirst = user_.empty();
        if (!isfirst && !online.count(info.get('c')))
            return "add_user: user is not online";
        if (!isfirst && online.at(info.get('c')) <= strtoint(info.get('g')))
            return "add_user: have not permission";

        if (user_.count(Varchar(info.get('u'))))
            return "add_user: user already exist";
        
        if (isfirst)
            info.set('g', "10");

        User tmp(info);
        user_[info.get('u')] = tmp;
        cout << '0' << endl;

        return "okk";
    }

    std::string login(const Cmd &info) {
        if (online.count(info.get('u')))
            return "login: user is online";
        if (!user_.count(Varchar(info.get('u'))))
            return "login: user do not exist";

        User tmp = user_.at(Varchar(info.get('u')));
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
        const User &tmp = user_.at(Varchar(info.get('u')));
    
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

        User tmp = user_.at(Varchar(info.get('u')));

        if (!(tmp.privilege < p || (tmp.privilege == p && info.get('u') == info.get('c'))))
            return "modify_profile: have not premission";

        if (info.have('p')) tmp.passwordHash = std::hash<std::string>()(info.get('p'));
        if (info.have('n')) strcpy(tmp.name, info.get('n').c_str());
        if (info.have('m')) strcpy(tmp.mailAddr, info.get('m').c_str());
        if (info.have('g')) tmp.privilege = strtoint(info.get('g'));

        user_[Varchar(info.get('u'))] = tmp;

        cout << tmp.username << ' ' << tmp.name << ' ' << tmp.mailAddr << ' ' << tmp.privilege << endl;
    
        return "okk";
    }

    std::string add_train(const Cmd &info) {
        if (train_.count(Varchar(info.get('i'))))
            return "add_train: trainID already exist";

        Train tmp;
        tmp.set(info);
        train_[info.get('i')] = tmp;
        cout << '0' << endl;

        return "okk";
    }

    std::string delete_train(const Cmd &info) {
        if (!train_.count(Varchar(info.get('i'))))
            return "delete_train: train do not exist";
        const Train &tmp = train_.at(Varchar(info.get('i')));
        if (tmp.released) 
            return "delete_train: train has already released";
        train_.erase(Varchar(info.get('i')));
        cout << '0' << endl;

        return "okk";
    }

    std::string release_train(const Cmd &info) {
        if (!train_.count(Varchar(info.get('i'))))
            return "release_train: train do not exist";
        Train tmp = train_.at(Varchar(info.get('i')));
        if (tmp.released)
            return "release_train: train has already released";
        tmp.released = 1;
        train_[Varchar(info.get('i'))] = tmp;

        for (Date i = tmp.startDate; i <= tmp.endDate; i.addDay())
            daytrain_[std::make_pair(Varchar(info.get('i')), i)] = dayTrain(tmp.seatNum);

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
            stationtrain_.insert(std::make_pair(Varchar(tmp.stationID[i]), stmp));
        }

        cout << '0' << endl;

        return "okk";
    }

    std::string query_train(const Cmd &info) {
        if (!train_.count(Varchar(info.get('i'))))
            return "query_train: train does not exist";
        const Train &tmp = train_.at(Varchar(info.get('i')));
        Date start;
        start.set_mmdd(info.get('d'));
        if (start < tmp.startDate || start > tmp.endDate)
            return "query_train: not in train run range";
        dayTrain dtmp;
        if (tmp.released)
            dtmp = daytrain_.at(std::make_pair(Varchar(info.get('i')), start));
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

        auto ranges = stationtrain_.equal_range(Varchar(info.get('s')));
        auto ranget = stationtrain_.equal_range(Varchar(info.get('t')));

        for (auto i = ranges.first; i != ranges.second; ++i)
            strain.push_back(i->second);
        for (auto i = ranget.first; i != ranget.second; ++i)
            ttrain.push_back(i->second);


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
            const dayTrain& dt = daytrain_.at(std::make_pair(Varchar(si.trainID), startDay));

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
        
        auto ranges = stationtrain_.equal_range(Varchar(info.get('s')));
        auto ranget = stationtrain_.equal_range(Varchar(info.get('t')));

        for (auto i = ranges.first; i != ranges.second; ++i)
            strain.push_back(i->second);
        for (auto i = ranget.first; i != ranget.second; ++i)
            ttrain.push_back(i->second);

        Date today;
        today.set_mmdd(info.get('d'));

        transferTrain ans;
        ans.first = INF;


        for (int i = 0; i < strain.size(); ++i) {
            Date startDay(((today.now + (strain[i].startTime + strain[i].leaveTime) % 1440 - strain[i].leaveTime) / 1440) * 1440); // 车 1 从始发站发车的日期
            if (startDay < strain[i].startDate || strain[i].endDate < startDay) continue;
            const auto &st = train_.at(strain[i].trainIDhash);
            const int &si = strain[i].idx;

            for (int j = 0; j < ttrain.size(); ++j) {
                if (strain[i].trainIDhash == ttrain[i].trainIDhash) continue;
                const auto &tt = train_.at(ttrain[i].trainIDhash);
                const int &ti = ttrain[i].idx;


                for (int sk = si + 1; sk < st.stationNum; ++sk)
                    for (int tk = 0; tk < ti; ++tk) {
                        if (strcmp(st.stationID[sk], tt.stationID[tk]))
                            continue;
                        

                        Date transfer = startDay + st.startTime + st.arriveTime[sk]; // 到达中转站的时间
                    
                        Date startDay2((transfer.now / 1440 * 1440 + (tt.startTime + tt.leaveTime[ti]) % 1440 - tt.leaveTime[ti]) / 1440 * 1440); // 车 2 从始发站发车的日期
                        if ((tt.startTime + tt.leaveTime[ti]) % 1440 < transfer.now % 1440) startDay2.addDay(); // 车 1 到达时间晚于车 2 发车时间，故发车日期 + 1

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

        if (ans.first == INF)
            return std::cout << 0 << std::endl, "okk";
        
        const auto &dt1 = daytrain_.at(std::make_pair(Varchar(ans.train1), ans.day1));
        int seat1 = dt1.getmin(ans.ids1, ans.idt1);

        const auto &dt2 = daytrain_.at(std::make_pair(Varchar(ans.train2), ans.day2));
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
        const Train &t = train_.at(Varchar(info.get('i')));
        if (!t.released)
            return "buy_ticket: train not released"; 
        
        int si = -1, ti = -1;
        for (int i = 0; i < t.stationNum; ++i) {
            if (!strcmp(t.stationID[i], info.get('f').c_str())) si = i;
            if (!strcmp(t.stationID[i], info.get('t').c_str())) ti = i;
        }

        if (si == -1 || ti == -1)
            return "buy_ticket: train not stop at s or t";
            
        Date today;
        today.set_mmdd(info.get('d'));
        Date startDay(((today.now + (t.startTime + t.leaveTime[si]) % 1440 - t.leaveTime[si]) / 1440) * 1440); // 始发站发车日期
        Date leavingTime = startDay + t.startTime + t.leaveTime[si];
        Date arrivingTime = startDay + t.startTime + t.arriveTime[ti];

        if (startDay < t.startDate || t.endDate < startDay)   
            return "buy_ticket: not in train run range";

        dayTrain dt = daytrain_.at(std::make_pair(Varchar(t.trainIDhash), startDay));
    
        int buyNum = strtoint(info.get('n'));
        Order porder;
        int prices = (t.prices[ti] - t.prices[si]) * buyNum;
        if (dt.getmin(si, ti) >= buyNum) {
            dt.minus(si, ti, buyNum);
            daytrain_[std::make_pair(Varchar(t.trainIDhash), startDay)] = dt;
            porder.set(++ordercnt, startDay, info.get('i').c_str(), si, ti, info.get('f').c_str(), info.get('t').c_str(), 0, leavingTime, arrivingTime, prices, buyNum);
            order_.insert(std::make_pair(ordercnt, porder));
            pending_order_.insert(std::make_pair(std::make_pair(Varchar(t.trainIDhash), startDay), ordercnt));
            user_order_.insert(std::make_pair(Varchar(info.get('u')), ordercnt));
            std::cout << prices << std::endl;
        } else if (info.get('q') == "true") {
            porder.set(++ordercnt, startDay, info.get('i').c_str(), si, ti, info.get('f').c_str(), info.get('t').c_str(), 1, leavingTime, arrivingTime, prices, buyNum);
            order_.insert(std::make_pair(ordercnt, porder));
            pending_order_.insert(std::make_pair(std::make_pair(Varchar(t.trainIDhash), startDay), ordercnt));
            user_order_.insert(std::make_pair(Varchar(info.get('u')), ordercnt));
            std::cout << "queue" << std::endl;
        } else return "buy_ticket: buy ticket failed";

        return "okk";
    }

    std::string query_order(const Cmd &info) {
        if (!online.count(info.get('u')))
            return "query_order: user not online";
        const auto &range = user_order_.equal_range(Varchar(info.get('u')));
        int cnt = 0;
        for (auto it = range.first; it != range.second; ++it) ++cnt;
        std::cout << cnt << std::endl;
        if (cnt == 0) return "okk";
        for (auto it = range.second; ; --it) {
            if (it == range.second) continue;
            const auto &o = order_.at(it->second);
            if (o.status == 0) std::cout << "[success] ";
            if (o.status == 1) std::cout << "[pending] ";
            if (o.status == 2) std::cout << "[refunded] ";
            std::cout << o.trainID << ' ' \
                      << o.startStation << ' ' << o.leavingTime.get_mmddhrmi() << " -> " \
                      << o.endStation << ' ' << o.arrivingTime.get_mmddhrmi() << ' ' \
                      << o.prices / o.num << ' ' << o.num << std::endl;
            if (it == range.first) break;
        }
    
        return "okk";
    }

    std::string refund_ticket(const Cmd &info) {
        if (!online.count(info.get('u')))
            return "refund_ticket: user not online";

        const auto &range = user_order_.equal_range(Varchar(info.get('u')));
        int idx = info.have('n') ? strtoint(info.get('n')) : 1;
        if (idx <= 0)
            return "refund_ticket: -n must > 0";
        auto it = range.second;
        if (it != range.first) --it;
        else return "refund_ticket: order exceed";
        while (--idx)
            if (it != range.first) --it;
            else return "refund_ticket: order exceed";

        auto o = order_.at(it->second);
        
        if (o.status == 2)
            return "refund_ticket: order has been refunded";

        if (o.status == 0) {
            auto dt = daytrain_.at(std::make_pair(Varchar(o.trainID), o.day));
            dt.minus(o.ids, o.idt, -o.num);
            const auto &range2 = pending_order_.equal_range(std::make_pair(Varchar(o.trainID), o.day));
            for (auto it2 = range2.first; it2 != range2.second; ) {
                auto to = order_.at(it2->second);
                if (to.status != 1) {
                    ++it2;
                    continue;
                }
                if (dt.getmin(to.ids, to.idt) >= to.num) {
                    dt.minus(to.ids, to.idt, to.num);
                    to.status = 0;
                    order_[it2->second] = to;
                    auto tmp = it2;
                    ++it2;
                    pending_order_.erase(tmp);
                } else ++it2;
            }
            daytrain_[std::make_pair(Varchar(o.trainID), o.day)] = dt;
        }

        o.status = 2;
        order_[it->second] = o;

        std::cout << '0' << std::endl;

        return "okk";
    }

    std::string rollback() {
        return "okk";
    }

    std::string clear() {
        return "okk";
    }

    void offline() {
        online.clear();
    }
};

#endif