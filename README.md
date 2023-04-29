# PlayerServer项目架构
# 1 进程模块
## 1.1 进程关系分析
<div align=center>
<img src="https://user-images.githubusercontent.com/126315502/235277196-69ec6789-1a28-41a5-ba9d-2a474b03ce8e.png" width="500px" height="400px">
<img src="https://user-images.githubusercontent.com/126315502/235277256-817b7199-87fd-487a-95c3-c17c2b405cbc.png" width="500px" height="400px">
</div>

## 1.2 进程模块的实现方案

需要两个功能：
（1）灵活的进程入口函数
（2）进程间传递文件描述符<br>
第一个功能使用C++模板来实现，增加可移植性
## 1.3 进程模块测试
<div align=center>
<img src="https://user-images.githubusercontent.com/126315502/235277827-fb19c668-02b0-46ba-886d-0f34ccfb711d.png" width="400px" height="400px">
</div>

## 1.4 守护进程实现
<div align=center>

<img src="https://user-images.githubusercontent.com/126315502/235277974-8e546e60-c5a6-42ec-a3b1-75257681ae88.png" width="450px" height="500px">
</div>

# 2 日志模块
## 2.1 日志模块设计分析
用本地套接字实现进程间通信<br>
（1）无需IP和端口，不影响服务器对外的资源
（2）信息无需加锁，可以多线程并发写
（3）数据传输量巨大，传输速率高（纯内存读写）<br>
<div align=center>
<img src="https://user-images.githubusercontent.com/126315502/235278141-f395d6f2-8ea7-4bfa-8f58-b0fe219b55fd.png" width="400px" height="400px">
</div>

## 2.2 Epoll的封装

<div align=center>
<img src="https://user-images.githubusercontent.com/126315502/235278189-2f558900-ac19-4209-9817-93afe90cac6a.png" width="700px" height="400px">
</div>

## 2.3 本地套接字的封装

<div align=center>
<img src="https://user-images.githubusercontent.com/126315502/235278250-c3cd5dd8-4b3e-45fe-a651-cbe8b9c58b1e.png" width="400px" height="500px">
</div>

## 2.4 线程的封装

<div align=center>
<img src="https://user-images.githubusercontent.com/126315502/235278280-63947352-b7db-462c-977e-b8329ab0cb1a.png" width="300px" height="350px">
</div>

## 2.5 日志模块的设计

<div align=center>
<img src="https://user-images.githubusercontent.com/126315502/235278331-70718d09-94c4-4bd1-ae89-ca4762c9ae13.png" width="700px" height="400px">
</div>

# 3 主模块

## 3.1 主模块的设计
<div align=center>
<img src="https://user-images.githubusercontent.com/126315502/235278410-efa6fbb6-01ff-4280-aea2-f7a5fa14b473.png" width="700px" height="400px">
</div>

## 3.2 线程池的设计
<div align=center>
<img src="https://user-images.githubusercontent.com/126315502/235278445-848ead0a-af29-428a-aa6c-a7b152c9c4a9.png" width="700px" height="100px">
</div>

## 3.3 主模块的设计

<div align=center>
<img src="https://user-images.githubusercontent.com/126315502/235278588-a9401f8a-e902-4be4-9071-547883cd1ccf.png" width="600px" height="400px">
</div>

## 3.4 客户端处理模块的设计

<div align=center>
<img src="https://user-images.githubusercontent.com/126315502/235278647-ef5c8d01-61d1-471a-9dab-56d4f6066776.png" width="400px" height="400px">
</div>

## 3.5 HTTP模块的设计

<div align=center>
<img src="https://user-images.githubusercontent.com/126315502/235278690-9847b232-9baa-4f14-9e66-7ff6e16d8565.png" width="200px" height="300px">
</div>

## 3.6 数据库模块设计

<div align=center>
<img src="https://user-images.githubusercontent.com/126315502/235278819-f1563526-3b6b-4e03-8eca-0e2da3dcef8c.png" width="500px" height="400px">
</div>

## 3.7 加密模块的设计

<div align=center>
<img src="https://user-images.githubusercontent.com/126315502/235278847-5ef2dba2-759f-4ec7-a3f5-3873fa7d0040.png" width="400px" height="600px">
</div>

# 4 业务处理模块

## 4.1 业务流程

<div align=center>
<img src="https://user-images.githubusercontent.com/126315502/235278925-00037582-f0aa-491a-8116-5a661e3a8d3f.png" width="200px" height="600px">
</div>

## 4.2 服务器处理流程

<div align=center>
<img src="https://user-images.githubusercontent.com/126315502/235278957-d1c6e8fa-5f4e-4bcb-9ae7-c62adcb0fb2d.png" width="200px" height="600px">
</div>