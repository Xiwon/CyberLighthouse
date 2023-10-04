## Cyber Lighthouse

> 冰岩作坊后端组实习项目

developed under linux environment (debian12)

lang: `c++, -std=c++14`

compile: `g++ 12.2.0, cmake 3.25.1`

开发记录在本目录下 `DevLog.md`

---

### 编译

```
cd server-side/build
cmake ..
make
./clh
```

```
cd client-side/build
cmake ..
make
./clh-clnt
```

## 用法

#### 服务端

```
./clh start <port> [OPTIONS...]
```

本机启动服务端，端口为 `<port>`，要求端口号在 `1024` 及以后

```
OPTIONS:
	-e, --echo 支持客户端回声连接, 服务端原样发回原包. 默认关闭
	-p, --print 支持客户端发送包, 收到的数据会打印在标准输出流. 默认关闭
	--tcp, --ntcp 启用/禁用 tcp 协议连接. 默认开启 tcp
	--udp, --nudp 启用/禁用 udp 协议连接. 默认关闭 udp
```

在执行 `start` 动作之后服务端会进入命令行模式，提示符 `#clh cmd>`

```
#clh cmd> config [OPTIONS...]
```

此时使用 `config` 可以调整 `OPTIONS` 里各项服务的开关

tcp/udp 由开启切换为关闭时会处理完当前连接再关闭

```
#clh cmd> shutdown
```

执行此命令时立即断开所有连接并关闭服务端

#### 客户端

```
./clh-clnt connect <ip>:<port> -e [OPTIONS...]
./clh-clnt connect <ip>:<port> --echo [OPTIONS...]
```

以回声模式连接到 `<ip>:<port>`

客户端进入命令行模式，提示符 `#echo-client input>`

如果此时服务端不支持回声连接，输入内容之后会提示

```
#message from server> echo service not supported
```

并关闭客户端

否则与服务端连接成功，输入的信息发送至服务端，再被原样返回，打印在标准输出流上

---

```
./clh-clnt connect <ip>:<port> -p <content> [OPTIONS...]
./clh-clnt connect <ip>:<port> --print <content> [OPTIONS...]
```

打印模式连接到 `<ip>:<port>`，向该地址发送 `<content>` 内容

如果服务端不支持打印连接，会输出

```
#message from server> print service not supported
```

否则输出

```
#message from server> ok, received <number> bytes
```

---

可选项：

```
OPTIONS
	--tcp, --nudp 使用 tcp 协议连接
	--udp, --ntcp 使用 udp 协议连接
```

#### 例

服务端

```
$ ./clh start 9190
command line running
[tcp protocol]: 0 -> 1
`shutdown` to close server
#clh cmd> config --print
print service enabled
[print service]: 0 -> 1
#clh cmd> 
print service started
#message from client> hello world
config -np
print service disabled
[print service]: 1 -> 0
#clh cmd> rejected print
config -e
echo service enabled
[echo service]: 0 -> 1
#clh cmd> 
echo service started

data package retransmitted

data package retransmitted

data package retransmitted
shutdown
[echo service]: 1 -> 0
[tcp protocol]: 1 -> 0
detach all connections and shutdown...
$
```

客户端

```
$ ./clh-clnt connect 127.0.0.1:9190 -p "hello world"
using print mode
start print-client connected to 127.0.0.1, port 9190
connected..........
#message from server> ok, received 11 bytes
$ ./clh-clnt connect 127.0.0.1:9190 -p "hello world"
using print mode
start print-client connected to 127.0.0.1, port 9190
connected..........
#message from server> print service not supported
$ ./clh-clnt connect 127.0.0.1:9190 -e
using echo mode
start echo-client connected to 127.0.0.1, port 9190
connected..........
Q/q to quit connection
#echo-client input> text1
#message from server> text1
#echo-client input> text2
#message from server> text2
#echo-client input> text3
#message from server> text3
#echo-client input> q
$ 
```











