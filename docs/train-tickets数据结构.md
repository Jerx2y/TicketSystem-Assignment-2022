# Train-ticket 数据结构

## 一、文件读写

### 功能说明

提供了一个模版类file class进行文件读写。

顺序存储数据并返回地址。

支持对特定地址的内容进行文件读写

```c++
template<class T, int n = 1>
class File{
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

## 二、数据主体BPT

### 功能说明

* 实现模版类的value，支持关键字查询，删除，修改

* 支持一个key值对应多个value，但是需要使用者自己实现value的比较函数

### 提供接口如下

```c++
template<class T,class S, int n = 1>
class BPTNode {
private:
    S value;
    T key;
    size_t sizeofkey = 0;
};
class BPT {
    const std::string file_name; // 存储 BPT 数据的文件名
    BPT(const std::string &file); // 用文件名 file 初始化 
    void AddNode(const BPTNode &BPTNode);
    int DeleteNode(const BPTNode &BPTNode);
    void FindNode(const T &key_, std::vector<S> &array0);
    // 功能如字面意思
};
```