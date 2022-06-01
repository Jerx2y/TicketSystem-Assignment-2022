# BPT

### 提供接口

```c++
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
```

`//todo 一个file类，实现向数据文件读写功能，提供map功能`

# 施工了捏

main.cpp里面包含bpt.hpp 

**头文件如何设置？**

file的作用是写入主数据文件

bpt自己负责自己的数据写入

## 一、构造函数/析构函数

**构造函数：**

创建文件，打开文件流指针，创建根节点，写入文件并保留在内存当中，读入root节点（or新建并写入）

*这里的totalblock数量是否有用？*

*看不懂.good()含义了qwq* 

*如果是第一次以in/out方式打开，且需要创造一个新文件的话，是否会打开失败然后进入第一个if语句，写入totalblock=0？*

**析构函数：**

写回root节点，关闭文件流指针。

写回`totalblock;totalleave;recycledist`

以上考虑封装

## 二、find私有函数

*注意son中存储的index不用加上sizeof(int)*

**实现内存回收的方式不需要再记录文件末尾位置?**

*//todo比较大小用结构体compare来实现*

## 三、内存回收实现

维护一个栈储存所有删掉的节点index，分为block与leaf

如果链表为空，则写入totalblock*sizeof(block)+totalleaf*sizeof(leaf)里，total block+1；

函数提供新元素写入地址

`ll add_one_leave(){}`

`ll add_one_block(){}`

`//todo  一个固定大小的链表，存储删除节点的下标，封装成提供地址的类 `

## 四、insert私有函数

考虑插入冲突的情况。

* 设计错误类：插入一个树中原来就有的元素，插入重复的时候希望throw还是忽略这个问题？ans：什么都没有发生

**insert_block(非叶子节点插入)**

```c++
//底层调用insert_leave
//非底层调用insert_block
下一层不裂块-end
下一层裂-add child
孩子数过大-继续裂块
//裂块到根的情况
若非根，裂块之后返回一个标记
若为根裂块之后建新根，新根有两个儿子
```

裂块函数：Spilit

## 五、裂块实现

###### 分裂叶子结点

类似块链，分裂以后返回后一个节点的第一个元素，传回上一层函数，提供给其父节点block，父节点将这个key值与son值组合插入到调用的index后面

**leinsert()（叶子节点插入）**

```c++
bool leinsert(Leave &le,const K &key,const ll &value,ll &leave_index,const ll &now_index){
  //le:要插入的对象叶子；kv对；
}
```

//递归过程中的文件读写：哪层读出来哪层写回去

*添加了KV对结构体*

###### 分裂block节点

注意分裂的位置的节点不会存在两个新的block当中，要存入KV对传回上层

### ~~二分查找规则~~

```c++
ll bisearch_leave(const Leave &le,const K &key){
            ll l = 1;//1 base
            ll r = le.num;
            while(l < r){
                ll mid = (l+r-1)>>1;//左二分
                if(compare(le.array[mid].key,key))l = mid+1;
                else r = mid;
            }
            return l;
        }
//找到第一个大于或者等于的位置num
//将num及其本身全部往后移动
// todo 找到最后一个小于or某个等于的
```

```c++
ll index = bisearch_block(b,key);//返回对应key数组序号，son = index+1
ll son_index = b.son[index + 1];//叶子结点在文件当中的位置。
```

// update5.31 写二分太麻烦了， 顺序查找

## 六、remove()//成功删除返回true

判断是否成功删除，返回find情况

采用先删除后调整的方式

### 叶子结点

用指向父节点的指针回溯

若为第一个节点则与右邻居合并/借

否则操作左邻居

合并：并块函数，减少父节点儿子的值并判断

### 树内节点

回到父节点找邻居

//update6.1：实现了

## 七、一对多查找

维护第二关键字，树上节点与叶子节点内部key数组存储结构体Node，重载Node的大于小于等运算符

## 参考代码：

* 如果当前位置是第一个，则与右边邻居借或者合并。如果不是则与左边邻居借或者合并，减少了边界判断。

* 多用指针，可以提速

* Node里面存一个父节点，修改父节点可以通过判断当前位置是不是数组第一个来判断。
* 搜索返回结果可以是一个pair Node* ， int意味node_index（是Node节点当中的第index个位置）

