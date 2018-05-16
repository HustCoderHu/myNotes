优化程序时需要用到

---
一般我们可以通过top、iostat、free、vmstat等命令来查看初步定位问题

- top
- vmstat
- iostat
- iotop
  > 一个用来监视磁盘 I/O 使用状况的 top 类工具, 可以观察进程io

# 1 top
wa 等待输入输出的CPU时间百分比  
 wa 的百分比可以大致的体现出当前的磁盘io请求是否频繁。如果 wa的数量比较大，说明等待输入输出的的io比较多。

# 2 vmstat
```
vmstat 2 5
```
如果发现等待的进程和处在非中断睡眠状态的进程数非常多，并且发送到块设备的块数和从块设备接收到的块数非常大，那就说明磁盘io比较多。

# 3 iostat
```
iostat -d -k 1 10        #查看TPS和吞吐量信息
iostat -d -x -k 1 10      #查看设备使用率（%util）、响应时间（await）
iostat -c 1 10            #查看cpu状态(iowait)
# 1 10: 刷新间隔 1s 显示10次
```

# iotop
可以用左右箭头操作，按 r 是相反方向，按 o 是动态切换

用法 iotop [参数]
```
–version 查看版本信息的 
-h, –help 查看帮助信息的 
-o, –only 只显示在划硬盘的程序
-b, –batch 批量处理 用来记录日志的 
-n NUM  设定循环几次 
-d SEC, –delay=SEC  设定显示时间间隔
```

# reference
linux查看磁盘io的几种方法  
<http://blog.51cto.com/7336056/1890035>  
Linux iostat监测IO状态  
<http://www.orczhou.com/index.php/2010/03/iostat-detail/>  
Linux系统IO分析工具之iotop参数详解（查看IO占用）  
<https://www.cnblogs.com/legendbaby/p/5056967.html>  