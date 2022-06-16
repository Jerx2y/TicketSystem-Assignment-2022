#ifndef TICKET_SYSTEM_ROLLBACK_HPP
#define TICKET_SYSTEM_ROLLBACK_HPP

#include <fstream>
#include <iostream>

using std::fstream;
using std::ifstream;
using std::ofstream;
using std::string;

template <class T>
class FileStack {
    private:
        fstream file_;
        string file_name_;
        int sizeofT = sizeof(T);
        static constexpr int sizeofI = sizeof(int);

    public:
        int cnt;
        FileStack(string FN = "", int opt = 1) {
            file_name_ = FN + ".log";
            file_.open(file_name_);
            if (!file_) {
                file_.open(file_name_, std::ios::out);
                for (int i = 1, x = 0; i <= opt; ++i)
                    file_.write(reinterpret_cast<char *>(&x), sizeof(int));
                cnt = 0;
            } else {
                file_.read(reinterpret_cast<char *>(&cnt), sizeof(int));
            }
            file_.close();
        }

        ~FileStack() {
            file_.open(file_name_);
            file_.write(reinterpret_cast<char*>(&cnt), sizeof(int));
            file_.close();
        }

        void push(const T &tmp) {
            file_.open(file_name_);
            file_.seekp(sizeofI + cnt * sizeofT);
            file_.write(reinterpret_cast<const char *>(&tmp), sizeofT);
            ++cnt;
            file_.close();
        }

        void gettop(T &tmp) {
            file_.open(file_name_);
            file_.seekg(sizeofI + (cnt - 1) * sizeofT);
            file_.read(reinterpret_cast<char *>(&tmp), sizeofT);
            file_.close();
        }

        void pop() { --cnt; }

        bool empty() { return !cnt; }

        int size() { return cnt; }
};

#endif