//
// Created by WYH on 2022/2/20.
//

#ifndef TICKET_SYSTEM_VECTOR_HPP
#define TICKET_SYSTEM_VECTOR_HPP

#include "exception.hpp"
#include <climits>
#include <cstddef>

namespace sjtu
{
/**
 * a data container like std::vector
 * store data in a successive memory and support random access.
 */
    template<typename T>
    class vector {
    public:
        /**
         * TODO
         * a type for actions of the elements of a vector, and you should write
         *   a class named const_iterator with same interfaces.
         */
        /**
         * you can see RandomAccessIterator at CppReference for help.
         */

        class const_iterator;

        class iterator {

            // The following code is written for the C++ type_traits library.
            // Type traits is a C++ feature for describing certain properties of a type.
            // For instance, for an iterator, iterator::value_type is the type that the
            // iterator points to.
            // STL algorithms and containers may use these type_traits (e.g. the following
            // typedef) to work properly. In particular, without the following code,
            // @code{std::sort(iter, iter1);} would not compile.
            // See these websites for more information:
            // https://en.cppreference.com/w/cpp/header/type_traits
            // About value_type: https://blog.csdn.net/u014299153/article/details/72419713
            // About iterator_category: https://en.cppreference.com/w/cpp/iterator

        public:
            using difference_type = std::ptrdiff_t;
            using value_type = T;
            using pointer = T*;
            using reference = T&;
            using iterator_category = std::output_iterator_tag;

        private:
            int pos = 0;
            T **beg = nullptr;

        public:
            /**
             * return a new iterator which pointer n-next elements
             * as well as operator-
             */
            iterator(T **_beg = nullptr, int _pos = 0) { beg = _beg, pos = _pos; }

            iterator operator+(const int &n) const {
                return iterator(beg, pos + n);
            }

            iterator operator-(const int &n) const {
                return iterator(beg, pos - n);
            }

            // return the distance between two iterators,
            // if these two iterators point to different vectors, throw invaild_iterator.

            int operator-(const iterator &rhs) const {
                if (beg != rhs.beg)
                    throw Exception("vector.h: invalid_iterator");
                return pos - rhs.pos;
            }

            iterator& operator+=(const int &n) {
                pos += n;
                return *this;
            }

            iterator& operator-=(const int &n) {
                pos -= n;
                return *this;
            }
            
            iterator operator++(int) {
                iterator tmp = *this;
                pos++;
                return tmp;
            }

            iterator& operator++() {
                pos++;
                return *this;
            }

            iterator operator--(int) {
                iterator tmp = *this;
                pos--;
                return tmp;
            }

            iterator& operator--() {
                pos--;
                return *this;
            }

            T& operator*() const {
                return *(*beg + pos);
            }

            /*
             * a operator to check whether two iterators are same (pointing to the same memory address).
             */

            bool operator==(const iterator &rhs) const {
                return beg == rhs.beg && pos == rhs.pos;
            }

            bool operator==(const const_iterator &rhs) const {
                return beg == rhs.beg && pos == rhs.pos;
            }

            bool operator!=(const iterator &rhs) const {
                return beg != rhs.beg || pos != rhs.pos;
            }

            bool operator!=(const const_iterator &rhs) const {
                return beg != rhs.beg || pos != rhs.pos;
            }

            const int getp() const {
                return pos;
            }
        };

        class const_iterator {
        public:
            using difference_type = std::ptrdiff_t;
            using value_type = T;
            using pointer = T*;
            using reference = T&;
            using iterator_category = std::output_iterator_tag;

        private:
            int pos = 0;
            const T * const*beg = nullptr;

        public:
            /**
             * return a new iterator which pointer n-next elements
             * as well as operator-
             */
            const_iterator(const T * const* _beg = nullptr, int _pos = 0) { beg = _beg, pos = _pos; }

            const_iterator operator+(const int &n) const {
                return const_iterator(beg, pos + n);
            }

            const_iterator operator-(const int &n) const {
                return const_iterator(beg, pos - n);
            }

            // return the distance between two iterators,
            // if these two iterators point to different vectors, throw invaild_iterator.

            int operator-(const const_iterator &rhs) const {
                if (beg != rhs.beg)
                    throw Exception("vector.h: invalid_iterator");
                return pos - rhs.pos;
            }

            const_iterator& operator+=(const int &n) {
                pos += n;
                return *this;
            }

            const_iterator& operator-=(const int &n) {
                pos -= n;
                return *this;
            }
            
            const_iterator operator++(int) {
                const_iterator tmp = *this;
                pos++;
                return tmp;
            }

            const_iterator& operator++() {
                pos++;
                return *this;
            }

            const_iterator operator--(int) {
                const_iterator tmp = *this;
                pos--;
                return tmp;
            }

            const_iterator& operator--() {
                pos--;
                return *this;
            }

            const T& operator*() const {
                return *(*beg + pos);
            }

            /**
             * a operator to check whether two iterators are same (pointing to the same memory address).
             */

            bool operator==(const iterator &rhs) const {
                return beg == rhs.beg && pos == rhs.pos;
            }

            bool operator==(const const_iterator &rhs) const {
                return beg == rhs.beg && pos == rhs.pos;
            }

            bool operator!=(const iterator &rhs) const {
                return beg != rhs.beg || pos != rhs.pos;
            }

            bool operator!=(const const_iterator &rhs) const {
                return beg != rhs.beg || pos != rhs.pos;
            }

            const int getp() const {
                return pos;
            }

        };

    private:
        int size_, all_;
        T *arr_ = nullptr;

    public:

        vector() {
            size_ = 0, all_ = 3;
            arr_ = static_cast<T*> (operator new(all_ * sizeof(T)));
        }

        vector(const vector &other) {
            size_ = other.size_;
            all_ = other.all_;
            arr_ = static_cast<T*> (operator new(all_ * sizeof(T)));
            for (int i = 0; i < size_; ++i)
                new (&arr_[i]) T(other.arr_[i]);
        }

        ~vector() {
            for (int i = size_ - 1; i >= 0; --i)
                arr_[i].~T();
            operator delete(arr_);
            all_ = size_ = 0;
        }

        vector &operator=(const vector &other) {
            if (this == &other)
                return *this;
            for (int i = size_ - 1; i >= 0; --i)
                arr_[i].~T();
            operator delete(arr_);
            size_ = other.size_;
            all_ = other.all_;
            arr_ = static_cast<T*> (operator new(all_ * sizeof(T)));
            for (int i = 0; i < size_; ++i)
                new (&arr_[i]) T(other.arr_[i]);
            return *this;
        }

        /**
         * assigns specified element with bounds checking
         * throw index_out_of_bound if pos is not in [0, size_)
         */

        T & at(const size_t &pos) {
            if (pos < 0 || pos >= size_)
                throw Exception("vector.h: index_out_of_bound");
            return arr_[pos];
        }

        const T & at(const size_t &pos) const {
            if (pos < 0 || pos >= size_)
                throw Exception("vector.h: index_out_of_bound");
            return arr_[pos];
        }

        /**
         * assigns specified element with bounds checking
         * throw index_out_of_bound if pos is not in [0, size_)
         * !!! Pay attentions
         *   In STL this operator does not check the boundary but I want you to do.
         */

        T & operator[](const size_t &pos) {
            if (pos < 0 || pos >= size_)
                throw Exception("vector.h: index_out_of_bound");
            return arr_[pos];
        }

        const T & operator[](const size_t &pos) const {
            if (pos < 0 || pos >= size_)
                throw Exception("vector.h: index_out_of_bound");
            return arr_[pos];
        }

        /**
         * access the first element.
         * throw container_is_empty if size_ == 0
         */

        const T & front() const {
            if (!size_)
                throw Exception("container_is_empty");
            return arr_[0];
        }

        /**
         * access the last element.
         * throw container_is_empty if size_ == 0
         */

        const T & back() const {
            if (!size_)
                throw Exception("container_is_empty");
            return arr_[size_ - 1];
        }

        iterator begin() {
            return iterator(&arr_, 0);
        }

        const_iterator cbegin() const {
            return const_iterator(&arr_, 0);
        }

        iterator end() {
            return iterator(&arr_, size_);
        }

        const_iterator cend() const {
            return const_iterator(&arr_, size_);
        }

        bool empty() const {
            return !size_;
        }

        size_t size() const {
            return size_;
        }

        void clear() {
            for (int i = size_ - 1; i >= 0; --i)
                arr_[i].~T();
            operator delete(arr_);
            size_ = 0, all_ = 3;
            arr_ = static_cast<T*> (operator new(3 * sizeof(T)));
        }

        /**
         * inserts value before pos
         * returns an iterator pointing to the inserted value.
         */

        void doubleLength() {
            all_ <<= 1;
            T *tmp = arr_;
            arr_ = static_cast<T*> (operator new(all_ * sizeof(T)));
            for (int i = 0; i < size_; ++i)
                new (&arr_[i]) T(tmp[i]);
            for (int i = size_ - 1; i >= 0; --i)
                tmp[i].~T();
            operator delete(tmp);
        }

        iterator insert(iterator pos, const T &value) {
            int ind = pos.getp();
            if (size_ == all_) doubleLength();
            ++size_;
            for (int i = size_ - 1; i > ind; --i)
                arr_[i] = arr_[i - 1];
            new (&arr_[ind]) T(value);
            return iterator(&arr_, ind);
        }

        /**
         * inserts value at index ind.
         * after inserting, this->at(ind) == value
         * returns an iterator pointing to the inserted value.
         * throw index_out_of_bound if ind > size_ (in this situation ind can be size_ because after inserting the size_ will increase 1.)
         */

        iterator insert(const size_t &ind, const T &value) {
            if (ind > size_)
                throw Exception("index_out_of_bound");
            if (size_ == all_) doubleLength();
            ++size_;
            for (int i = size_ - 1; i > ind; --i)
                arr_[i] = arr_[i - 1];
            new (&arr_[ind]) T(value);
            return iterator(&arr_, ind);
        }

        /**
         * removes the element at pos.
         * return an iterator pointing to the following element.
         * If the iterator pos refers the last element, the end() iterator is returned.
         */

        iterator erase(iterator pos) {
            int ind = pos.getp();
            arr_[ind].~T();
            for (int i = ind; i < size_ - 1; ++i)
                arr_[i] = arr_[i + 1];
            --size_;
            return iterator(&arr_, ind);
        }

        /**
         * removes the element with index ind.
         * return an iterator pointing to the following element.
         * throw index_out_of_bound if ind >= size_
         */

        iterator erase(const size_t &ind) {
            if (ind >= size_)
                throw Exception("index_out_of_bound");
            arr_[ind].~T();
            for (int i = ind; i < size_ - 1; ++i)
                arr_[i] = arr_[i + 1];
            --size_;
            return iterator(&arr_, ind);
        }

        /**
         * adds an element to the end.
         */

        void push_back(const T &value) {
            if (size_ == all_) doubleLength();
            new (&arr_[size_]) T(value);
            ++size_;
        }

        /**
         * remove the last element from the end.
         * throw container_is_empty if size_() == 0
         */

        void pop_back() {
            if (!size_)
                throw Exception("container_is_empty");
            arr_[size_- 1].~T();
            --size_;
        }

    };


}

#endif