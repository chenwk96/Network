# URI

URI 本质上是一个字符串，这个字符串的作用是**唯一地标记资源的位置或者名字**。它不仅能够标记万维网的资源，也可以标记其他的，如邮件系统、本地文件系统等任意资源。而“资源”既可以是存在磁盘上的静态文本、页面数据，也可以是由 Java、PHP 提供的动态服务。

#### URI的基本组成

![URI 最常用的形式](https://i.loli.net/2021/06/04/yjIWitwAJrpo8Rx.png)

1. scheme：方案名或者协议名，表示**资源应该使用哪种协议**来访问。最常见的就是HTTP，还有https，ftp，file，news等。如果一个 URI 没有提供 scheme，即使后面的地址再完善，也是无法处理的。
2. 在scheme后，是三个**特定的字符“://”**，把scheme与后面的部分分开
3. “://”是被称为“authority”的部分，表示**资源所在的主机名**，通常的形式是“host:port”，即主机名加端口号。主机名可以是 IP 地址或者域名的形式，必须要有，否则浏览器就会找不到服务器。但端口号有时可以省略，浏览器等客户端会依据 scheme 使用默认的端口号，例如 HTTP 的默认端口号是 80，HTTPS 的默认端口号是 443。
4. path：**标记资源所在位置**，URI 里 path 采用了类似文件系统“目录”“路径”的表示方式。URI 的 path 部分必须以“/”开始，也就是必须包含“/”，不要把“/”误认为属于前面 authority。
5. URI 后面还有一个“**query**”部分，它在 path 之后，用一个“?”开始，但不包含“?”，表示对资源附加的额外要求。查询参数 query 有一套自己的格式，是多个“**key=value**”的字符串，这些 KV 值用字符“**&**”连接，浏览器和客户端都可以按照这个格式把长串的查询参数解析成可理解的字典或关联数组形式。

#### URI的完整格式

![完整格式](https://i.loli.net/2021/06/04/RMaOxIiE5Dyhz3X.png)

这个“真正”形态比基本形态多了两部分。

第一个多出的部分是协议名之后、主机名之前的**身份信息**“user:passwd@”，表示登录主机时的用户名和密码，但现在已经不推荐使用这种形式了（RFC7230），因为它把敏感信息以明文形式暴露出来，存在严重的安全隐患。

第二个多出的部分是查询参数后的**片段标识符**“#fragment”，它是 URI 所定位的资源内部的一个“锚点”或者说是“标签”，浏览器可以在获取资源后直接跳转到它指示的位置。

但片段标识符仅能由浏览器这样的客户端使用，服务器是看不到的。也就是说，浏览器永远不会把带“#fragment”的 URI 发送给服务器，服务器也永远不会用这种方式去处理资源的片段。

#### URI的编码

URI 转义的规则直接把非 ASCII 码或特殊字符转换成十六进制字节值，然后前面再加上一个“%”。





