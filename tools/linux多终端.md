
---

# 背景
系统管理员经常需要SSH 或者telent 远程登录到Linux 服务器，经常运行一些需要很长时间才能完成的任务，比如系统备份、ftp 传输等等。通常情况下我们都是为每一个这样的任务开一个远程终端窗口，因为它们执行的时间太长了。必须等待它们执行完毕，在此期间不能关掉窗口或者断开连接，否则这个任务就会被杀掉，一切半途而废了。

解决办法有两个， screen 和 tmux，下面分别介绍
# GNU Screen
GNU Screen是一款由GNU计划开发的用于命令行终端切换的自由软件。用户可以通过该软件同时连接多个本地或远程的命令行会话，并在其间自由切换。  
GNU Screen可以看作是窗口管理器的命令行界面版本。它提供了统一的管理多个会话的界面和相应的功能。  
## 用途
- 会话恢复

只要Screen本身没有终止，在其内部运行的会话都可以恢复。这一点对于远程登录的用户特别有用——即使网络连接中断，用户也不会失去对已经打开的命令行会话的控制。只要再次登录到主机上执行screen -r就可以恢复会话的运行。同样在暂时离开的时候，也可以执行分离命令detach，在保证里面的程序正常运行的情况下让Screen挂起（切换到后台）。这一点和图形界面下的VNC很相似。
- 多窗口

在Screen环境下，所有的会话都独立的运行，并拥有各自的编号、输入、输出和窗口缓存。用户可以通过快捷键在不同的窗口下切换，并可以自由的重定向各个窗口的输入和输出。Screen实现了基本的文本操作，如复制粘贴等；还提供了类似滚动条的功能，可以查看窗口状况的历史记录。窗口还可以被分区和命名，还可以监视后台窗口的活动。
- 会话共享

Screen可以让一个或多个用户从不同终端多次登录一个会话，并共享会话的所有特性（比如可以看到完全相同的输出）。它同时提供了窗口访问权限的机制，可以对窗口进行密码保护。
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

# detach，暂时离开当前session，将目前的 screen session (可能含有多个 windows)
# 丢到后台执行，并会回到还没进 screen 时的状态，此时在 screen session 里，每个
# window 内运行的 process (无论是前台/后台)都在继续执行，即使 logout 也不影响
d

# 创建一个新的运行shell的窗口并切换到该窗口
c

# 显示所有窗口列表
w

# session 重命名
A
```

# tmux
tmux 的快捷键和 screen 的有点类似，只是初始快捷键改为了 Ctrl+B：

Ctrl-B-C ：创建新窗口；  
Ctrl-B-D ：detach窗口，和 screen 的类似；  
Ctrl-B-"  ：将当前窗口拆分为上下两行；  
Ctrl-B-%：将当前窗口拆分为左右两列；  
Ctrl-B-上下左右：切换 shell pane  

# 后记
github上面搜 tmux 可以搜到别人的一些配置，star 竟然还上 k

# 参考
linux screen 命令详解  
<https://www.cnblogs.com/mchina/archive/2013/01/30/2880680.html>  
Tmux : GNU Screen 的替代品  
<http://wowubuntu.com/tmux.html>  
从Tmux切换到GNU Screen  
<https://bamanzi.github.io/2016/07/04/gnu-screen-vs-tmux/>  
刚迷上screen就不得不马上移情tmux  
<https://zohead.com/archives/terminal-screen-and-tmux/>