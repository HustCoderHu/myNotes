linux 上鲜为人知的那些命令
---

# 多任务
很多时候我们只有一个终端界面，而执行的任务耗时又很长
## 重定向
诸如 `cmd > log.txt 2>&1` 表示将stderr和stdout合并重定向到log
- 标准输入文件(stdin)：stdin的文件描述符为0，Unix程序默认从stdin读取数据
- 标准输出文件(stdout)：stdout 的文件描述符为1，Unix程序默认向stdout输出数据
- 标准错误文件(stderr)：stderr的文件描述符为2，Unix程序会向stderr流中写入错误信息
```
`>>`则是将新内容添加在文件末尾  

这种方式执行时，终端上不会显示out和err的输出。如果要同时输出到屏幕和文件就用`tee`
```
cmd 2>&1 | tee log.txt
# 如果不加 2>&1 就只有 stdout 到文件
```

上述两种方式遇到 ctrl + c 还是会终止命令，最完善的方案是  
** GNU Screen **  
GNU Screen是一款由GNU计划开发的用于命令行终端切换的自由软件。用户可以通过该软件同时连接多个本地或远程的命令行会话，并在其间自由切换。  
GNU Screen可以看作是窗口管理器的命令行界面版本。它提供了统一的管理多个会话的界面和相应的功能。  
常用命令如下
```
# 启动一个 session
screen
# 新建一个叫 s_id 的 session
screen -S s_id
# 列出当前所有的 session
screen -ls
# 回到 s_id 这个session
screen -r s_id
远程 detach 某个 session
screen -d yourname

# 在 session 内时，所有命令都以 ctrl+a(C-a) 开始
# 显示所有键绑定信息
?

# detach，暂时离开当前session，将目前的 screen session (可能含有多个 windows) 丢到后台执行，并会回到还没进 screen 时的状态，此时在 screen session 里，每个 window 内运行的 process (无论是前台/后台)都在继续执行，即使 logout 也不影响
d

# 创建一个新的运行shell的窗口并切换到该窗口
c

# 显示所有窗口列表
w

# session 重命名
A
```

参考 INSTALL 安装

如果缺少libssl
- centos 就 `yum install openssl-devel`

下载百度云的文件时,断开之后重连可能会提示
```
Doing search...  
File not found
```
用 **chrome 的 IDM & 百度云大文件破解** 插件重新获取链接，再下载一次，axel开始续传


# 参考
Shell 输入/输出重定向  
<http://www.runoob.com/linux/linux-shell-io-redirections.html>  
linux screen 命令详解  
<https://www.cnblogs.com/mchina/archive/2013/01/30/2880680.html>  
<https://github.com/axel-download-accelerator/axel>  
<http://wowubuntu.com/axel.html>  
<https://www.youyong.top/article/115948fc283e6>  
<https://axel.alioth.debian.org/>