# TicketSystem Document

> [SJTU CS1951 课程大作业](https://github.com/ACMClassCourse-2021/TicketSystem)

## Version

3.1

## 然然今天吃火车🚂
|Contributor | BackEnd | FrontEnd |
| -------- | -------- | -------- |
| WYH | 逻辑 | 前后端交互 |
| JL | B+ 树 | 前端设计 |

## BONUS
缓存，空间回收，命令行交互，前端（施工中...)

## 代码文件 hpp/cpp

### 模块划分
![](https://raw.githubusercontent.com/Jerx2y/TicketSystem-Assignment-2022/w/docs/codefile.jpeg)

### 工具模块 `/lib`

+ `utils.hpp` 实现一些工具函数
+ `hash.hpp` 实现 hash 函数
+ `linked_hashmap.hpp` 实现 hashmap
+ `vector.hpp` 实现 vector
+ `storage.hpp` 实现文件读写类
+ `BPlustree.hpp` 实现 B+ 树
+ `user.hpp` 包含与 user 有关的类
+ `train.hpp` 包含与 train 有关的类
+ `order.hpp` 包含与 order 有关的类
+ `rollback.hpp` 包含 rollback 相关的类

### 管理模块 `/src`

+ `manager.hpp` 负责命令的执行
+ `main.cpp` 火车票系统的调用

## 工具类及其接口 class
> 设以下代码 class 中的函数均为 public 类型

### 文件读写 Storage

 * 提供了一个模版类file class进行文件读写。
 * 顺序存储数据并返回地址。
 * 支持对特定地址的内容进行文件读写

```cpp

class Storage {
    Storage(std::string filename); // 存储文件名
    void read(T &, int); // 实现从文件 index 位置读入 value
    void write(T &, int); // 实现将 value 写入 index 位置
    void update(T &, int); // 更新
    void ReadInfo(int &, int); // 读入文件头信息
    void WriteInfo(int , int); // 写入文件头信息
};
```

### 文件读写栈 FileStack
```cpp
class FileStack {
    FileStack(std::string filename); // 存储文件名
    void push(const T &);
    void gettop(T &); // 取堆顶元素
    void pop();
    bool empty();
    int size();
};
```

### 文件存储结构 map
 * 实现模版类的 key -> value 的映射，支持关键字查询，删除，修改
 * 带有 Cache
```c++
class map {
    map(const std::string &); // 用文件名 初始化 
    void Getone(const T &key, S &var); // 查询 key 对应的值，存储在 var 中
    void Get(const T &l, const T &r, vector<S> &var); // 查询 l 到 r 之间的所有值，存储在 var 中。
    void Insert(const T &, const S &);
    void Remove(const T &); // 删除 key 值
    void Modify(const T &); // 修改 key 值
    bool count(const T &); // 查询 key 值是否存在
};
```

### 动态数组 vector

实现类似于 std::vector 的数据结构

### 哈希表 linked_hashmap

实现类似于 std::unordered_map 的数据结构

### 语法分析 parse

```cpp
class Parse {
    void read(char c_); // 读入字符串，以 c_ 为分隔符
    void set(char c_, const std::string &str_); // 设置字符串 str_，以 c_ 为分隔符
    std::string nextToken(); // 获取下一个信息串
};
```

### 命令值 Cmd

```cpp
class Cmd {
    std::string get(char ch) const; // 返回 -ch 的值
    void set(char ch, const std::string &str); // 将 -ch 的值设为 str
    bool have(char ch) const; // 判断 -ch 是否有值
    void reset(); // 将所有值清零
};
```

### 字符值 Varchar

```cpp
struct Varchar {
    size_t var; // 储存字符串的 hash 值
    Varchar(const std::string &str);
};

```

### 时间 date

```cpp
class Date {
    int now; // 01-01 00:00 至 now 时刻所经过的分钟数
    Date(int now_);// 用 now 构造类
    Date(int mm, int dd, int hr = 0, int mi = 0); // 用 mm-dd hr:mi 构造类
    void set_mmdd(const std::string &str); // 用 mm-dd 字符串构造 整数天的类
    void addDay(int x = 1); // 自增 x 天
    std::string get_mmdd() const; // 返回 mm-dd 形式的字符串
    std::string get_hrmi() const; // 返回 hr:mi 形式的字符串
    std::string get_mmddhrmi() const; // 返回 mm-dd hr:mi 形式的字符串
};

```

### 管理 Manager

```cpp
class Manager {
    std::string add_user(Cmd);
    std::string query_profile(const Cmd &);
    std::string modify_profile(const Cmd &);
    std::string add_train(const Cmd &);
    std::string release_train(const Cmd &);
    std::string query_train(const Cmd &);
    std::string query_ticket(const Cmd &);
    std::string query_transfer(const Cmd &);
    std::string buy_ticket(const Cmd &);
    std::string refund_ticket(const Cmd &);
    std::string query_order(const Cmd &);
    std::string login(const Cmd &);
    std::string logout(const Cmd &);
    std::string clean();
    std::string rollback();
};
```

## 存储类

### 用户 User

```cpp
class User {
    char username[USERNAME_LEN];
    char name[NAME_LEN]; //储存中文姓名，假设每个汉字占 3 字节 
    char mailAddr[PASSWORD_LEN];
    int privilege;
    size_t usernameHash, passwordHash;  //存储密码的 hash 值 
};
```

### 车次 Train

```cpp
class Train {
    char trainID[TRAINID_LEN];
    size_t trainIDhash;
    int stationNum;
    int seatNum;
    char stationID[STATION_NUM][STATIONID_LEN]; // 储存每个站的中文名，假设每个汉字占 3 字节 
    int prices[STATION_NUM]; // 储存站 i 到站 i+1 的票价 
    int startTime; // 始发站发车的时间离当天 0 点经过的分钟数
    int arriveTime[STATION_NUM], leaveTime[STATION_NUM]; // 第 i 站的到达和发车时间
    Date startDate, endDate;
    char type;
    bool released;
};
```

### 订单 Order
```cpp
class Order {
    char trainID[TRAINID_LEN];
    char startStation[STATIONID_LEN];
    char endStation[STATIONID_LEN];
    Date day; // 出发日期
    Date leavingTime; // 开车时间 
    Date arrivingTime; // 到达时间
    int ids; // 始发站编号
    int idt; // 终到站编号
    int prices, num;
    int status; // 订单状态
};
```

## rollback 实现方法

rollback 的实现依托 BPTree，需要将对 BPTree 的每个命令（插入、删除、修改）记录在文件读写栈中，需要回溯时依次读取并反向操作。