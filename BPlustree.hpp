#ifndef TRAINTICKET2022_BPLUSTREE_HPP
#define TRAINTICKET2022_BPLUSTREE_HPP

#include<fstream>
#include<vector>

#define BLOCK_SIZE 6
#define LEAVE_SIZE 6
#define LEAVE_SPLIT_LEFT 3
#define LEAVE_MIN 2
using ll = long long;
using namespace ::std;
namespace jl {
    template<class K, class S, class Compare = std::less<K>>
    class BPT {
        friend class Leave;

    private:
        //内嵌类

        struct KVleave {
            ll file_index = 0;
            ll num = 0;
        };
        struct KVblock {
            ll file_index = 0;
            ll num = 0;
        };

        class Node {//叶子节点内-块链-点
        private://维护第二关键字
            long long value;
            K key;
        public:

            Node();

            Node(const ll &value_, const K &key_) : value(value_), key(key_) {
            };
        };

        class Block {//树上节点
        private:
            bool isbottom = false;
            ll son[BLOCK_SIZE];
            Node key[BLOCK_SIZE];
            int num = 0;
            std::pair<Block *, ll> fa;//存储父节点，内存地址
        public:
            Block();
        };

        class Leave {//叶子结点-块链-块
            friend class BPT;

        private:
            int nxt = -1;
            int pre = -1;
            int num = 0;
            std::pair<Block *, ll> fa;
            //read head and tail from array
            Node array[LEAVE_SIZE + 2];
        public:
            void MergeBlock(const int &offset1, const int &offset2);


            Leave() {};
        };

//private:

        const std::string file_name;

        std::fstream fileIndex;

        std::fstream reuse_block;

        std::fstream reuse_leave;

        Block root;

        Compare com;

        int totalblock;

        int totalleave;

        ll index_root;

        const int L;

        const int M;

        void BorrowLeft(Block &l, Block &pa, int index);

        void BorrowRight(Block &l, Block &pa, int index);

        void MergeLeft(Block &l, Block &pa, int index);

        void MergeRight(Block &l, Block &pa, int index);

        std::vector<S> find(const K &key_, int index);

        bool compare(const Node &n1, const Node &n2) {
            if (com(n1.key, n2.key))return true;
            if (com(n2.key, n1.key))return false;
            return n1.value < n2.value;

        }

        void recycle_leave(ll index) {}

        void recycle_block(ll index) {}

        ll add_one_leave() {//提供内存地址
            fileIndex.seekg(0);
            fileIndex.read(reinterpret_cast<char *>(&totalblock), sizeof(int));
            fileIndex.seekg(sizeof(int));
            fileIndex.read(reinterpret_cast<char *>(&totalleave), sizeof(int));
            ll index = totalblock * sizeof(Block) + totalleave * sizeof(Leave) + 2 * sizeof(int);
            ++totalleave;
            fileIndex.seekg(sizeof(int));
            fileIndex.write(reinterpret_cast<char *>(&totalleave), sizeof(int));
            return index;
        }

        ll add_one_block() {
            fileIndex.seekg(0);
            fileIndex.read(reinterpret_cast<char *>(&totalblock), sizeof(int));
            fileIndex.seekg(sizeof(int));
            fileIndex.read(reinterpret_cast<char *>(&totalleave), sizeof(int));
            ll index = totalblock * sizeof(Block) + totalleave * sizeof(Leave) + 2 * sizeof(int);
            ++totalblock;
            fileIndex.seekg(0);
            fileIndex.write(reinterpret_cast<char *>(&totalblock), sizeof(int));
            return index;
        }


        bool findone(const K &key, ll &value) {//true 找到了， false没有找到
            return bifind(root, key, value);
        }

        void erase(const K &key, ll &index) {

        }

        void insert(const K &key, ll &value) {
            KVblock pair_;
            fileIndex.seekg(2 * sizeof(int));
            fileIndex.read(reinterpret_cast<char *>(&index_root), sizeof(ll));
            if (biinsert(root, key, value, index_root, pair_)) {//根节点的特判
                Block newroot;
                ll new_root_index = add_one_block();
                newroot.son[1] = index_root;
                newroot.son[2] = pair_.file_index;
                newroot.son[1] = pair_.num;
                index_root = new_root_index;
                root = newroot;
                fileIndex.seekg(new_root_index);
                fileIndex.write(reinterpret_cast<char *>(&newroot), sizeof(Leave));
                fileIndex.seekg(2 * sizeof(int));
                fileIndex.write(reinterpret_cast<char *>(&index_root), sizeof(ll));
            }
        }

        ll bisearch_block(const Block &b, const K &key) {//升序排列，最后一个<=的key值
            int i;
            for (i = 0; i <= b.num - 1; ++i) {
                if (compare(b.key[i + 1], key))continue;
            }
            return i;
        }

        ll bisearch_leave(const Leave &le, const K &key) {
            int i;
            for (i = 1; i <= le.num; ++i) {
                if (compare(le.array[i], key))continue;
            }
            return i;
        }

        void SpilitBlock(ll index, KVblock &pair) {
            Block b, newb;
            fileIndex.seekg(index);//移动指针到指定位置
            fileIndex.read(reinterpret_cast<char *>(&b), sizeof(Block));
            for (int i = LEAVE_SPLIT_LEFT + 1; i <= b.num; ++i) {//复制过程
                newb.key[i - LEAVE_SPLIT_LEFT] = newb.key[i];
            }
            for (int i = LEAVE_SPLIT_LEFT + 1; i <= b.num + 1; ++i) {//复制过程
                newb.son[i - LEAVE_SPLIT_LEFT] = newb.son[i];
            }
            pair.num = b.key[LEAVE_SPLIT_LEFT];
            //修改元素数量
            newb.num = b.num - LEAVE_SPLIT_LEFT;
            b.num = LEAVE_SPLIT_LEFT - 1;
            newb.isbottom = b.isbottom;
            ll new_index = add_one_block();
            fileIndex.seekg(new_index);
            fileIndex.write(reinterpret_cast<char *>(&newb), sizeof(Block));
            fileIndex.seekg(index);
            fileIndex.write(reinterpret_cast<char *>(&b), sizeof(Block));
            pair.file_index = new_index;
        }

        bool biinsert(Block &b, const K &key, const ll &value, const ll &now_index, KVblock &pair_) {//返回值表示是否裂块
            ll index = bisearch_block(b, key);//返回对应key数组序号，son = index+1
            ll son_index = b.son[index + 1];
//                if(!compare(b.key[index+1],key)&&!compare(key,b.key[index]))return false;//留到叶节点当中判断
            if (!b.bottom) {//非叶子结点的父节点
                Block bl;//todo
                fileIndex.seekg(son_index);
                fileIndex.read(reinterpret_cast<char *>(&bl), sizeof(Block));
                if (biinsert(bl, key, value, son_index, pair_)) {
                    ++b.num;
                    for (int i = b.num; i >= index + 2; --i) {
                        b.num[i] = b.num[i - 1];
                    }
                    for (int i = b.num + 1; i >= index + 3; --i) {
                        b.son[i] = b.son[i - 1];
                    }
                    b.num[index + 1] = pair_.num;
                    b.son[index + 2] = pair_.file_index;
                    if (b.num >= BLOCK_SIZE) {//分裂block节点
                        fileIndex.seekg(now_index);
                        fileIndex.write(reinterpret_cast<char *>(&b), sizeof(Block));
                        SpilitBlock(now_index, pair_);
                        return true;
                    } else {
                        return false;
//                            fileIndex.seekg(b.son[index+1]);
//                            fileIndex.write(reinterpret_cast<char *>(&bl), sizeof(Block));
                    }
                } else {
                    fileIndex.seekg(son_index);
                    fileIndex.write(reinterpret_cast<char *>(&bl), sizeof(Block));
                    return false;
                }

            } else {//是叶子节点的父节点
                Leave le;
                KVleave pair;
                fileIndex.seekg(son_index);
                fileIndex.read(reinterpret_cast<char *>(&le), sizeof(Leave));//注意！这里将叶子节点读出来了，在leinsert函数当中写回文件
                if (leinsert(le, key, value, pair, b.son[index + 1])) {//叶子裂块，儿子增加
                    ++b.num;
                    for (int i = b.num; i >= index + 2; --i) {
                        b.num[i] = b.num[i - 1];
                    }
                    for (int i = b.num + 1; i >= index + 3; --i) {
                        b.son[i] = b.son[i - 1];
                    }
                    b.num[index + 1] = pair.num;
                    b.son[index + 2] = pair.file_index;
                    if (b.num >= BLOCK_SIZE) {
                        fileIndex.seekg(now_index);
                        fileIndex.write(reinterpret_cast<char *>(&b), sizeof(Block));
                        SpilitBlock(now_index, pair_);
                        return true;
                    } else return false;
                } else {//叶子未裂块，儿子不增加，没有裂块可能性
                    fileIndex.seekg(son_index);
                    fileIndex.write(reinterpret_cast<char *>(&le), sizeof(Leave));
                    return false;
                }
//                    fileIndex.seekg(b.son[index+1]);
//                    fileIndex.write(reinterpret_cast<char *>(&le), sizeof(Block));
            }

        }

        void SplitLeave(ll index, KVleave &pair) { //return new block's fileindex
            Leave le, newle, block3;
            fileIndex.seekg(index);//移动指针到指定位置
            fileIndex.read(reinterpret_cast<char *>(&le), sizeof(Leave));
            for (int i = LEAVE_SPLIT_LEFT + 1; i <= le.num; ++i) {//复制过程
                newle.array[i - LEAVE_SPLIT_LEFT] = le.array[i];
            }
            pair.num = newle.array[1];
            //修改元素数量
            newle.num = le.num - LEAVE_SPLIT_LEFT;
            le.num = LEAVE_SPLIT_LEFT;
            //本身是末位节点
            if (le.nxt == -1) {
                newle.pre = index;
                newle.nxt = le.nxt;
                le.nxt = add_one_leave();
                //写入两个block
                fileIndex.seekg(le.nxt);
                fileIndex.write(reinterpret_cast<char *>(&newle), sizeof(Leave));
                fileIndex.seekg(index);
                fileIndex.write(reinterpret_cast<char *>(&le), sizeof(Leave));
                pair.file_index = le.nxt;
            }
                //不是末位节点
            else {
                fileIndex.seekg(le.nxt);
                fileIndex.read(reinterpret_cast<char *>(&block3), sizeof(Leave));
                newle.nxt = le.nxt;
                newle.pre = index;
                block3.pre = add_one_leave();
                le.nxt = block3.pre;
                fileIndex.seekg(le.nxt);
                fileIndex.write(reinterpret_cast<char *>(&newle), sizeof(Leave));
                fileIndex.seekg(index);
                fileIndex.write(reinterpret_cast<char *>(&le), sizeof(Leave));
                fileIndex.seekg(newle.nxt);
                fileIndex.write(reinterpret_cast<char *>(&block3), sizeof(Leave));
                pair.file_index = le.nxt;
            }
        };

        bool leinsert(Leave &le, const K &key, const ll &value,
                      KVleave &pair, const ll &now_index) {//true = spilited
            ll index = bisearch_leave(le, key);//找到对应的kv对位置
            if (!compare(le.array[index].key, key) && !compare(key, le.array[index].key))return false;//未裂块
            //----插入新的kv对
            ++le.num;
            for (int i = le.num; i >= index; --i) {
                le.array[i + 1] = le.array[i];
            }
            Node n(value, key);
            le.array[index] = n;
            //-----插入结束-----是否裂块
            if (le.num >= BLOCK_SIZE) {
                fileIndex.seekg(now_index);
                fileIndex.write(reinterpret_cast<char *>(&le), sizeof(Leave));
                SplitLeave(now_index, pair);
                return true;//裂块
            } else {
//                fileIndex.seekg(now_index);
//                fileIndex.write(reinterpret_cast<char *>(&le), sizeof(Leave));//重新写回文件
                return false;
            }
        }

        bool lfind(const Leave &le, const K &key, ll &value) {
            ll l = 1;
            ll r = le.num;//数组key的大小
            ll mid = (l + r) >> 1;
            while (l < r) {
                if (le.array[mid].key < key)l = mid + 1;
                else r = mid;
                mid = (l + r) >> 1;
            }
            if (le.array[l].key == key) {
                value = le.array[l].index;
                return true;
            }
            return false;
        }

        bool bifind(Block *&b, const K &key, ll &value) {
            int i;
            for (i = 0; i <= b->num - 1; ++i) {
                if (compare(b->key[i + 1], key))continue;
            }
            if (b->isbottom) {
                ll index = b->son[i];
                fileIndex.seekg(index);
                Leave lea;
                fileIndex.read(reinterpret_cast<char *>(&lea), sizeof(Leave));
                lea.fa.first = b;
                lea.fa.second = i;
                return lfind(lea, key, value);
            }

            ll index = b->son[i];
            fileIndex.seekg(index);
            Block bl;
            fileIndex.read(reinterpret_cast<char *>(&bl), sizeof(Block));
            bl.fa.first = b;
            bl.fa.second = i;
            b = &bl;
            return bifind(b, key, value);

        }

        void get_one_child_r_b(Block &b, Block &r) {//if b is the first child
            //key fa r
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
            for (int i = b.num; i > 1; --i) {
                b.key[i] = b.key[i - 1];
            }
            for (int i = b.num; i > 0; --i) {
                b.son[i] = b.son[i - 1];
            }
            b.num++;
            b.son[0] = l.son[l.num];
            b.key[1] = b.fa.first->key[b.fa.second];
            b.fa.first->key[b.fa.second] = l.key[l.num];
            l.num--;
        }

        void merge_r_b(Block &b, Block &r) {
            --totalblock;
            for (int i = b.num + 1; i <= b.num + r.num; ++i) {
                b.key[i] = r.key[i - b.num];
            }
            for (int i = b.num + 1; i <= b.num + r.num + 1; ++i) {
                b.son[i] = r.son[i - b.num - 1];
            }
            b.num += r.num;
            b.fa.first->num--;
            for (int i = b.fa.second + 1; i <= b.fa.first->num; i++) {
                b.fa.first->key[i] = b.fa.first->key[i + 1];
            }
        }

        void merge_l_b(Block &b, Block &l) {
            --totalblock;
            for (int i = l.num + 1; i <= b.num + l.num; ++i) {
                l.key[i] = b.key[i - l.num];
            }
            for (int i = l.num + 1; i <= b.num + l.num + 1; ++i) {
                l.son[i] = b.son[i - l.num - 1];
            }
            l.num += b.num;
        }

        bool biremove(const Block &b, const K &key, K &array_key, ll now_index) {//return false,fa没有被改变；true：fa被改变了
            ll index_key = bisearch_block(b, key);
//            if(index_key!=0&&!compare(b.key[index_key],key)&&!compare(key,b.key[index_key]))array_key=&b.key[index_key];
            ll index_son = b.son[index_key];
            if (!b.isbottom) {
                Block bson;
                fileIndex.seekg(index_son);
                fileIndex.read(reinterpret_cast<char *>(&bson), sizeof(Block));
                bson.fa.first = &b;
                bson.fa.second = index_key;
                if (!biremove(bson, key)) {
                    return false;
                } else {
                    fileIndex.seekg(now_index);
                    fileIndex.write(reinterpret_cast<char *>(&b), sizeof(Block));
                    if (b.num < LEAVE_MIN) {
                        if (!b.fa.second) {//合并右邻居
                            ll bro_index = b.fa.first->son[1];
                            Block bro;
                            fileIndex.seekg(bro_index);
                            fileIndex.read(reinterpret_cast<char *>(&bro), sizeof(Block));
                            if (bro.num > LEAVE_MIN) {
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
                            ll bro_index = b.fa.first[b.fa.second];
                            Block bro_l;
                            fileIndex.seekg(bro_index);
                            fileIndex.read(reinterpret_cast<char *>(&bro_l), sizeof(Block));
                            if (bro_l.num > LEAVE_MIN) {
                                get_one_child_l_b(b, bro_l);
                                fileIndex.seekg(now_index);
                                fileIndex.write(reinterpret_cast<char *>(&b), sizeof(Block));
                                fileIndex.seekg(bro_index);
                                fileIndex.write(reinterpret_cast<char *>(&bro_l), sizeof(Block));
                                return true;
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
            } else {
                Leave lson;
                fileIndex.seekg(index_son);
                fileIndex.read(reinterpret_cast<char *>(&lson), sizeof(Leave));
                lson.fa.first = &b;
                lson.fa.second = index_key;
                if (!leremove(lson, key)) {
                    return false;
                } else {//调整中间节点
                    fileIndex.seekg(now_index);
                    fileIndex.write(reinterpret_cast<char *>(&b), sizeof(Block));
                    if (b.num < LEAVE_MIN) {
                        if (!b.fa.second) {//合并右邻居
                            ll bro_index = b.fa.first->son[1];
                            Block bro;
                            fileIndex.seekg(bro_index);
                            fileIndex.read(reinterpret_cast<char *>(&bro), sizeof(Block));
                            if (bro.num > LEAVE_MIN) {
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
                            ll bro_index = b.fa.first[b.fa.second];
                            Block bro_l;
                            fileIndex.seekg(bro_index);
                            fileIndex.read(reinterpret_cast<char *>(&bro_l), sizeof(Block));
                            if (bro_l.num > LEAVE_MIN) {
                                get_one_child_l_b(b, bro_l);
                                fileIndex.seekg(now_index);
                                fileIndex.write(reinterpret_cast<char *>(&b), sizeof(Block));
                                fileIndex.seekg(bro_index);
                                fileIndex.write(reinterpret_cast<char *>(&bro_l), sizeof(Block));
                                return true;
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
        }

        void get_one_child_l(Leave &le, Leave &lle) {
            le.num++;
            lle.num--;
            le.array[le.num] = lle.array[1];
            for (int i = 1; i <= lle.num; ++i) {
                lle.array[i] = lle.array[i + 1];
            }
        }

        void merge_r(Leave &le, Leave &rle) {
            --totalleave;
            for (int i = le.num + 1; i <= le.num + rle.num; ++i) {
                le.array[i] = rle.array[i - le.num];
            }
            le.num += rle.num;
        }

        void merge_l(Leave &le, Leave &lle) {
            --totalleave;
            for (int i = le.num + 1; i <= le.num + lle.num; ++i) {
                le.array[i] = lle.array[i - le.num];
            }
            le.num += lle.num;
        }

        bool leremove(Leave &l, const K &key, ll now_index, int key_index) {//true:fa内容被修改，false：fa内容未被修改
            //npw_index是文件下标
            //key_index是son[]的下标
            ll index = bisearch_leave(l, key);
            for (int i = index; i <= l.num; ++i) {
                l.array[i] = l.array[i + 1];
            }
            l.num--;
            if (l.num < LEAVE_MIN) {
                if (!l.fa.second) {//对右邻居操作
                    ll bro_index = l.fa.first->son[1];
                    Leave bro_r;
                    fileIndex.seekg(bro_index);
                    fileIndex.read(reinterpret_cast<char *>(&bro_r), sizeof(Leave));
                    if (bro_r.num > LEAVE_MIN) {
                        get_one_child_r(l, bro_r);
                        l.fa.first->key[1] = bro_r.array[1].key;
                        fileIndex.seekg(bro_index);
                        fileIndex.write(reinterpret_cast<char *>(&bro_r), sizeof(Leave));
                        fileIndex.seekg(now_index);
                        fileIndex.write(reinterpret_cast<char *>(&l), sizeof(Leave));
                        return true;//false意思是fa被修改，需要写回文件
                    } else {
                        merge_r(l, bro_r);
                        fileIndex.seekg(now_index);
                        fileIndex.write(reinterpret_cast<char *>(&l), sizeof(Leave));
                        recycle_leave(bro_index);
                        --l.fa.first->num;
                        for (int i = 1; i <= l.fa.first->num; ++i) {
                            l.fa.first->key[i] = l.fa.first->key[i + 1];
                        }
                        return true;
                    }
                } else {//对左邻居操作
                    ll bro_index = l.fa.first->son[key_index - 1];
                    Leave bro_l;
                    fileIndex.seekg(bro_index);
                    fileIndex.read(reinterpret_cast<char *>(&bro_l), sizeof(Leave));
                    if (bro_l.num > LEAVE_MIN) {
                        get_one_child_l(l, bro_l);
                        l.fa.first->key[1] = bro_l.array[1].key;
                        fileIndex.seekg(bro_index);
                        fileIndex.write(reinterpret_cast<char *>(&bro_l), sizeof(Leave));
                        fileIndex.seekg(now_index);
                        fileIndex.write(reinterpret_cast<char *>(&l), sizeof(Leave));
                        return true;//false意思是fa被修改，需要写回文件
                    } else {
                        merge_l(l, bro_l);
                        fileIndex.seekg(now_index);
                        fileIndex.write(reinterpret_cast<char *>(&l), sizeof(Leave));
                        recycle_leave(bro_index);
                        --l.fa.first->num;
                        for (int i = 1; i <= l.fa.first->num; ++i) {
                            l.fa.first->key[i] = l.fa.first->key[i + 1];
                        }
                        return true;
                    }

                }
            } else {
                fileIndex.seekg(now_index);
                fileIndex.write(reinterpret_cast<char *>(&l), sizeof(Leave));
                return false;
            }

        }
        void remove(const K &key) {//确定存在的情况下
            if (biremove(root, key, index_root)) {
                if (root.num == 1&&!root.isbottom) {
                    recycle_block(index_root);
                    index_root=root.son[0];
                    fileIndex.seekg(index_root);
                    fileIndex.read(reinterpret_cast<char *>(&root), sizeof(Block));
                }
                else {
                    fileIndex.seekg(index_root);
                    fileIndex.write(reinterpret_cast<char *>(&root), sizeof(Block));
                }
            }
        }
        void find_list_b(const Block &b,const K &k,std::vector<ll> &v){

        }
        void find_list_l(const Leave &le,const K &k,std::vector<ll> &v){

        }

    public:

        BPT(const std::string &file) : file_name(file) {
            fileIndex.open(file_name, ios::out | ios::in | ios::binary);
            if (!fileIndex.good()) {
                fileIndex.open(file_name, ios::out);
                fileIndex.close();
                fileIndex.open(file_name, ios::out | ios::in | ios::binary);
                fileIndex.seekg(0);
                totalblock = 1;
                fileIndex.write(reinterpret_cast<char *>(&totalblock), sizeof(int));
                fileIndex.seekg(sizeof(int));
                totalleave = 1;
                fileIndex.write(reinterpret_cast<char *>(&totalleave), sizeof(int));
                fileIndex.seekg(2 * sizeof(int));
                index_root = 2 * sizeof(int) + sizeof(long long);
                fileIndex.write(reinterpret_cast<char *>(&index_root), sizeof(ll));
                fileIndex.seekg(index_root);
                fileIndex.write(reinterpret_cast<char *>(&root), sizeof(Block));
            } else {
                fileIndex.seekg(2 * sizeof(int));
                fileIndex.read(reinterpret_cast<char *>(&index_root), sizeof(ll));
                fileIndex.seekg(index_root);
                fileIndex.read(reinterpret_cast<char *>(&root), sizeof(Block));
            }
        };

        ~BPT() {
            fileIndex.seekg(sizeof(int));
            fileIndex.write(reinterpret_cast<char *>(&root), sizeof(Block));
            fileIndex.close();
        };

        bool Getone(const K &key, ll &value) {
            return find(key, value);//value存储查找结果
        }

        void Get(const K &key, std::vector<ll> &v) {
           v.clear();
        }

        void Set(const K &key, ll &value) {
            if (find(key, value)) {

            } else {
                insert(key, value);
            }
        }

        bool Remove(const K &key) {
            ll value;
            if (!find(key, value))return false;
            remove(key);
            return true;
        }
    };

}

#endif TRAINTICKET2022_BPLUSTREE_HPP