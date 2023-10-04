## 10.1

建立仓库，建立 ssh 连接，配置开发环境

cmake 快速入门

## 10.2

读 《TCP/IP 网络编程》前 120 页

上手 socket

实现了一个简单的 tcp 回声服务端 & 客户端，不过实现很烂

重新组织文件目录，把原来的单个文件拆解成了若干模块

使用 cmake 生成 makefile，在 build 文件夹下外部构建

## 10.3

调整代码，重构了服务端和客户端的运行方式

现在服务端集成了各种服务，启动时确定是否支持一些服务，而不是以一种特定功能启动

使用了多线程，服务端在 `start` 之后主线程负责进入命令行模式等待输入，另外创建 `tcp_listener` 和 `udp_listener(未完成)` 两个线程，这两个线程接听所有的 tcp 和 udp 连接请求，对于每个成功连接再开线程执行服务

添加了 `print` 模式

`tcp_listener` 和 `udp_listener` 是否被阻塞通过条件变量控制，流程如下

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

服务端的命令行现在可用了，可以在运行期间使用 `config` 动作修改服务设置

## 10.4

重新写了 README 和 DevLog

修复了一点小 bug







