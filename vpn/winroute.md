cmd 命令 `route add/delete` 效率非常低，下文描述通过winapi操作

---
# functions
函数都在 `iphlpapi.h` 里，第1个参考链接里搜索 `forward` 可以找到四个函数
- `CreateIpForwardEntry`
- `DeleteIpForwardEntry`
- `SetIpForwardEntry`
- `GetIpForwardTable` 

前三个是我们会用到的，对应添加, 删除, 修改

openvpn 的 `src/openvpn/route.c` 可以找到create和delete两个函数的使用  

python3 使用例子已入库  
<https://github.com/HustCoderHu/WinRoute>

# reference
iphlpapi.h header  
<https://docs.microsoft.com/en-us/windows/desktop/api/iphlpapi/>  

<https://github.com/OpenVPN/openvpn/blob/master/src/openvpn/route.c>  
C#操作静态路由表(增、删、改、查、遍历)  
<http://blog.chdz1.com/NETjs/254.html>  
C# 通过Iphlpapi.dll API操作路由表  
<https://blog.csdn.net/skyboyyu/article/details/8029047>  

<https://github.com/Shinlor/WinRoute>  
[网络通信] dllcall 调用CreateIpForwardEntry 修改路由应该怎么弄？[已解决]感谢P版  
<http://www.autoitx.com/forum.php?mod=viewthread&tid=26092&extra=&page=1>  