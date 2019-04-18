代理 简述

---
# 协议层面

## 传输层
ssr, v2ray, wireguard

## 网络层
openvpn, softether
增加一个虚拟网卡，根据目标 ip 确定出的接口
vpn


运载协议  
- TCP
1. 长连接，容易被关注，延迟高
- UDP
1. 运营商 QOS，限流导致浏览体验差

场景分析  
web代理 tcp  
游戏代理 udp  

# 内网穿透

frp ngrok

# ipv6

# idea
个人主机 <---> 公网机器 <---> 校内机器

思路  
- 内网穿透
> 个人主机通过校内机器就能访问文献数据库


# reference
一分钟实现内网穿透（ngrok服务器搭建）  
<https://blog.csdn.net/zhangguo5/article/details/77848658>  