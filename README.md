# TicketSystem 开发文档

> [SJTU CS1951 课程大作业](https://github.com/ACMClassCourse-2021/TicketSystem)

## Version

2.1

## 然然今天吃火车🚂
|Contributor | Mission |
| -------- | -------- |
| Wang Yuhao | 后端逻辑、前端 |
| Jiang Lai | B+ 树、前端 |


## 代码风格
Google

## 模块划分

[![](https://s1.ax1x.com/2022/05/18/O7uOoT.jpg)](https://imgtu.com/i/O7uOoT)

### 存储模块 `/file`

包含文件读写，文件数据结构。

### 工具模块 `/lib`

包含 `vector`, `linked_hashmap` 等数据结构以及一些其他工具。

### 管理系统模块 `/src`

包含火车票系统管理命令的实现。

## 类 class

### 文件读写 File

```cpp
/*
 * 提供了一个模版类file class进行文件读写。
 * 顺序存储数据并返回地址。
 * 支持对特定地址的内容进行文件读写
 */

template<class T, int n = 1>
class File {
private:
    fstream file; // 文件
    const string file_name_; // 文件名
    int sizeofInt = sizeof(int);
    int sizeofT = sizeof(T);
public:
    void Read(T &value, int index) // 实现从文件 index 位置读入 value
    void Write(T &value,int index) // 实现将 value 写入 index 位置
    int Add(T &value) // 实现在新的位置写入 value 并返回位置
    void ReadInfo(int &tmp) // 读入文件头信息
    void WriteInfo(int tmp) // 写入文件头信息
};
```

### 文件存储结构 BPT
```c++
/*
 * 实现模版类的 value ，支持关键字查询，删除，修改
 * 支持一个 key 值对应多个 value ，但是需要使用者自己实现 value 的比较函数
 * 支持根据 key 添加、删除、查找 value
 */
template<class T,class S, int n = 1>
class BPTNode {
private:
    T key; S value;
    size_t sizeofkey = 0;
};

class BPT {
    const std::string file_name; // 存储 BPT 数据的文件名
    BPT(const std::string &file); // 用文件名 file 初始化 
    void AddNode(const BPTNode &BPTNode);
    int DeleteNode(const BPTNode &BPTNode);
    void FindOneNode(const T &key_, S &var); // 根据 key 查询最小的 value
    void FindAllNode(const T &key_, std::vector<S> &array0); // 根据 key 查询所有 value
};
```

### 异常 Exception
```cpp
class Exception {
    std::string message //抛异常时给 message 赋值，以判断异常的类型 
    std::string what() // 返回 message
};
```

### 动态数组 vector

实现类似于 std::vector 的数据结构

### 哈希表 linked_hashmap

实现类似于 std::unordered_map 的数据结构

### 链表 list

实现类似于 std::list 的数据结构

### 日期 Date
```cpp
class Date {
    int now //将 06-01 00:00 到此时刻的时间转化为分钟存储 
    bool in(int m, int d) //判断此时刻是否在某天当中 
    Date(int m, int d, int hour, int min) //将某时刻转化为 now 
    Date(int m, int d) //将某天看做 0 时 0 分转化为 now 
};
```

### 用户 User

```cpp
class User {
    char username[21]
    int usernameHash //储存 username 的 hash 值 
    char name[21] //储存中文姓名，假设每个汉字占 4 字节 
    char mailAddr[31]
    int privilege
    int passwordHash  //存储密码的 hash 值 
};
```

### 车次 Train

```cpp
class Train {
    char trainID[21]
    int stationNum
    char stationID[100][42] //储存每个站的中文名，假设每个汉字占 4 字节 
    int seatNum[100] //储存站 i 到站 i+1 的座位数 
    int prices[100] //储存站 i 到站 i+1 的票价 
    Date startTime //始发站发车时间 
    int travelTimes[100] //储存站 i 到站 i+1 的用时 
    int stopoverTimes[100] //储存站 i 的停站时间 
    Date startDate, endDate
    char type
    bool released
};
```

### 订单 Order
```cpp
class Order {
    int orderID;
    char username[21];
    char trainID[21];
    char startStation[41];
    char endStatioin[41];
    int num; // 订单数量
    int price; // 票价
    int status; // 0,1,2 分别为 购票成功，候补中，退票
};
```

### 日志 Log
```cpp
/*
 * 管理日志文件，记录操作
 * 实现回滚
 */
class Log {
    File log; // 操作日志文件
    void take() // 记录操作
    void rollback() // 实现回滚操作
};
```

### 管理 TicketSystem
```cpp
class TicketSystem {
    Log recorder;
    linked_hashmap online; // 存储在线用户池
    BPT user; // 根据 username 查询 User
    BPT train; // 根据 trainID 查询 Train
    BPT station_train // 根据 stationID 查询 trainID
    BPT order; // 根据 orderID 查询 Order
    BPT user_order; // 根据 username 查询 orderID
    list<int> queue; // 按时间顺序存储候补订单的编号
    void add_user();
    void query_profile();
    void modify_profile();
    void add_train();
    void release_train();
    void query_train();
    void query_ticket();
    void query_transfer();
    void buy_ticket();
    void refund_ticket();
    void query_order();
    void login();
    void logout();
    void clean();
    void exit();
};
```

## 文件结构

### 代码文件

> 文件按照包含关系的拓扑序排列

#### exception.h

+ 实现 Exception 类

#### vector.h

+ 实现 vector 类

#### linked_hashmap.h

+ 实现 linked_hashmap 类

#### list.h

+ 实现 list 类

#### File.h

+ 实现 File 类

#### BPT.h

+ 实现 BPT 类

#### Date.h

+ 实现 Date 类

#### User.h

+ 实现 User 类

#### Train.h

+ 实现 Train 类

#### Order.h

+ 实现 Order 类

#### Log.h

+ 实现 Log 类

#### TicketSystem.h

+ 实现 TicketSystem 类

#### utils.h

+ `std::string nextToken()` （处理输入命令）
+ `int stringToInt()` （转 string 到 int） 
+ 以及其他必要的工具函数

#### main.cpp

+ 输入命令处理
+ 检测并指定操作类型
+ 异常处理

### 数据索引文件

#### user.idx
+ 存储 username 到 User 的索引

#### train.idx
+ 存储 trainID 到 Train 的索引

#### order.idx
+ 存储 orderID 到 Order 的索引
    
#### user_order.idx
+ 存储 username 到 orderID 的索引

#### station_train.idx
+ 存储 stationID 到 trainID 的索引

### 日志文件

#### log
+ 按照时间顺序存储操作命令，具体格式同输入格式

## Notes
（暂无）