
---
1.执行epoll_create时，创建了红黑树和就绪list链表。

2.执行epoll_ctl时，如果增加fd（socket），则检查在红黑树中是否存在，存在立即返回，不存在则添加到红黑树上，然后向内核注册回调函数，用于当中断事件来临时向准备就绪list链表中插入数据。

3.执行epoll_wait时立刻返回准备就绪链表里的数据即可。

# reference
epoll源码实现分析[整理]  
<https://www.cnblogs.com/apprentice89/p/3234677.html>