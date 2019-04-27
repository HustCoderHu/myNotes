## 2 端口映射
### 2.1 在搭建负载均衡的那台服务器上，设置端口映射规则，使得如果访问它的9999端口，则实际访问到的是psdash主服务器上的9999端口。


打开/etc/sysctl.conf文件，找到#net.ipv4.ip_forward=1这一行，取消其注释（开启转发功能）:
```
# Uncomment the next line to enable packet forwarding for IPv4
net.ipv4.ip_forward=1
```

执行sysctl -p命令，使其立即生效:
```
root@newbie-unknown168764:/home/yuezhu# sysctl -p
net.ipv4.ip_forward = 1
```

设置端口转发：hostA:9999 ---> hostB:9999

```
root@newbie-unknown168764:/home/yuezhu# iptables -t nat -A PREROUTING  -p tcp -d hostA --dport 9999 -j DNAT --to-destination hostB:9999
root@newbie-unknown168764:/home/yuezhu# iptables -t nat -A POSTROUTING -p tcp -d hostB --dport 9999 -j SNAT --to-source hostA
root@newbie-unknown168764:/home/yuezhu# iptables -t nat -nL
```

将规则保存至文件：
```
root@newbie-unknown168764:/home/yuezhu# iptables-save > /etc/iptables.rules
```
