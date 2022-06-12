#include <iostream>

#include "utils.hpp"
#include "manager.hpp"

int main() {

    std::string opt, timestamp;
    Cmd info;
    static Manager sys;
    while (readline(opt, timestamp, info)) {
        std::string result;

        if (opt != "offline")
            cout << timestamp << ' ';

        if (opt == "add_user")
            result = sys.add_user(info);
        if (opt == "query_profile")
            result = sys.query_profile(info);
        if (opt == "modify_profile")
            result = sys.modify_profile(info);
        if (opt == "add_train")
            result = sys.add_train(info);
        if (opt == "delete_train")
            result = sys.delete_train(info);
        if (opt == "release_train")
            result = sys.release_train(info);
        if (opt == "query_train")
            result = sys.query_train(info);
        if (opt == "query_ticket")
            result = sys.query_ticket(info);
        if (opt == "query_transfer")
            result = sys.query_transfer(info);
        if (opt == "buy_ticket")
            result = sys.buy_ticket(info);
        if (opt == "refund_ticket")
            result = sys.refund_ticket(info);
        if (opt == "query_order")
            result = sys.query_order(info);
        if (opt == "login")
            result = sys.login(info);
        if (opt == "logout")
            result = sys.logout(info);
        if (opt == "clean")
            result = sys.clean();
        if (opt == "exit")
            return std::cout << "bye" << std::endl, 0;
        
        if (opt == "offline")
            sys.offline(), result = "okk";
        
        if (result != "okk") {
            std::cout << -1 << std::endl;
            // std::cout << "__DEBUG " << result << std::endl; 
        }

        info.reset();
    }

    return 0;
}
