#ifndef TICKET_SYSTEM_EXCEPTION_HPP
#define TICKET_SYSTEM_EXCEPTION_HPP

#include <iostream>

class Exception {
  private:
    std::string message;
  public:
    Exception() { }
    Exception(std::string str) : message(str) { }
    std::string what() { return message; }
};

#endif