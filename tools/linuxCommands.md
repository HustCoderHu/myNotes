linux 上鲜为人知的那些命令
---

# 多任务
很多时候我们只有一个终端界面，而执行的任务耗时又很长
## 重定向
诸如 `cmd > log.txt 2>&1` 表示将stderr和stdout合并重定向到log
- 标准输入文件(stdin)：stdin的文件描述符为0，Unix程序默认从stdin读取数据
- 标准输出文件(stdout)：stdout 的文件描述符为1，Unix程序默认向stdout输出数据
- 标准错误文件(stderr)：stderr的文件描述符为2，Unix程序会向stderr流中写入错误信息

`>>`则是将新内容添加在文件末尾  

这种方式执行时，终端上不会显示out和err的输出。如果要同时输出到屏幕和文件就用`tee`
```
cmd 2>&1 | tee log.txt
# 如果不加 2>&1 就只有 stdout 到文件
```

上述两种方式遇到 ctrl + c 还是会终止命令，最完善的方案是  
多终端  
<https://github.com/HustCoderHu/myNotes/blob/master/tools/linux%E5%A4%9A%E7%BB%88%E7%AB%AF.md>



# 参考
Shell 输入/输出重定向  
<http://www.runoob.com/linux/linux-shell-io-redirections.html>  