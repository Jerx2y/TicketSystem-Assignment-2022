#ifndef TICKET_SYSTEM_STORAGE_HPP
#define TICKET_SYSTEM_STORAGE_HPP

#include <fstream>
#include <iostream>

#include "rollback.hpp"
#include "utils.hpp"

using std::fstream;
using std::ifstream;
using std::ofstream;
using std::string;

template <class T>
class Storage {
    private:
        fstream file_;
        string file_name_;
        int sizeofT_ = sizeof(T);
        struct RollbackNode {
            int ti;
            int k;
            T v;
            RollbackNode() { }
            RollbackNode(const int &k_, const T &v_) {
                k = k_, v = v_;
                ti = strtoint(TIMESTAMP.substr(1, TIMESTAMP.size() - 2));
            }
        };

        FileStack<RollbackNode> stk;

    public:
        Storage(string FN = "", int opt = 0) : stk(FN) {
            file_name_ = FN + ".dat";
            file_.open(file_name_);
            if (!file_) {
                file_.open(file_name_, std::ios::out);
                for (int i = 1, x = 0; i <= opt; ++i)
                    file_.write(reinterpret_cast<char *>(&x), sizeof(int));
            }
            file_.close();
        }

        ~Storage() { file_.close(); }

        //读出第n个int的值赋给tmp，1_base
        void get_info(int &tmp, int n) {
            file_.open(file_name_);
            file_.seekg((n - 1) * sizeof(int));
            file_.read(reinterpret_cast<char *>(&tmp), sizeof(int));
            file_.close();
        }

        //将tmp写入第n个int的位置，1_base
        void write_info(int tmp, int n) {
            file_.open(file_name_);
            file_.seekp((n - 1) * sizeof(int));
            file_.write(reinterpret_cast<char *>(&tmp), sizeof(int));
            file_.close();
        }

        //在文件合适位置写入类对象t，并返回写入的位置索引index
        //位置索引意味着当输入正确的位置索引index，在以下三个函数中都能顺利的找到目标对象进行操作
        //位置索引index可以取为对象写入的起始位置
        int write(T &t) {
            file_.open(file_name_);
            file_.seekp(0, std::ios::end);
            int res = file_.tellp();
            file_.write(reinterpret_cast<char *>(&t), sizeofT_);
            file_.close();
            return res;
        }

        //用t的值更新位置索引index对应的对象，保证调用的index都是由write函数产生
        void update(T &t, const int index, int log = 0) {
            file_.open(file_name_);
            if (!log) {
                file_.seekg(index);
                T tmp;
                file_.read(reinterpret_cast<char *>(&tmp), sizeofT_);
                stk.push(RollbackNode(index, tmp));
            }
            file_.seekp(index);
            file_.write(reinterpret_cast<char *>(&t), sizeofT_);
            file_.close();
        }

        //读出位置索引index对应的T对象的值并赋值给t，保证调用的index都是由write函数产生
        void read(T &t, const int index = 0) {
            file_.open(file_name_);
            file_.seekg(index);
            file_.read(reinterpret_cast<char *>(&t), sizeofT_);
            file_.close();
        }

        void rollback(int now) {
            while (!stk.empty()) {
                RollbackNode tmp;
                stk.gettop(tmp);
                if (tmp.ti <= now)
                    break;
                update(tmp.v, tmp.k, 1);
                stk.pop();
            }
        }

        void Delete(int index) { ; }
};

#endif

