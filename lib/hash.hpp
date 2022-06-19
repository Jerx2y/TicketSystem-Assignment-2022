#ifndef TICKET_SYSTEM_HASH_HPP
#define TICKET_SYSTEM_HASH_HPP

#include "utils.hpp"

class hash_varchar {
public:
	size_t operator () (const Varchar &var) const {
        return var.var;
	}
};

class hash_vv {
public:
	size_t operator () (const std::pair<Varchar, Varchar> &var) const {
        return var.first.var ^ var.second.var;
	}
};

class hash_vdi {
public:
	size_t operator () (const std::pair<std::pair<Varchar, Date>, int> &var) const {
        return var.first.first.var ^ var.first.second.now ^ var.second;
	}
};

class hash_vi {
public:
	size_t operator () (const std::pair<Varchar, int> &var) const {
        return var.first.var ^ var.second;
	}
};

#endif