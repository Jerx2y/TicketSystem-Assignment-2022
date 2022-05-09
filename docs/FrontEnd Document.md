# 2022 TrainTickets-Bonus 前端 Document

> Version 1.0

## 页面设计

### Welcome Page
* Search -> Search Page
* Login -> Sign in/up Page

### Search Page
* search tickets
* search trains

### Sign in/up Page
* input username & password -> Index Page

### Index Page
* Query trains
* Query trains(transfer)
* Buy tickets
* Add trains
* Find users
* Add users
* Change information

## 实现概论

返回新界面： Python + flask (异步：AJAX)

客户端与服务器命令传输： socket(GET/POST)

服务器：网络服务器与数据服务器合并

服务器内处理：通过 Socket 实现 c++ 与 python 进程之间的通信

命令转义： Python 处理接收到的网页命令并转化为 c++ 程序认可的字符串命令