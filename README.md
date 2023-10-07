## Cyber Lighthouse

> 冰岩作坊后端组实习项目

```
Part 1. 使用 tcp & udp 协议，支持简单功能的服务端和客户端
Part 2. 使用 udp 协议的 dns 查询工具 & 本地 dns 缓存服务器 (coding...)
```

developed under linux environment (debian12)

lang: `c++, -std=c++14`

compile: `g++ 12.2.0, cmake 3.25.1`

开发记录在本目录下 `DevLog.md`

---

## Part 1.

使用 tcp & udp 协议，支持简单功能的服务端和客户端

### 编译

```
cd server-side/build
cmake ..
make
```

```
cd client-side/build
cmake ..
make
```

### 使用

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

---

## Part 2.

使用 udp 协议的 dns 查询工具 & 本地 dns 缓存服务器

由于时间不多，加上 `Zackary` 先生的评价（，这部分代码就不拆分多个文件了

### 编译

```
cd dns-server
g++ dns.cpp -o dns
```

### 使用 (dns 记录查询工具)

参考 `dig` 命令

```
./dns <domain name> [@<server ip>:<port>] [-A|NS|CNAME|MX|TXT] [-nr]
```

- `<domain name>` 是一个域名，比如 `baidu.com`, `bingyan.net`, `store.steampowered.com`
- `[@<server ip>:<port>]` 可选项，向 ip 为 `<server ip>` 的 dns 服务器的 `<port>` 端口发送和接受 dns 报文。默认为 `8.8.8.8:53`
- `[-A|NS|CNAME|MX|TXT]` 查询哪一种记录。默认查询 `A` 记录
- `[-nr]` 不使用递归查询。默认会发送递归请求，通过此选项关闭
- 可选项的顺序不固定

超时设置：每次等待三秒，最多重发三遍，如果 udp 包均丢失则报错

工具会把询问信息和收到的回应解析打印在标准输出上，但是只打印 `Answers` 段和 `Authoritative nameservers` 段，忽略 `Additional RRs`。 

`SOA` 类型记录只会打印它的 `MNAME` 段，就是最重要的权威服务器域名（见下例第二次查询）

#### 例

```
$ ./dns baidu.com
===== [Query Info] =====
> Dst IP: 8.8.8.8
> Dst Port: 53
> Recursive Query: 1
> Type: 1 (A)
===== [Response Info] =====
> Transaction ID: 0x2cbb
> Flags: 0x8180 (No Error)
> Questions: 1
> Answer RRs: 2
> Authority RRs: 0
> Additional RRs: 0
===== [Queries] =====
> Name: baidu.com.
> Type: 1 (A)
> Class: 1 (IN)
===== [Answers] =====
> Name: baidu.com., Type: 1 (A), Class: 1 (IN)
> Time To Live: 258s
> Address: 110.242.68.66
---------------
> Name: baidu.com., Type: 1 (A), Class: 1 (IN)
> Time To Live: 258s
> Address: 39.156.66.10
$ ./dns www.madoka-magica.com -NS
===== [Query Info] =====
> Dst IP: 8.8.8.8
> Dst Port: 53
> Recursive Query: 1
> Type: 2 (NS)
===== [Response Info] =====
> Transaction ID: 0x2cbb
> Flags: 0x8180 (No Error)
> Questions: 1
> Answer RRs: 0
> Authority RRs: 1
> Additional RRs: 0
===== [Queries] =====
> Name: www.madoka-magica.com.
> Type: 2 (NS)
> Class: 1 (IN)
===== [Authoritative nameservers] =====
> Name: madoka-magica.com., Type: 6 (SOA), Class: 1 (IN)
> Time To Live: 900s
> Primary Name Server: ns-633.awsdns-15.net.
$ ./dns store.steampowered.com @114.114.114.114:53
===== [Query Info] =====
> Dst IP: 114.114.114.114
> Dst Port: 53
> Recursive Query: 1
> Type: 1 (A)
===== [Response Info] =====
> Transaction ID: 0x2cbb
> Flags: 0x8180 (No Error)
> Questions: 1
> Answer RRs: 1
> Authority RRs: 6
> Additional RRs: 12
===== [Queries] =====
> Name: store.steampowered.com.
> Type: 1 (A)
> Class: 1 (IN)
===== [Answers] =====
> Name: store.steampowered.com., Type: 1 (A), Class: 1 (IN)
> Time To Live: 21s
> Address: 23.44.52.78
===== [Authoritative nameservers] =====
> Name: steampowered.com., Type: 2 (NS), Class: 1 (IN)
> Time To Live: 137983s
> Name Server: a24-64.akam.net.
---------------
> Name: steampowered.com., Type: 2 (NS), Class: 1 (IN)
> Time To Live: 137983s
> Name Server: a9-67.akam.net.
---------------
> Name: steampowered.com., Type: 2 (NS), Class: 1 (IN)
> Time To Live: 137983s
> Name Server: a1-164.akam.net.
---------------
> Name: steampowered.com., Type: 2 (NS), Class: 1 (IN)
> Time To Live: 137983s
> Name Server: a11-67.akam.net.
---------------
> Name: steampowered.com., Type: 2 (NS), Class: 1 (IN)
> Time To Live: 137983s
> Name Server: a26-65.akam.net.
---------------
> Name: steampowered.com., Type: 2 (NS), Class: 1 (IN)
> Time To Live: 137983s
> Name Server: a8-66.akam.net.
$ ./dns bingyan.net -CNAME -nr @198.41.0.4:53
===== [Query Info] =====
> Dst IP: 198.41.0.4
> Dst Port: 53
> Recursive Query: 0
> Type: 5 (CNAME)
===== [Response Info] =====
> Transaction ID: 0x2cbb
> Flags: 0x8180 (No Error)
> Questions: 1
> Answer RRs: 0
> Authority RRs: 1
> Additional RRs: 0
===== [Queries] =====
> Name: bingyan.net.
> Type: 5 (CNAME)
> Class: 1 (IN)
===== [Authoritative nameservers] =====
> Name: bingyan.net., Type: 6 (SOA), Class: 1 (IN)
> Time To Live: 600s
> Primary Name Server: dns1.hichina.com.
$ ./dns bilibili.com -NS @198.41.0.4:53
===== [Query Info] =====
> Dst IP: 198.41.0.4
> Dst Port: 53
> Recursive Query: 1
> Type: 2 (NS)
===== [Response Info] =====
> Transaction ID: 0x2cbb
> Flags: 0x8180 (No Error)
> Questions: 1
> Answer RRs: 2
> Authority RRs: 0
> Additional RRs: 16
===== [Queries] =====
> Name: bilibili.com.
> Type: 2 (NS)
> Class: 1 (IN)
===== [Answers] =====
> Name: bilibili.com., Type: 2 (NS), Class: 1 (IN)
> Time To Live: 16990s
> Name Server: ns4.dnsv5.com.
---------------
> Name: bilibili.com., Type: 2 (NS), Class: 1 (IN)
> Time To Live: 16990s
> Name Server: ns3.dnsv5.com.
```

### 使用 (本地 dns 缓存服务器)

《  锐  意  制  作  中  》







