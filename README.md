# Hello World!
学习TCP/IP, 以及HTTP时的一些记录。

## TCP 重传、滑动窗口、流量控制、拥塞控制

### 重传机制

TCP实现可靠传输的方式之一，是通过序列号与确认应答。

 TCP 针对数据包丢失的情况，会⽤重传机制解决：

* 超时重传：发送端在发送数据时，设定⼀个定时器，当超过指定的时间后，没有收到对⽅的 ACK 确认应答报⽂，就会重发该数据。

  ![](https://raw.githubusercontent.com/chenwk96/picgoImage/main/20210705200538.png)

  两种超时时间不同的情况（同RTT，Round-Trip Time 往返时延比较）：

  * 当超时时间 RTO 较⼤时，重发就慢，效率低，性能差； 

  * 当超时时间 RTO 较⼩时，会导致可能并没有丢就重发，于是重发的就快，会增加⽹络拥塞，导致更多的超时，更多的超时导致更多的重发。

    ![](https://raw.githubusercontent.com/chenwk96/picgoImage/main/20210705201130.png)

    > 超时重传时间 RTO 的值应该略⼤于报⽂往返 RTT 的值。
    >
    > 实际中的「报⽂往返 RTT 的值」是经常变化的，因为我们的网络也是时常变化的。也就因为「报⽂往返 RTT 的值」 是经常波动变化的，所以「超时重传时间 RTO 的值」应该是⼀个动态变化的值
    >
    > 如果超时重发的数据，再次超时⼜需要重传的时候，TCP 的策略是**超时间隔加倍**，即每当遇到一次超时重传，都会将下一次超时时间间隔设为先前值的两倍。两次超时，说明网络环境擦汗，不易频繁反复发送。
    >
    > 超时触发重传存在的问题是超时周期可能相对较⻓。

* 快速重传：不以时间为驱动，而是以数据驱动重传。 其⼯作⽅式是当收到三个相同的 ACK 报⽂时，**会在定时器过期之前**，重传丢失的报⽂段。

  ![](https://raw.githubusercontent.com/chenwk96/picgoImage/main/20210705201838.png)

  > 快速重传解决了超时时间的问题，但是面临另一个问题，重传的时候，**是重传之前的一个还是重传所有的问题。**根据TCP不同的实现，以上两种情况均有可能。

* SACK：为了解决不知道该快重传哪些 TCP 报⽂，于是就有 SACK （selective acknowledgement 选择性确认）方法。

  ![](https://raw.githubusercontent.com/chenwk96/picgoImage/main/20210705202400.png)

  这种方式需要在 TCP 头部「选项」字段里加⼀个 SACK 的东西，它可以将缓存的地图发送给发送方，这样发送方就可以知道哪些数据收到了，哪些数据没收到，知道了这些信息，就可以只重传丢失的数据

  > 在Linux下，可以用通过net.ipv4.tcp_sack参数打开SACK功能。并且SCAK功能需通信双方均支持。

* D-SACK：Duplicate-SACK，主要使用SACK来告诉发送方有哪些数据被重复接收。

  * ACK 丢包

    ![](https://raw.githubusercontent.com/chenwk96/picgoImage/main/20210705203505.png)

  * 网络延迟

    ![](https://raw.githubusercontent.com/chenwk96/picgoImage/main/20210705203626.png)

  > 在 Linux 下可以通过 net.ipv4.tcp_dsack 参数开启/关闭这个功能（Linux 2.4 后默认打开）。
  >
  > D-SACK可以让发送方知道是发出去的包丢了还是接收方回应的ACK包丢了，可以知道是不是发送方的数据包被网络延迟了、可以知道网络中是不是把发送方的数据包给复制了。



### 滑动窗口

为每个数据包都确认应答的缺点：包的往返时间越长，网络的吞吐量会越低。为解决这个问题，TCP 引入了**窗口**这个概念。即使在往返时间较长的情况下，它也不会降低网络通信的效率。

窗口大小就是指**无需等待确认应答，而可以继续发送数据的最大值**。窗口的实现实际上是操作系统开辟的一个缓存空间，发送方主机在等到确认应答返回之前，必须在缓冲区中保留已发送的数据。如果按期收到确认应答，此时数据就可以从缓存区清除。

![](https://raw.githubusercontent.com/chenwk96/picgoImage/main/20210705204640.png)

图中的 ACK 600 确认应答报文丢失，也没关系，因为可以通话下一个确认应答进行确认，只要发送方收到了 ACK 700 确认应答，就意味着 700 之前的所有数据「接收方」都收到了。这个模式就叫**累计确认**或者**累计应答**。

**窗口的大小由接收方决定：**

TCP 头里有一个字段叫 `Window`，也就是窗口大小。**这个字段是接收端告诉发送端自己还有多少缓冲区可以接收数据。于是发送端就可以根据这个接收端的处理能力来发送数据，而不会导致接收端处理不过来。**发送方发送的数据大小不能超过接收方的窗口大小，否则接收方就无法正常接收到数据。

**发送方的窗口**

![](https://raw.githubusercontent.com/chenwk96/picgoImage/main/20210705205743.png)

- `SND.WND`：表示发送窗口的大小（大小是由接收方指定的）；
- `SND.UNA`：是一个绝对指针，它指向的是已发送但未收到确认的第一个字节的序列号，也就是 #2 的第一个字节。
- `SND.NXT`：也是一个绝对指针，它指向未发送但可发送范围的第一个字节的序列号，也就是 #3 的第一个字节。
- 指向 #4 的第一个字节是个相对指针，它需要 `SND.UNA` 指针加上 `SND.WND` 大小的偏移量，就可以指向 #4 的第一个字节了。

**接收方的窗口**

![](https://raw.githubusercontent.com/chenwk96/picgoImage/main/20210705205624.png)

- `RCV.WND`：表示接收窗口的大小，它会通告给发送方。
- `RCV.NXT`：是一个指针，它指向期望从发送方发送来的下一个数据字节的序列号，也就是 #3 的第一个字节。
- 指向 #4 的第一个字节是个相对指针，它需要 `RCV.NXT` 指针加上 `RCV.WND` 大小的偏移量，就可以指向 #4 的第一个字节了。

### 流量控制

