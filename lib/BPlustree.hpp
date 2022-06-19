#ifndef TRAINTICKET2022_BPLUSTREE_HPP
#define TRAINTICKET2022_BPLUSTREE_HPP
#include<fstream>
#include <functional>
#include "vector.hpp"
using ll = size_t;
using sjtu::vector;
using std::ios;
using std::fstream;
namespace lailai {
    template<class K,class S, class Compare = std::less<K>> class BPT;
    template<class K, class S, int needCache = 0, class HASH = std::hash<K>, class Compare = std::less<K>,class Com=std::less<S>>
    class map{//可以直接在map层面做缓存，用linkedehashmap实现；查找key对应的value的时候可以直接从内存里读取；
    private:
        Compare compare;
        Com com;
        BPT<K,S> bpt;
        struct Node{
            K key_;
            S value_;
            Node (){}
            Node (const K &key,const S &value){
                key_ = key;
                value_=value;
            }
        };
    public:
        map(const std::string &file):bpt(file+".idx"){}

        ~map(){}
        class Cache{
       public:
            class node{
            public:
                K key_;
                S value_;
                node *nxt_= nullptr;
            public:
                node(const K &key,const S &value,node *p){
                    key_=key;
                    value_=value;
                    nxt_=p;
                }
            };
            const int Size = 103;
            int total = 0;
            node **array;
            Compare compare;
            HASH hash;
        public:
            Cache():total(0),Size(needCache){
                array = new node*[Size];
                for(int i = 0; i < Size; ++i){
                    array[i]= nullptr;
                }
            }
            ~Cache(){
                for(int i =0; i < Size; ++i){
                    node * p=array[i];
                    node *q=p;
                    if (p == nullptr)
                        continue;
                    while(p->nxt_!= nullptr){
                        q=p;
                        p=p->nxt_;
                        delete q;
                    }
                    delete p;
                }
            };
            bool find(const K &key , S &value){
                int index = hash(key)%Size;
                if(array[index]!= nullptr){
                    node *p = array[index];
                    while(p->nxt_!= nullptr&&(compare(p->key_,key)||compare(key,p->key_)))p=p->nxt_;
                    if(!compare(p->key_,key)&&!compare(key,p->key_)){
                        value = p->value_;
                        return true;
                    }
                    return false;
                }
                return false;
            }
            void set(const K &key, const S &value){
                int index = hash(key)%Size;
                if(array[index]== nullptr){
                    node *p=new node(key,value, nullptr);
                    array[index]=p;
                    ++total;
                }
                else{
                    node *p=array[index];
                    while(p->nxt_!= nullptr&&(compare(p->key_,key)||compare(key,p->key_)))p=p->nxt_;
                    if(!compare(p->key_,key)&&!compare(key,p->key_))p->value_=value;
                    else{
                        p->nxt_=new node(key,value, nullptr);
                        ++total;
                    }
                }
                if(total > Size){
                    int i;
                    for(i=0;i<Size;++i)if(array[i]!= nullptr)break;
                    node *p=array[i];
                    array[i]=p->nxt_;
                    delete p;
                    --total;
                }
            }
            void remove(const K &key, const S &value){
//                std::cout << "here_remove" <<std::endl;
                int index = hash(key)%Size;
                if(array[index]!= nullptr){
                    node *p=array[index];
                    node *q=array[index];
                    while(p->nxt_!= nullptr&&(compare(p->key_,key)||compare(key,p->key_))){
                        q=p;
                        p=p->nxt_;
                    }
                    if(compare(p->key_,key)||compare(key,p->key_)){
                        return;
                    }
                    else{
                        --total;
                        if(p==q){
                            array[index]=p->nxt_;
                            delete p;
                        }
                        else{
                            q->nxt_=p->nxt_;
                            delete p;
                        }
                    }
                }
            }
        }cache;

        void Get(const K &left,const K &right ,vector<S> &v){
            v.clear();
            bpt.Get(left,right,v);
        }
        bool Getone(const K &key, S &value){
            if (needCache) {
                if(cache.find(key,value))return true;
                bool flag = bpt.Getone(key,value);
                if(!flag)return false;
                else{
                    cache.set(key,value);
                    return true;
                }
            } else return bpt.Getone(key, value);
        }
        int count(const K &key) {
            S v;
            if (needCache && cache.find(key, v))
                return true;
            return bpt.Getone(key, v);
        }
        void Modify(const K &key, const S &value) { // need to be changed
            if (needCache)
                cache.set(key,value);
            bpt.Modify(key,value);
        }
        void Insert(const K &key,const S &value){
            if (needCache) 
                cache.set(key,value);
            bpt.Insert(key,value);
        }
        bool Remove(const K &key, const S &value){
//            std::cout << "remove cache before" << std::endl;
            if (needCache)
                cache.remove(key,value);
//            std::cout << "remove cache finished" << std::endl;
            return bpt.Remove(key,value);
        }
    };
    template<class K,class S,class Compare>
    class BPT {
    private:
        constexpr static int max(ll x, ll y){return (x > y) ? x : y;}
        static constexpr int BLOCK_SIZE = max(8192/(sizeof(K)*2),10ll);
        static constexpr int LEAVE_SIZE =max(8192/(sizeof(K)+sizeof(S)),10ll);
        static constexpr int LEAVE_SPLIT_LEFT=LEAVE_SIZE/2;
        static constexpr int BLOCK_SPLIT_LEFT=BLOCK_SIZE/2;
        static constexpr int BLOCK_MIN = BLOCK_SPLIT_LEFT/2;
        static constexpr int LEAVE_MIN = LEAVE_SPLIT_LEFT/2;
    public:
        class Node {//叶子节点内-块链-点
        public://维护第二关键字
            S value;
            K key;
        public:
            void operator=(const Node &x) {
                value = x.value;
                key = x.key;
            }

            Node() {};

            Node(const K &key_, const S &value_) : value(value_), key(key_) {
            };
        };
    private:
        //内嵌类
        class Block {//树上节点
            friend class BPT;

        private:
            bool isbottom = false;
            ll son[BLOCK_SIZE + 5];
            K key[BLOCK_SIZE + 5];
            int num = 0;
            std::pair<Block *, ll> fa;//存储父节点，内存地址
        public:
            Block() {};
        };

        class Leave {//叶子结点-块链-块
            friend class BPT;
            int num = 0;
        private:
            std::pair<Block *, ll> fa;
            Node array[LEAVE_SIZE + 5];
        public:
            Leave() {};
        };

        struct KVblock {
            ll file_index = 0;
            K key;
        };

        struct KVleave {
            ll file_index = 0;
            K key;
        };

        const std::string file_name;

        std::fstream fileIndex;

        Block root;

        Compare com;

        class basicManager{
        public:
            int totalblock=1;

            int totalleave=0;

            ll index_root;

            int stack_block[500];

            int stack_leave[500];

            int top_block=0;

            int top_leave=0;
        }basicManager;


        bool compare(const Node &n1, const Node &n2) {
            if (com(n1.key, n2.key))return true;
            if (com(n2.key, n1.key))return false;
            return n1.value < n2.value;

        }

        void recycle_leave(ll index) {
            if(basicManager.top_leave<1500-1)basicManager.stack_leave[++basicManager.top_leave]=index;
        }

        void recycle_block(ll index) {
            if(basicManager.top_block<1500-1)basicManager.stack_block[++basicManager.top_block]=index;
        }

        ll add_one_leave() {//提供内存地址
            if(!basicManager.top_leave){
                ll index = basicManager.totalblock * sizeof(Block) + basicManager.totalleave * sizeof(Leave) + sizeof(basicManager);
                ++basicManager.totalleave;
                return index;
            }
            return basicManager.stack_leave[basicManager.top_leave--];
        }

        ll add_one_block() {
            if(!basicManager.top_block){
                ll index = basicManager.totalblock * sizeof(Block) + basicManager.totalleave * sizeof(Leave) + sizeof(basicManager);
                ++basicManager.totalblock;
                return index;
            }
            return basicManager.stack_block[basicManager.top_block--];
        }

        void modify_father(const Leave &l, const K &key) {
            Block *p = l.fa.first;
            if (l.fa.second) {
                l.fa.first->key[l.fa.second] = key;
                return;
            }
            while (p != &root && !p->fa.second) {
                p = p->fa.first;
            }
            if (p == &root)return;
            p->fa.first->key[p->fa.second] = key;
        }

        bool find_one_block(Node &n,const Block &b){
            if(!b.num)return false;
            int i;
            for(i = 0; i < b.num; ++i){
                if(com(n.key,b.key[i+1]))break;
            }
            if(b.isbottom){
                ll son_index = b.son[i];
                fileIndex.seekg(son_index);
                Leave son;
                fileIndex.read(reinterpret_cast<char *>(&son), sizeof(Leave));
                return find_one_leave(n,son);
            }
            else{
                ll son_index = b.son[i];
                fileIndex.seekg(son_index);
                Block son;
                fileIndex.read(reinterpret_cast<char *>(&son), sizeof(Block));
                return find_one_block(n,son);
            }
        }
        bool find_one_leave(Node &n, const Leave &l){
            if(!l.num)return false;
            int i;
            for( i = 0; i < l.num; ++i){
                if(com(n.key,l.array[i+1].key))break;
            }
            if(!i)return false;
            if(!com(n.key,l.array[i].key)&&!com(l.array[i].key,n.key)){
                n.value=l.array[i].value;
                return true;
            }
            return false;
        }
        bool find_one(Node &n){
            return find_one_block(n,root);
        }
        void find_list_l(const Leave &le, const K &left,const K &right, vector<S> &v) {
            for (int i = 1; i <= le.num; ++i) {
                if (!com(le.array[i].key, left) && !com(right, le.array[i].key))v.push_back(le.array[i].value);//不比left小不比right大
            }
        }

        void find_list_b(const Block &b, const K &left,const K &right, vector<S> &v) {
            if(!b.num)return;
            if (!b.isbottom) {
                int i;
                for (i = 0; i < b.num; ++i) {
                    if (com(left, b.key[i + 1]) || !com(left, b.key[i + 1]) && !com(b.key[i + 1], left))
                        break;//下一个位置关键字大于等于
                }
                while (i <= b.num) {
                    ll index_son = b.son[i];
                    Block son;
                    fileIndex.seekg(index_son);
                    fileIndex.read(reinterpret_cast<char *>(&son), sizeof(Block));
                    find_list_b(son, left,right, v);
                    ++i;
                    if (i > b.num || com(right, b.key[i]))break;//新的位置关键字大于key
                }
            } else {
                int i;
                for (i = 0; i < b.num; ++i) {
                    if (com(left, b.key[i + 1]) || !com(left, b.key[i + 1]) && !com(b.key[i + 1], left))break;
                }
                while (i <= b.num) {
                    ll index_son = b.son[i];
                    Leave son;
                    fileIndex.seekg(index_son);
                    fileIndex.read(reinterpret_cast<char *>(&son), sizeof(Leave));
                    find_list_l(son, left,right, v);
                    ++i;
                    if (i > b.num || com(right, b.key[i]))break;
                }
            }

        }

        void insert(const K &key, const S &value) {
            Node n(key, value);
            KVblock pair_;
            if (biinsert(root, n, basicManager.index_root, pair_)) {//根节点的特判
                Block newroot;
                newroot.num = 1;
                ll new_root_index = add_one_block();
                newroot.son[0] = basicManager.index_root;
                newroot.son[1] = pair_.file_index;
                newroot.key[1] = pair_.key;
                basicManager.index_root = new_root_index;
                root = newroot;
                fileIndex.seekg(new_root_index);
                fileIndex.write(reinterpret_cast<char *>(&newroot), sizeof(Block));
                fileIndex.seekg(2 * sizeof(int));
                fileIndex.write(reinterpret_cast<char *>(&basicManager.index_root), sizeof(ll));
            }
        }

        void SpilitBlock(ll index, KVblock &pair) {
            Block b, newb;
            fileIndex.seekg(index);//移动指针到指定位置
            fileIndex.read(reinterpret_cast<char *>(&b), sizeof(Block));
            for (int i = BLOCK_SPLIT_LEFT + 2; i <= b.num; ++i) {//复制过程
                newb.key[i - BLOCK_SPLIT_LEFT - 1] = b.key[i];
            }
            for (int i = BLOCK_SPLIT_LEFT + 1; i <= b.num + 1; ++i) {//复制过程
                newb.son[i - BLOCK_SPLIT_LEFT - 1] = b.son[i];
            }
            pair.key = b.key[BLOCK_SPLIT_LEFT + 1];
            //修改元素数量
            newb.num = b.num - BLOCK_SPLIT_LEFT - 1;
            b.num = BLOCK_SPLIT_LEFT;
            newb.isbottom = b.isbottom;
            ll new_index = add_one_block();
            fileIndex.seekg(new_index);
            fileIndex.write(reinterpret_cast<char *>(&newb), sizeof(Block));
            fileIndex.seekg(index);
            fileIndex.write(reinterpret_cast<char *>(&b), sizeof(Block));
            pair.file_index = new_index;
        }

        bool biinsert(Block &b, const Node &key, const ll &now_index, KVblock &pair_) {//返回值表示下层是否裂块，这层需要添加键值。
            if (b.num == 0) {//key个数为0
                b.isbottom = true;
                ++b.num;
                Leave l, r;
                b.key[1] = key.key;
                b.son[0] = add_one_leave();
                b.son[1] = add_one_leave();
                ++r.num;
                r.array[r.num] = key;
                fileIndex.seekg(now_index);
                fileIndex.write(reinterpret_cast<char *>(&b), sizeof(Block));
                fileIndex.seekg(b.son[1]);
                fileIndex.write(reinterpret_cast<char *>(&r), sizeof(Leave));
                fileIndex.seekg(b.son[0]);
                fileIndex.write(reinterpret_cast<char *>(&l), sizeof(Leave));
                return false;
            }
            int index;
            for (index = 0; index < b.num; ++index) {
                //debug
                if (com(key.key, b.key[index + 1]))break;//下一个大于自己就取当前所在位置，退出循环
            }
            ll son_index = b.son[index];
            if (!b.isbottom) {//非叶子结点的父节点
                Block bl;//todoxx
                fileIndex.seekg(son_index);
                fileIndex.read(reinterpret_cast<char *>(&bl), sizeof(Block));
                bl.fa.first = &b;
                bl.fa.second = index;
                if (biinsert(bl, key, son_index, pair_)) {
                    ++b.num;
                    for (int i = b.num; i >= index + 2; --i) {
                        b.key[i] = b.key[i - 1];
                    }
                    for (int i = b.num + 1; i >= index + 2; --i) {
                        b.son[i] = b.son[i - 1];
                    }
                    b.key[index + 1] = pair_.key;
                    b.son[index + 1] = pair_.file_index;
                    fileIndex.seekg(now_index);
                    fileIndex.write(reinterpret_cast<char *>(&b), sizeof(Block));
                    if (b.num > BLOCK_SIZE) {//分裂block节点
                        SpilitBlock(now_index, pair_);
                        return true;
                    } else {
                        return false;
                    }
                } else {
                    fileIndex.seekg(now_index);
                    fileIndex.write(reinterpret_cast<char *>(&b), sizeof(Block));
                    return false;
                }

            } else {//是叶子节点的父节点
                Leave le;
                KVleave pair;
                reset();
                fileIndex.seekg(son_index);
                fileIndex.read(reinterpret_cast<char *>(&le), sizeof(Leave));
                le.fa.first = &b;
                le.fa.second = index;
                if (leinsert(le, key, pair, son_index)) {//叶子裂块，儿子增加
                    ++b.num;
                    for (int i = b.num; i > index + 1; --i) {
                        b.key[i] = b.key[i - 1];
                    }
                    for (int i = b.num + 1; i > index + 1; --i) {
                        b.son[i] = b.son[i - 1];
                    }
                    b.key[index + 1] = pair.key;
                    b.son[index + 1] = pair.file_index;
                    if (b.num > BLOCK_SIZE) {
                        fileIndex.seekg(now_index);
                        fileIndex.write(reinterpret_cast<char *>(&b), sizeof(Block));
                        SpilitBlock(now_index, pair_);
                        return true;
                    } else {
                        reset();
                        fileIndex.seekg(now_index);
                        fileIndex.write(reinterpret_cast<char *>(&b), sizeof(Block));
                        return false;
                    }
                } else {//叶子未裂块，儿子不增加，没有裂块可能性,但是key可能修改，所以写回
                    fileIndex.seekg(now_index);
                    fileIndex.write(reinterpret_cast<char *>(&b), sizeof(Block));
                    return false;
                }
            }

        }

        void splitLeave(ll index, KVleave &pair) { //return new block's fileindex
            Leave le, newle;
            fileIndex.seekg(index);//移动指针到指定位置
            fileIndex.read(reinterpret_cast<char *>(&le), sizeof(Leave));
            //debug
            for (int i = LEAVE_SPLIT_LEFT + 1; i <= le.num; ++i) {//复制过程
                newle.array[i - LEAVE_SPLIT_LEFT] = le.array[i];
            }
            pair.key = newle.array[1].key;
            pair.file_index = add_one_leave();
            //修改元素数量
            newle.num = le.num - LEAVE_SPLIT_LEFT;
            le.num = LEAVE_SPLIT_LEFT;
            //本身是末位节点
            //写入两个block
            fileIndex.seekg(pair.file_index);
            fileIndex.write(reinterpret_cast<char *>(&newle), sizeof(Leave));
            fileIndex.seekg(index);
            fileIndex.write(reinterpret_cast<char *>(&le), sizeof(Leave));
            //不是末位节点
        };

        bool leinsert(Leave &le, const Node &key,
                      KVleave &pair, const ll &now_index) {//true = spilited
            //todo modify father
            //----插入新的kv对
            int i;
            for (i = 0; i < le.num; ++i) {
                if (com(key.key, le.array[i + 1].key))break;
            }
            if (i && !com(key.key, le.array[i].key) && !com(le.array[i].key, key.key))return false;
            else {
                ++le.num;
                for (int j = le.num; j > i + 1; --j) {
                    le.array[j] = le.array[j - 1];
                }
                le.array[i + 1] = key;
                if (!i) {
                    modify_father(le, key.key);
                }
            }
            //-----插入结束-----是否裂块
            if (le.num > LEAVE_SIZE) {
                fileIndex.seekg(now_index);
                fileIndex.write(reinterpret_cast<char *>(&le), sizeof(Leave));
                splitLeave(now_index, pair);
                return true;//裂块
            } else {
                reset();
                fileIndex.seekg(now_index);
                fileIndex.write(reinterpret_cast<char *>(&le), sizeof(Leave));//重新写回文件
                return false;
            }
        }

        void get_one_child_r_b(Block &b, Block &r) {//if b is the first child
            b.key[++b.num] = b.fa.first->key[1];
            b.son[b.num] = r.son[0];
            b.fa.first->key[1] = r.key[1];
            for (int i = 1; i < r.num; ++i) {
                r.key[i] = r.key[i + 1];
            }
            for (int i = 0; i < r.num; ++i) {
                r.son[i] = r.son[i + 1];
            }
            r.num--;
        }

        void get_one_child_l_b(Block &b, Block &l) {
            b.num++;
            for (int i = b.num; i > 1; --i) {
                b.key[i] = b.key[i - 1];
            }
            for (int i = b.num; i > 0; --i) {
                b.son[i] = b.son[i - 1];
            }
            b.son[0] = l.son[l.num];
            b.key[1] = b.fa.first->key[b.fa.second];
            b.fa.first->key[b.fa.second] = l.key[l.num];
            l.num--;
        }

        void merge_r_b(Block &b, Block &r) {
            b.key[b.num+1]=b.fa.first->key[1];
            for (int i = b.num + 2; i <= b.num + r.num+1; ++i) {
                b.key[i] = r.key[i - b.num-1];
            }
            for (int i = b.num + 1; i <= b.num + r.num + 1; ++i) {
                b.son[i] = r.son[i - b.num - 1];
            }
            b.num += r.num+1;
            b.fa.first->num--;
            for (int i = 1; i <= b.fa.first->num; i++) {
                b.fa.first->key[i] = b.fa.first->key[i + 1];
                b.fa.first->son[i] = b.fa.first->son[i + 1];
            }
        }

        void merge_l_b(Block &b, Block &l) {
            l.key[l.num+1]=b.fa.first->key[b.fa.second];
            for (int i = l.num + 2; i <= b.num + l.num+1; ++i) {
                l.key[i] = b.key[i - l.num-1];
            }
            for (int i = l.num + 1; i <= b.num + l.num + 1; ++i) {
                l.son[i] = b.son[i - l.num - 1];
            }
            l.num += b.num+1;
            b.fa.first->num--;
            for(int i = b.fa.second; i <= b.fa.first->num; ++i){
                b.fa.first->key[i] = b.fa.first->key[i + 1];
                b.fa.first->son[i] = b.fa.first->son[i + 1];
            }
        }

        bool biremove(Block &b, const Node &key, ll now_index) {//true说明key值减少
            int i;
            for (i = 0; i < b.num; ++i)if(com(key.key,b.key[i+1]))break;
            ll index_son = b.son[i];
            if (!b.isbottom) {
                Block bson;
                fileIndex.seekg(index_son);
                fileIndex.read(reinterpret_cast<char *>(&bson), sizeof(Block));
                bson.fa.first = &b;
                bson.fa.second = i;
                if (!biremove(bson, key, index_son)) {
                    fileIndex.seekg(now_index);
                    fileIndex.write(reinterpret_cast<char *>(&b), sizeof(Block));
                    return false;
                } else {
                    fileIndex.seekg(now_index);
                    fileIndex.write(reinterpret_cast<char *>(&b), sizeof(Block));
                    if (b.num < BLOCK_MIN) {
                        if (!b.fa.second) {//处理右邻居
                            ll bro_index = b.fa.first->son[1];
                            Block bro;
                            fileIndex.seekg(bro_index);
                            fileIndex.read(reinterpret_cast<char *>(&bro), sizeof(Block));
                            if (bro.num > BLOCK_MIN) {
                                get_one_child_r_b(b, bro);
                                fileIndex.seekg(now_index);
                                fileIndex.write(reinterpret_cast<char *>(&b), sizeof(Block));
                                fileIndex.seekg(bro_index);
                                fileIndex.write(reinterpret_cast<char *>(&bro), sizeof(Block));
                                return false;
                            } else {
                                merge_r_b(b, bro);
                                fileIndex.seekg(now_index);
                                fileIndex.write(reinterpret_cast<char *>(&b), sizeof(Block));
                                recycle_block(bro_index);
                                return true;
                            }
                        } else {//合并左邻居
                            ll bro_index = b.fa.first->son[b.fa.second-1];
                            Block bro_l;
                            fileIndex.seekg(bro_index);
                            fileIndex.read(reinterpret_cast<char *>(&bro_l), sizeof(Block));
                            if (bro_l.num > BLOCK_MIN) {
                                get_one_child_l_b(b, bro_l);
                                fileIndex.seekg(now_index);
                                fileIndex.write(reinterpret_cast<char *>(&b), sizeof(Block));
                                fileIndex.seekg(bro_index);
                                fileIndex.write(reinterpret_cast<char *>(&bro_l), sizeof(Block));
                                return false;
                            } else {
                                merge_l_b(b, bro_l);
                                fileIndex.seekg(bro_index);
                                fileIndex.write(reinterpret_cast<char *>(&bro_l), sizeof(Block));
                                recycle_block(bro_index);
                                return true;
                            }
                        }
                    } else{
                        return false;
                    }
                }
            } else {
                Leave lson;
                fileIndex.seekg(index_son);
                fileIndex.read(reinterpret_cast<char *>(&lson), sizeof(Leave));
                lson.fa.first = &b;
                lson.fa.second = i;
                if (!leremove(lson, key, index_son)) {
                    fileIndex.seekg(now_index);
                    fileIndex.write(reinterpret_cast<char *>(&b), sizeof(Block));
                    return false;
                } else {//调整中间节点
                    fileIndex.seekg(now_index);
                    fileIndex.write(reinterpret_cast<char *>(&b), sizeof(Block));
                    if (b.num < BLOCK_MIN) {
                        if (!b.fa.second) {//合并右邻居
                            ll bro_index = b.fa.first->son[1];
                            Block bro;
                            fileIndex.seekg(bro_index);
                            fileIndex.read(reinterpret_cast<char *>(&bro), sizeof(Block));
                            if (bro.num > BLOCK_MIN) {
                                get_one_child_r_b(b, bro);
                                fileIndex.seekg(now_index);
                                fileIndex.write(reinterpret_cast<char *>(&b), sizeof(Block));
                                fileIndex.seekg(bro_index);
                                fileIndex.write(reinterpret_cast<char *>(&bro), sizeof(Block));
                                return true;
                            } else {
                                merge_r_b(b, bro);
                                fileIndex.seekg(now_index);
                                fileIndex.write(reinterpret_cast<char *>(&b), sizeof(Block));
                                recycle_block(bro_index);
                                return true;
                            }
                        } else {//合并左邻居
                            ll bro_index = b.fa.first->son[b.fa.second-1];
                            Block bro_l;
                            fileIndex.seekg(bro_index);
                            fileIndex.read(reinterpret_cast<char *>(&bro_l), sizeof(Block));
                            if (bro_l.num > BLOCK_MIN) {
                                get_one_child_l_b(b, bro_l);
                                fileIndex.seekg(now_index);
                                fileIndex.write(reinterpret_cast<char *>(&b), sizeof(Block));
                                fileIndex.seekg(bro_index);
                                fileIndex.write(reinterpret_cast<char *>(&bro_l), sizeof(Block));
                                return false;
                            } else {
                                merge_l_b(b, bro_l);
                                fileIndex.seekg(bro_index);
                                fileIndex.write(reinterpret_cast<char *>(&bro_l), sizeof(Block));
                                recycle_block(bro_index);
                                return true;
                            }
                        }
                    } else return false;
                }
            }
        }

        void get_one_child_r(Leave &le, Leave &rle) {
            le.num++;
            rle.num--;
            le.array[le.num] = rle.array[1];
            for (int i = 1; i <= rle.num; ++i) {
                rle.array[i] = rle.array[i + 1];
            }
            le.fa.first->key[le.fa.second+1]=rle.array[1].key;
        }

        void get_one_child_l(Leave &le, Leave &lle) {
            le.num++;
            lle.num--;
            for(int i = le.num; i > 1; --i)le.array[i]=le.array[i-1];
            le.array[1]=lle.array[lle.num+1];
            le.fa.first->key[le.fa.second]=le.array[1].key;
        }

        void merge_r(Leave &le, Leave &rle) {
            for (int i = le.num + 1; i <= le.num + rle.num; ++i) {
                le.array[i] = rle.array[i - le.num];
            }
            le.num += rle.num;
            --le.fa.first->num;
            for (int j = le.fa.second+1; j <= le.fa.first->num; ++j) {
                le.fa.first->key[j] = le.fa.first->key[j + 1];
                le.fa.first->son[j]=le.fa.first->son[j+1];
            }
        }

        void merge_l(Leave &le, Leave &lle) {
            for (int i = lle.num + 1; i <= le.num + lle.num; ++i) {
                lle.array[i] = le.array[i - lle.num];
            }
            lle.num += le.num;
            --le.fa.first->num;
            for (int j = le.fa.second; j <= le.fa.first->num; ++j) {
                le.fa.first->key[j] = le.fa.first->key[j + 1];
                le.fa.first->son[j] = le.fa.first->son[j + 1];
            }
        }

        bool leremove(Leave &l, const Node &key, ll now_index) {//true:fa内容被修改，false：fa内容未被修改
            int i;
            for(i = 0; i < l.num; ++i)if(com(key.key,l.array[i+1].key))break;
            for (int j = i; j < l.num; ++j)l.array[j] = l.array[j + 1];
            if(i==1)modify_father(l,l.array[1].key);
            l.num--;
            if (l.num < LEAVE_MIN) {
                if (!l.fa.second) {//是左边第一个节点
                    ll bro_index = l.fa.first->son[1];
                    Leave bro_r;
                    fileIndex.seekg(bro_index);
                    fileIndex.read(reinterpret_cast<char *>(&bro_r), sizeof(Leave));
                    if (bro_r.num > LEAVE_MIN) {
                        get_one_child_r(l, bro_r);
                        fileIndex.seekg(bro_index);
                        fileIndex.write(reinterpret_cast<char *>(&bro_r), sizeof(Leave));
                        fileIndex.seekg(now_index);
                        fileIndex.write(reinterpret_cast<char *>(&l), sizeof(Leave));
                        return false;//false意思是key未减少
                    } else if(bro_r.num) {
                        merge_r(l, bro_r);
                        fileIndex.seekg(now_index);
                        fileIndex.write(reinterpret_cast<char *>(&l), sizeof(Leave));
                        recycle_leave(bro_index);
                        return true;
                    }
                } else {//对左邻居操作
                    //可能左叶子是空的
                    ll bro_index = l.fa.first->son[l.fa.second - 1];
                    Leave bro_l;
                    fileIndex.seekg(bro_index);
                    fileIndex.read(reinterpret_cast<char *>(&bro_l), sizeof(Leave));
                    if (bro_l.num > LEAVE_MIN) {
                        get_one_child_l(l, bro_l);
                        fileIndex.seekg(bro_index);
                        fileIndex.write(reinterpret_cast<char *>(&bro_l), sizeof(Leave));
                        fileIndex.seekg(now_index);
                        fileIndex.write(reinterpret_cast<char *>(&l), sizeof(Leave));
                        return false;//
                    } else if(bro_l.num) {
                        merge_l(l, bro_l);
                        fileIndex.seekg(bro_index);
                        fileIndex.write(reinterpret_cast<char *>(&bro_l), sizeof(Leave));
                        recycle_leave(now_index);
                        return true;
                    }
                    else{//如果左叶子结点是空的,则与右邻居合并，块
                        if(l.fa.first->num==1){//不存在右邻居的情况，上方是根节点
                            fileIndex.seekg(now_index);
                            fileIndex.write(reinterpret_cast<char *>(&l), sizeof(Leave));
                            if(!l.num)return true;
                            return false;
                        }
//根节点特判
                        Leave bro_r;
                        bro_index = l.fa.first->son[l.fa.second+1];
                        fileIndex.seekg(bro_index);
                        fileIndex.read(reinterpret_cast<char *>(&bro_r), sizeof(Leave));
                        if(bro_r.num>LEAVE_MIN){//借
                            get_one_child_r(l,bro_r);
                            fileIndex.seekg(bro_index);
                            fileIndex.write(reinterpret_cast<char *>(&bro_r), sizeof(Leave));
                            fileIndex.seekg(now_index);
                            fileIndex.write(reinterpret_cast<char *>(&l), sizeof(Leave));
                            return false;
                        }
                        else{
                            merge_r(l,bro_r);
                            fileIndex.seekg(now_index);
                            fileIndex.write(reinterpret_cast<char *>(&l), sizeof(Leave));
                            recycle_leave(bro_index);
                            return true;

                        }
                    }

                }
            } else {
                fileIndex.seekg(now_index);
                fileIndex.write(reinterpret_cast<char *>(&l), sizeof(Leave));
                return false;
            }

        }

        bool modify(const Block &root,const K &key,const S &value){
            Block b=root;
            if(!b.num)return false;
            while(!b.isbottom){
                int index;
                for (index = 0; index < b.num; ++index)if (com(key, b.key[index + 1]))break;//下一个大于自己就取当前所在位置，退出循环
                ll index_son = b.son[index];
                fileIndex.seekg(index_son);
                fileIndex.read(reinterpret_cast<char *>(&b), sizeof(Block));
            }
            int index;
            for (index = 0; index < b.num; ++index)if(com(key, b.key[index + 1])){
                    break;//下一个大于自己就取当前所在位置，退出循环
            }
            ll index_son = b.son[index];
            Leave le;
            fileIndex.seekg(index_son);
            fileIndex.read(reinterpret_cast<char *>(&le), sizeof(Leave));
            for (index = 0; index < le.num; ++index){
                if (com(key, le.array[index + 1].key)){
                    break;//下一个大于自己就取当前所在位置，退出循环
                }
            }
            if(!index)return false;
            if(!com(le.array[index].key,key)&&!com(key,le.array[index].key))le.array[index].value=value;
            else return false;
            fileIndex.seekg(index_son);
            fileIndex.write(reinterpret_cast<char *>(&le), sizeof(Leave));
            return true;
        }
    public:

        BPT(const std::string &file) : file_name(file) {
            fileIndex.open(file_name, ios::out | ios::in | ios::binary);
            if (!fileIndex.good()) {
                fileIndex.open(file_name, ios::out);
                fileIndex.close();
                fileIndex.open(file_name, ios::out | ios::in | ios::binary);
                basicManager.index_root=sizeof(basicManager);
                fileIndex.seekg(0);
                fileIndex.write(reinterpret_cast<char *>(&basicManager), sizeof(basicManager));
                fileIndex.seekg(basicManager.index_root);
                fileIndex.write(reinterpret_cast<char *>(&root), sizeof(Block));
            } else {
                fileIndex.seekg(0);
                fileIndex.read(reinterpret_cast<char *>(&basicManager), sizeof(basicManager));
                fileIndex.seekg(basicManager.index_root);
                fileIndex.read(reinterpret_cast<char *>(&root), sizeof(Block));
            }
        };

        ~BPT() {
            fileIndex.seekg(0);
            fileIndex.write(reinterpret_cast<char *>(&basicManager), sizeof(basicManager));
            fileIndex.seekg(basicManager.index_root);
            fileIndex.write(reinterpret_cast<char *>(&root), sizeof(Block));
            fileIndex.close();
        };
        void remove(const Node &key_) {//确定存在的情况下
            int i;
            for(i = 0; i<root.num; ++i){
                if(com(key_.key,root.key[i+1]))break;
            }
            if(root.isbottom){
                Leave son;
                ll index_son = root.son[i];
                fileIndex.seekg(index_son);
                fileIndex.read(reinterpret_cast<char *>(&son), sizeof(Leave));
                son.fa.first=&root;
                son.fa.second=i;
                if(leremove(son,key_,index_son)){
                    if(!root.num){//合并只剩下一个儿子
                        Leave l,r;
                        fileIndex.seekg(root.son[0]);
                        fileIndex.read(reinterpret_cast<char *>(&l), sizeof(Leave));
                        if(l.num){//左儿子取空叶子结点
                            ll index;
                            ++root.num;
                            root.son[1]=root.son[0];
                            root.son[0]=index=add_one_leave();
                            root.key[1]=l.array[1].key;
                            fileIndex.seekg(index);
                            fileIndex.write(reinterpret_cast<char *>(&r), sizeof(Leave));
                        }
                    }
                    else{
                        Leave r;
                        fileIndex.seekg(root.son[1]);
                        fileIndex.read(reinterpret_cast<char *>(&r), sizeof(Leave));
                        if(!r.num){
                            recycle_leave(root.son[0]);
                            recycle_leave(root.son[1]);
                            root.num=0;
                        }
                    }
                }
            }
            else{
                Block son;
                ll index_son = root.son[i];
                fileIndex.seekg(index_son);
                fileIndex.read(reinterpret_cast<char *>(&son), sizeof(Block));
                son.fa.first=&root;
                son.fa.second=i;
                if (biremove(son, key_, index_son)) {
                    if (!root.num) {
                        recycle_block(basicManager.index_root);
                        basicManager.index_root = root.son[0];
                        fileIndex.seekg(2*sizeof(int));
                        fileIndex.write(reinterpret_cast<char *>(&basicManager.index_root), sizeof(ll));
                        fileIndex.seekg(basicManager.index_root);
                        fileIndex.read(reinterpret_cast<char *>(&root), sizeof(Block));//换根
                    }

                }
            }
            fileIndex.seekg(basicManager.index_root);
            fileIndex.write(reinterpret_cast<char *>(&root), sizeof(Block));
        }
        void Get(const K &left,const K &right, vector<S> &v) {
            v.clear();
            find_list_b(root, left,right, v);
        }

        bool Getone(const K &key,S &value){
            Node n(key,value);
            bool flag = find_one_block(n,root);
            if(!flag)return false;
            value = n.value;
            return true;
        }

        void Insert(const K &key, const S &value) {
            insert(key, value);
        }

        bool Remove(const K &key,S value) {
            Node n(key,value);
            if(!find_one(n))return false;
            remove(n);
            return true;
        }

        void reset() {
//          fileIndex.close();
//          fileIndex.open(file_name,ios::in|ios::out|ios::binary);
        }

        bool Modify(const K &key,const S &value){
        return modify(root,key,value);
        }
    };
}
#endif //TRAINTICKET2022_BPLUSTREE_HPP