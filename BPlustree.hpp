#ifndef TRAINTICKET2022_BPLUSTREE_HPP
#define TRAINTICKET2022_BPLUSTREE_HPP

#include<fstream>
#include<vector>

#define BLOCK_SIZE 3
#define LEAVE_SIZE 3
#define LEAVE_SPLIT_LEFT 2
#define LEAVE_MIN 2
using ll = long long;
using namespace ::std;
namespace lailai {
    template<class K, class S, class Compare = std::less<K>>
    class BPT {
        friend class Leave;

    private:
        //内嵌类
        class Node {//叶子节点内-块链-点
        public://维护第二关键字
            long long value;
            K key;
        public:
           void operator=(const Node &x){
               value=x.value;
               key=x.key;
           }
            Node(){};

            Node( const K &key_,const ll &value_) : value(value_), key(key_) {
            };
        };
        class Block {//树上节点
            friend class BPT;

        private:
            bool isbottom = false;
            ll son[BLOCK_SIZE + 5];
            Node key[BLOCK_SIZE + 5];
            int num = 0;
            std::pair<Block *, ll> fa;//存储父节点，内存地址
        public:
            Block(){};
        };
        class Leave {//叶子结点-块链-块
            friend class BPT;

        private:
//            int nxt = -1;
//            int pre = -1;
            int num = 0;
            std::pair<Block *, ll> fa;
            //read head and tail from array
            Node array[LEAVE_SIZE + 5];
        public:
            void MergeBlock(const int &offset1, const int &offset2);


            Leave() {};
        };
        struct KVblock {
            ll file_index = 0;
            Node key;
        };
        struct KVleave {
            ll file_index = 0;
            Node key;
        };

        const std::string file_name;

        std::fstream fileIndex;

        std::fstream reuse_block;

        std::fstream reuse_leave;

        Block root;

        Compare com;

        int totalblock;

        int totalleave;

        ll index_root;

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
//            cout << totalblock << "%%" << endl;
            fileIndex.seekg(sizeof(int));
            fileIndex.read(reinterpret_cast<char *>(&totalleave), sizeof(int));
//            cout << totalleave << "%%" << endl;
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

        void modify_father(const Leave &l,const Node &key){
            Block *p=l.fa.first;
            if(l.fa.second){
                l.fa.first->key[l.fa.second]=key;
                return;
            }
            while(p!=&root&&!p->fa.second){
                p = p->fa.first;
            }
            if(p==&root)return;
            p->fa.first->key[p->fa.second]=key;
        }

        bool findone(const K &key, ll &value) {//true 找到了， false没有找到
            return bifind(root, key, value);
        }

        ll bisearch_block(const Block &b, const Node &key) {//升序排列，最后一个<=的key值
            int i;
            for (i = 0; i <= b.num - 1; ++i) {
                if (compare(b.key[i + 1], key) || !compare(b.key[i + 1], key) && !compare(key, b.key[i + 1]))continue;
                else break;
            }
            return i;
        }

        ll bisearch_leave(const Leave &le, const Node &key) {
            int i=0;
            for (i = 1; i <= le.num; ++i) {
                if (compare(le.array[i], key))continue;
                else break;
            }
            return i;
        }

        void find_list_l(const Leave &le, const K &key, std::vector<ll> &v) {
            for (int i = 1; i <= le.num; ++i) {
                if (!com(le.array[i].key, key) && !com(key, le.array[i].key))v.push_back(le.array[i].value);
            }
        }

        void find_list_b(const Block &b, const K &key, std::vector<ll> &v) {
            if (!b.isbottom) {
                cout << "&&" << endl;
                int i;
                for (i = 0; i < b.num; ++i) {
                    if (com(key,b.key[i + 1].key)|| !com(key,b.key[i+1].key)&&!com(b.key[i+1].key,key))break;//下一个位置关键字大于等于
                }
                while (i <= b.num){
                    ll index_son = b.son[i];
                    Block son;
                    fileIndex.seekg(index_son);
                    fileIndex.read(reinterpret_cast<char *>(&son), sizeof(Block));
                    find_list_b(son, key, v);
                    ++i;
                    if(i>b.num|| com(key,b.key[i].key))break;//新的位置关键字大于key
                }
            } else {
                cout << "$%$" << endl;
                int i;
//                cout << "debug" << b.num << endl;
                for (i = 0; i < b.num; ++i) {
                    if (com(key,b.key[i + 1].key)||!com(key,b.key[i+1].key)&&!com(b.key[i+1].key,key))break;
                }
//                cout << "debug" << i << endl;
//                ll index_son = b.son[i];
//                Leave son;
//                fileIndex.seekg(index_son);
//                fileIndex.read(reinterpret_cast<char *>(&son), sizeof(Leave));
//                find_list_l(son, key, v);
//                ++i;
                while (i <= b.num) {
                    ll index_son = b.son[i];
                    Leave son;
                    fileIndex.seekg(index_son);
                    fileIndex.read(reinterpret_cast<char *>(&son), sizeof(Leave));
                    find_list_l(son, key, v);
                    ++i;
                    if(i>b.num||com(key,b.key[i].key))break;
                }
            }

        }

        void insert(const K &key, const ll &value) {
            Node n(key, value);
            KVblock pair_;
            //debug
            cout << "----root----" << endl;
            for(int i = 1;i <= root.num; ++i){
                cout << root.key[i].key << ' ';
            }
            cout << endl;
            cout << "-----------" << endl;
            if (biinsert(root, n, index_root, pair_)) {//根节点的特判
//                ++root.num;
//                fileIndex.seekg(index_root);
//                fileIndex.write(reinterpret_cast<char *>(&root), sizeof(Block));
//                if(root.num>BLOCK_SIZE){

//                    SpilitBlock(index_root,pair_);
                Block newroot;
                newroot.num=1;
                ll new_root_index = add_one_block();
                newroot.son[0] = index_root;
                newroot.son[1] = pair_.file_index;
                newroot.key[1] = pair_.key;
                index_root = new_root_index;
                root = newroot;
                fileIndex.seekg(new_root_index);
                fileIndex.write(reinterpret_cast<char *>(&newroot), sizeof(Block));
                fileIndex.seekg(2 * sizeof(int));
                fileIndex.write(reinterpret_cast<char *>(&index_root), sizeof(ll));
//                }

            }
        }

        void SpilitBlock(ll index, KVblock &pair) {
            Block b, newb;
            fileIndex.seekg(index);//移动指针到指定位置
            fileIndex.read(reinterpret_cast<char *>(&b), sizeof(Block));
            for (int i = LEAVE_SPLIT_LEFT + 2; i <= b.num; ++i) {//复制过程
                newb.key[i - LEAVE_SPLIT_LEFT-1] = b.key[i];
            }
            for (int i = LEAVE_SPLIT_LEFT + 1; i <= b.num + 1; ++i) {//复制过程
                newb.son[i - LEAVE_SPLIT_LEFT-1] = b.son[i];
            }
            pair.key = b.key[LEAVE_SPLIT_LEFT+1];
            //修改元素数量
            newb.num = b.num - LEAVE_SPLIT_LEFT-1;
            b.num = LEAVE_SPLIT_LEFT;
            newb.isbottom = b.isbottom;
            ll new_index = add_one_block();
            fileIndex.seekg(new_index);
            fileIndex.write(reinterpret_cast<char *>(&newb), sizeof(Block));
            fileIndex.seekg(index);
            fileIndex.write(reinterpret_cast<char *>(&b), sizeof(Block));
            pair.file_index = new_index;
        }

        bool biinsert(Block &b, const Node &key, const ll &now_index, KVblock &pair_) {//返回值表示下层是否裂块，这层需要添加键值。
            //debug
            cout << "-----insert block-------" << endl;
            cout << b.num <<endl;
            for(int i = 1; i <= b.num;++i){
                cout << b.key[i].key<<' ';
            }
            cout << endl;
            cout << "----------" << endl;
            if(b.num==0){//key个数为0
                b.isbottom=true;
                ++b.num;
                Leave l,r;
                b.key[1]=key;
                b.son[0]=add_one_leave();
                b.son[1]=add_one_leave();
//                r.pre=b.son[0];
                ++r.num;
                r.array[r.num]=key;
                fileIndex.seekg(now_index);
                fileIndex.write(reinterpret_cast<char *>(&b), sizeof(Block));
                fileIndex.seekg(b.son[1]);
                fileIndex.write(reinterpret_cast<char *>(&r), sizeof(Leave));
                fileIndex.seekg(b.son[0]);
                fileIndex.write(reinterpret_cast<char *>(&l), sizeof(Leave));
                return false;
            }
            int index;
            for(index = 0; index < b.num; ++index){
                //debug
//                cout << "block compare"<<' ' << key.key << ' ' << b.key[index+1].key << ' ';
                if(compare(key,b.key[index+1]))break;//下一个大于自己就取当前所在位置，退出循环
            }
//            cout << endl;
            //返回对应key数组序号，son[index];
            ll son_index = b.son[index];
            cout << "son-index "<<index << ' '<<son_index << endl;
            if (!b.isbottom) {//非叶子结点的父节点
                Block bl;//todoxx
                fileIndex.seekg(son_index);
                fileIndex.read(reinterpret_cast<char *>(&bl), sizeof(Block));
//                cout << "bl.num " << bl.num << endl;
                cout << fileIndex.bad() << "qwq" << endl;
                bl.fa.first=&b;
                bl.fa.second=index;
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
                cout << son_index << "##\n";
                fileIndex.seekg(son_index);
                fileIndex.read(reinterpret_cast<char *>(&le), sizeof(Leave));
                le.fa.first=&b;

                le.fa.second=index;
//                cout << "read" << le.num << endl;
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
                    cout << "---spilit leave up block-------" << endl;
                    for(int i =1; i <= b.num; ++i){
                        cout << b.key[i].key<<' ';
                    }
                    cout << endl;
                    cout << "-------" << endl;

                    if (b.num > BLOCK_SIZE) {
                        fileIndex.seekp(now_index);
                        fileIndex.write(reinterpret_cast<char *>(&b), sizeof(Block));
                        SpilitBlock(now_index, pair_);
                        return true;
                    } else{
//                        cout << "now-index " << now_index << endl;
//                        cout << "b.num " << b.num << ' ' << now_index << endl;
                        reset();
                        fileIndex.seekp(now_index);
                        fileIndex.write(reinterpret_cast<char *>(&b), sizeof(Block));
                        Block bb;
//                        fileIndex.close();
//                        fileIndex.open(file_name, ios::out | ios::in | ios::binary);
                        fileIndex.seekg(now_index);
                        fileIndex.read(reinterpret_cast<char *>(&bb), sizeof(Block));
                        cout << "debug: " << now_index << endl;
                        cout << bb.num << ' ';
                        for(int i = 1; i <= bb.num; ++i){
                            cout << bb.key[i].key << ' ';
                        }
                        cout << endl;
//                        cout << fileIndex.bad() << "qwq" << endl;
                        return false;
                    }
                } else {//叶子未裂块，儿子不增加，没有裂块可能性,但是key可能修改，所以写回
                    fileIndex.seekg(now_index);
                    fileIndex.write(reinterpret_cast<char *>(&b), sizeof(Block));
                    return false;
                }
//                    fileIndex.seekg(b.son[index+1]);
//                    fileIndex.write(reinterpret_cast<char *>(&le), sizeof(Block));
            }

        }

        void splitLeave(ll index, KVleave &pair) { //return new block's fileindex
            Leave le, newle;
            fileIndex.seekg(index);//移动指针到指定位置
            fileIndex.read(reinterpret_cast<char *>(&le), sizeof(Leave));
            //debug
//            cout << "new-array----------" << endl;
            for (int i = LEAVE_SPLIT_LEFT + 1; i <= le.num; ++i) {//复制过程
                newle.array[i - LEAVE_SPLIT_LEFT] = le.array[i];
//                cout << le.array[i].key << ' ';
            }
//            cout << endl;
//            cout << "-------"<<endl;
            pair.key = newle.array[1];
            pair.file_index=add_one_leave();
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
            //debug
//            cout << key.key << " key"<< endl;
            cout << "-----insert-leave----"<<endl;
            for(i = 1; i <= le.num; ++i)cout << le.array[i].key<<' ';
            cout << endl;
            cout << "---------" << endl;
            for(i = 0; i < le.num; ++i){
                if(compare(key,le.array[i+1]))break;
            }
            if(i&&!compare(key,le.array[i])&&!compare(le.array[i],key))return false;
            else{
                ++le.num;
                for(int j = le.num; j>i+1;--j){
                    le.array[j]=le.array[j-1];
                }
                le.array[i+1]=key;
                if(!i){
                    modify_father(le,key);
                }
            }
            cout << "-----insert-leave-after----"<<endl;
            for(i = 1; i <= le.num; ++i)cout << le.array[i].key<<' ';
            cout << endl;
            cout << "---------" << endl;
            //-----插入结束-----是否裂块
            if (le.num > BLOCK_SIZE) {
                fileIndex.seekg(now_index);
                fileIndex.write(reinterpret_cast<char *>(&le), sizeof(Leave));
                splitLeave(now_index, pair);
                return true;//裂块
            } else {
                reset();
                cout << now_index<< "%%%";
                fileIndex.seekg(now_index);
                fileIndex.write(reinterpret_cast<char *>(&le), sizeof(Leave));//重新写回文件
                Leave t;
                fileIndex.seekg(now_index);
                fileIndex.read(reinterpret_cast<char *>(&t), sizeof(Leave));//重新写回文件
                cout << "de"<<endl;
                cout << t.num << endl;
                return false;
            }
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

        bool biremove(Block &b, const Node &key,ll now_index) {//return false,fa没有被改变；true：fa被改变了
            ll index_key = bisearch_block(b, key);
//            if(index_key!=0&&!compare(b.key[index_key],key)&&!compare(key,b.key[index_key]))array_key=&b.key[index_key];
            ll index_son = b.son[index_key];
            if (!b.isbottom) {
                Block bson;
                fileIndex.seekg(index_son);
                fileIndex.read(reinterpret_cast<char *>(&bson), sizeof(Block));
                bson.fa.first = &b;
                bson.fa.second = index_key;
                if (!biremove(bson, key,index_son)) {
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
                            ll bro_index = b.fa.first->son[b.fa.second];
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
                if (!leremove(lson, key,index_son)) {
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
                            ll bro_index = b.fa.first->son[b.fa.second];
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

        bool leremove(Leave &l, const Node &key, ll now_index) {//true:fa内容被修改，false：fa内容未被修改
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
                        l.fa.first->key[1] = bro_r.array[1];
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
                    ll bro_index = l.fa.first->son[l.fa.second - 1];
                    Leave bro_l;
                    fileIndex.seekg(bro_index);
                    fileIndex.read(reinterpret_cast<char *>(&bro_l), sizeof(Leave));
                    if (bro_l.num > LEAVE_MIN) {
                        get_one_child_l(l, bro_l);
                        l.fa.first->key[1] = bro_l.array[1];
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

        void remove(const K &key_) {//确定存在的情况下
            Node key(key_,1);
            if (biremove(root, key, index_root)) {
                if (root.num == 1 && !root.isbottom) {
                    recycle_block(index_root);
                    index_root = root.son[0];
                    fileIndex.seekg(index_root);
                    fileIndex.read(reinterpret_cast<char *>(&root), sizeof(Block));
                } else {
                    fileIndex.seekg(index_root);
                    fileIndex.write(reinterpret_cast<char *>(&root), sizeof(Block));
                }
            }
        }

        void Test(){

        }

    public:

        BPT(const std::string &file) : file_name(file) {
            fileIndex.open(file_name, ios::out | ios::in | ios::binary);
            if (!fileIndex.good()) {
//                cout << "here" << endl;
                fileIndex.open(file_name, ios::out);
                fileIndex.close();
                fileIndex.open(file_name, ios::out | ios::in | ios::binary);
                fileIndex.seekg(0);
                totalblock = 1;
                fileIndex.write(reinterpret_cast<char *>(&totalblock), sizeof(int));
                fileIndex.seekg(sizeof(int));
                totalleave = 0;
                fileIndex.write(reinterpret_cast<char *>(&totalleave), sizeof(int));
                fileIndex.seekg(2 * sizeof(int));
                index_root = 2 * sizeof(int)+sizeof(ll);
                fileIndex.write(reinterpret_cast<char *>(&index_root), sizeof(ll));
                fileIndex.seekg(index_root);
                fileIndex.write(reinterpret_cast<char *>(&root), sizeof(Block));
            } else {
//                cout << "Here" << endl;
                fileIndex.seekg(0);
                fileIndex.read(reinterpret_cast<char *>(&totalblock), sizeof(int));
                fileIndex.seekg(sizeof(int));
                fileIndex.read(reinterpret_cast<char *>(&totalleave), sizeof(int));
//                cout << "&&&" << totalleave << endl;
                fileIndex.seekg(2 * sizeof(int));
                fileIndex.read(reinterpret_cast<char *>(&index_root), sizeof(ll));
                fileIndex.seekg(index_root);
                fileIndex.read(reinterpret_cast<char *>(&root), sizeof(Block));
            }
        };

        ~BPT() {
            fileIndex.seekg(index_root);
            fileIndex.write(reinterpret_cast<char *>(&root), sizeof(Block));
            fileIndex.seekg(2 * sizeof(int));
            fileIndex.write(reinterpret_cast<char *>(&index_root), sizeof(ll));
            fileIndex.seekg(0);
            fileIndex.write(reinterpret_cast<char *>(&totalblock), sizeof(int));
            fileIndex.seekg(sizeof(int));
            fileIndex.write(reinterpret_cast<char *>(&totalleave), sizeof(int));
            fileIndex.close();
            fileIndex.open(file_name,ios::trunc);
            fileIndex.close();
        };

//        bool Getone(const K &key, ll &value) {
//            return find(key, value);//value存储查找结果
//        }

        void Get(const K &key, std::vector<ll> &v) {
            v.clear();

            find_list_b(root, key, v);
        }

        void Insert(const K &key, const ll &value) {
            insert(key, value);
        }

        void Set(const K &key, ll &value) {
            if (find(key, value)) {
                remove(key);
                insert(key, value);
            } else {
                insert(key, value);
            }
        }

        bool Remove(const K &key) {
            vector<ll> v;
            v.clear();
            find_list_b(root, key,v);
            if (v.empty())return false;
            remove(key);
            return true;
        }
        void reset(){
          //fileIndex.close();
          //fileIndex.open(file_name,ios::in|ios::out|ios::binary);
        }
    };

}
#endif //TRAINTICKET2022_BPLUSTREE_HPP