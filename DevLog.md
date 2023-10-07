## 10.1

中午拿到了实习任务，在 `Zackary` 先生的建议下选择 `Cyber Lighthouse`（

![](https://z1.ax1x.com/2023/10/07/pPjwY3d.png)

建立仓库，建立 ssh 连接

debian 下配置开发环境，安装 vscode 和插件

设置 vmware 共享文件夹

makefile 快速入门

cmake 快速入门

> Ref:
>
> [cmake使用教程（实操版）](https://blog.csdn.net/qq_34796146/article/details/108877159)
>
> [CMake内部构建与外部构建_build central cmake](https://blog.csdn.net/hubing_hust/article/details/128505399)

## 10.2

阅读读 《TCP/IP 网络编程》前 120 页

socket 初体验

实现了一个简单的 tcp 回声服务端 & 客户端，不过实现很烂

重新组织了文件目录，把原来的单个文件拆解成了若干模块，放在 `src` 目录下

使用 cmake 生成 makefile，在 build 目录外部构建

> Ref:
>
> 《TCP / IP 网络编程》, [韩]尹圣雨

## 10.3

调整代码，重构了服务端和客户端的运行方式

现在服务端集成了各种服务，启动时确定是否支持一些服务，而不是以一种特定功能启动

使用了多线程，服务端在 `start` 之后主线程负责进入命令行模式等待输入，另外创建 `tcp_listener` 和 `udp_listener` 两个线程，这两个线程接听所有的 tcp 和 udp 连接请求，对于每个成功连接再开线程执行服务

添加了 `print` 模式，在服务端标准输出打印从客户端收到的数据

`tcp` 和 `udp` 可以在服务端运行过程中实时开关，需要比较好的手段动态控制线程阻塞 

`tcp_listener` 和 `udp_listener` 是否被阻塞通过条件变量控制，大致流程如下

```
while (!program_end) {
	while (!enable && !program_end)
		cv.wait(lck); // 使用锁和条件变量阻塞线程
		
	socket();
	bind();
	listen();
	while (enable) {
		accept();
	}
	close();
}
```

需要更改服务开关时首先更改 `enable` 变量，然后 `cv.notify_all()` 刷新即可

被关闭的服务会被锁阻塞住，不占用 cpu

---

服务端的命令行现在可用了，可以在运行期间使用 `config` 动作修改服务设置，即时生效

使用 `shutdown` 命令时会直接分离两个 `listener` 线程然后结束，不再等待

> Ref:
>
> [std::thread - cppreference.com](https://en.cppreference.com/w/cpp/thread/thread)
>
> [C++11多线程入门（2）——std::mutex](https://www.bilibili.com/read/cv3834615/)
>
> [C++11多线程编程(三)——lock_guard和unique_lock](https://zhuanlan.zhihu.com/p/340348726)
>
> [详解 C++ 多线程的condition_variable](https://zhuanlan.zhihu.com/p/458481436?utm_id=0)

## 10.4

写 README 和 DevLog

修复了一点小 bug（udp 连接写入和读取有时不对应造成问题）

补全了 udp 协议的 `print` 服务

暂且休息一下

## 10.5

学习 dns 协议的处理流程，dns 服务器分类

`dig` 命令快速入门

用 `dig` 把 dns 报文发到本机端口后由程序接收，看看二进制包长啥样

学习了一部分报文格式

`wireshark` 快速入门，捕捉用 `dig` 查询时发出和收到的报文，查看报文

> Ref:
>
> [RFC 1035](https://www.rfc-editor.org/rfc/rfc1035)
>
> [linux dig命令详解 - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/612410033)
>
> [DNS报文格式解析（非常详细）](http://c.biancheng.net/view/6457.html)
>
> [DNS报文格式 - 报文格式大全](http://luhalu.cn/message/1115)

## 10.6

终于整出来一个简单的 dns 查询工具，但是功能太简陋了，输出内容还是 16 进制报文

添加了一些程序选项，添加查询超时设置，添加了每一类 `RR` 的解析和打印

调整输出格式，调调调，依据 wireshark 的样子改了一遍

坐车返校，赶末班地铁

（火车上 udp 丢包率惊人）

> Ref:
>
> [网络编程（13）高级IO函数_sendmsg和recvmsg函数 阻塞](https://blog.csdn.net/wanggao_1990/article/details/109296400)

## 10.7

改进细节，删了一点过程文件

研究了递归查询和记录缓存的实现方法，但是这时候时间已经不多了

据说晚上要一块吃饭？！/jk 

那就真 rush 不出来了，只能紧急把文档补一下

之前搞 socket 工时太多，后面这个 dns 部分搞不完力 /kk

> Ref:
>
> [DNS报文格式（RFC1035） - 北岛知寒](https://www.cnblogs.com/crazyacking/p/5670045.html)
>
> [【网络】DNS记录类型_soa记录和ns记录_wecode66的博客](https://blog.csdn.net/sepnineth/article/details/129819718)

---

> Extra Ref:
>
> [Commit message 和 Change log 编写指南 - 阮一峰的网络日志](http://www.ruanyifeng.com/blog/2016/01/commit_message_change_log.html)
>
> [基于C++11实现的阻塞队列(BlockQueue)_c++ 阻塞队列_{(sunburst)}的博客](https://blog.csdn.net/weixin_50437588/article/details/128434180)




