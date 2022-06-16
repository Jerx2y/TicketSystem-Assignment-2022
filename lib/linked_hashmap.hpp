#ifndef TICKET_SYSTEM_HASHMAP_HPP
#define TICKET_SYSTEM_HASHMAP_HPP

// only for std::equal_to<T> and std::hash<T>
#include <functional>
#include <cstddef>

#include "utils.hpp"

template<
	class Key,
	class T,
	class Hash = std::hash<Key>, 
	class Equal = std::equal_to<Key>
> class linked_hashmap {
public:

	typedef pair<const Key, T> value_type;
	size_t Capacity;
	const double LoadFactor;

	struct node {
		value_type *val = nullptr;
		node *prev, *next;
		node *l_prev, *l_next;
		node() { prev = next = l_prev = l_next = nullptr; }
		node(const value_type &_val) {
			prev = next = l_prev = l_next = nullptr;
			val = new value_type(_val);
		}
		~node() {
			delete val;
		}
	};
	
	node *head;
	node *l_head, *l_tail;
	size_t cnt;

	linked_hashmap() : LoadFactor(0.5) {
		Capacity = 29;
		cnt = 0;
		head = new node[Capacity];
		l_head = new node, l_tail = new node;
		l_head->l_next = l_tail;
		l_tail->l_prev = l_head;
	}

	linked_hashmap(const linked_hashmap &other) : LoadFactor(0.5) {
		Capacity = other.Capacity;
		cnt = 0;
		head = new node[Capacity];
		l_head = new node, l_tail = new node;
		l_head->l_next = l_tail;
		l_tail->l_prev = l_head;
		l_move(other.l_head, other.l_tail);
	}

	void rebuild() {
		for (node *p = l_head->l_next; p != l_tail; p = p->l_next) {
			size_t tmp = Hash()(p->val->first) % Capacity;
			p->next = head[tmp].next;
			p->prev = &head[tmp];
			if (head[tmp].next)
				head[tmp].next->prev = p;
			head[tmp].next = p;
		}
	}

	void extend() {
		if (cnt <= Capacity * LoadFactor) return ;
		Capacity = Capacity * 2 + 1; 
		delete[] head;
		head = new node[Capacity];
		rebuild();
	}

	void shrink() {
		if (cnt > Capacity * LoadFactor * 0.25) return ;
		Capacity = (Capacity - 1) / 2;
		delete[] head;
		head = new node[Capacity];
		rebuild();
	}

	void l_move(node *hd, node *tl) {
		for (node *p = hd->l_next; p != tl; p = p->l_next)
			insert(*(p->val));
	}
 
	/**
	 * TODO assignment operator
	 */
	linked_hashmap & operator=(const linked_hashmap &other) {
		if (&other == this) return *this;
		destruct();
		Capacity = other.Capacity;
		cnt = 0;
		head = new node[Capacity];
		l_head = new node, l_tail = new node;
		l_head->l_next = l_tail;
		l_tail->l_prev = l_head;
		l_move(other.l_head, other.l_tail);
		return *this;
	}
 
	/**
	 * TODO Destructors
	 */
	~linked_hashmap() {
		destruct();
	}

	void destruct() {
		clear();
		delete[] head;
		delete l_head;
		delete l_tail;
	}
 
	/**
	 * TODO
	 * access specified element with bounds checking
	 * Returns a reference to the mapped value of the element with key equivalent to key.
	 * If no such element exists, an exception of type `index_out_of_bound'
	 */
	T & at(const Key &key) {
		size_t tmp = Hash()(key);
		for (node *p = head[tmp % Capacity].next; p; p = p->next)
			if (Equal()(p->val->first, key)) return p->val->second;
		// throw Exception("index_out_of_bound");
	}

	const T & at(const Key &key) const {
		size_t tmp = Hash()(key);
		for (node *p = head[tmp % Capacity].next; p; p = p->next)
			if (Equal()(p->val->first, key)) return p->val->second;
		// throw Exception("index_out_of_bound");
	}
 
	/**
	 * TODO
	 * access specified element 
	 * Returns a reference to the value that is mapped to a key equivalent to key,
	 *   performing an insertion if such key does not already exist.
	 */
	T & operator[](const Key &key) {
		size_t tmp = Hash()(key);
		for (node *p = head[tmp % Capacity].next; p; p = p->next)
			if (Equal()(p->val->first, key)) return p->val->second;
		auto it = insert(pair(key, T()));
		return it.first->second;
	}
 
	/**
	 * behave like at() throw index_out_of_bound if such key does not exist.
	 */
	const T & operator[](const Key &key) const {
		return at(key);
	}
 
	bool empty() const { return cnt == 0; }
 
	/**
	 * returns the number of elements.
	 */
	size_t size() const { return cnt;}
 
	/**
	 * clears the contents
	 */
	void clear() {
		delete[] head;
		for (node *p = l_head->l_next; p != l_tail; ) {
			node *tmp = p;
			p = p->l_next;
			delete tmp;
		}
		l_head->l_next = l_tail;
		l_tail->l_prev = l_head;
		Capacity = 29;
		cnt = 0;
		head = new node[Capacity];
	}
 
	/**
	 * insert an element.
	 * return a pair, the first of the pair is
	 *   the iterator to the new element (or the element that prevented the insertion), 
	 *   the second one is true if insert successfully, or false.
	 */
	bool insert(const value_type &value) {
		size_t tmp = Hash()(value.first) % Capacity;
		for (node *p = head[tmp].next; p; p = p->next)
			if (Equal()(p->val->first, value.first))
				return false;
		node *now = new node(value);
		l_tail->l_prev->l_next = now;
		now->l_prev = l_tail->l_prev;
		now->l_next = l_tail;
		l_tail->l_prev = now;
		now->next = head[tmp].next;
		now->prev = &head[tmp];
		if (head[tmp].next)
			head[tmp].next->prev = now;
		head[tmp].next = now;
		++cnt;
		extend();
		return true;
	}
 
	/**
	 * erase the element at pos.
	 *
	 * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
	 */
	bool erase(const Key &k) {
		size_t t = Hash()(k) % Capacity;
		node *tmp = nullptr;
		for (node *p = head[t].next; p; p = p->next)
			if (Equal()(p->val->first, k)) {
				tmp = p;
				break;
			}
		if (tmp == nullptr) return false;
		tmp->l_next->l_prev = tmp->l_prev;
		tmp->l_prev->l_next = tmp->l_next;
		tmp->prev->next = tmp->next;
		if (tmp->next)
			tmp->next->prev = tmp->prev;
		delete tmp;
		--cnt;
		shrink();
		return true;
	}
 
	/**
	 * Returns the number of elements with key 
	 *   that compares equivalent to the specified argument,
	 *   which is either 1 or 0 
	 *     since this container does not allow duplicates.
	 */
	size_t count(const Key &key) const {
		size_t tmp = Hash()(key);
		for (node *p = head[tmp % Capacity].next; p; p = p->next)
			if (Equal()(p->val->first, key)) return 1;
		return 0;
	}
 
};


#endif
