
---
# overview
示意图
客户端 -> 服务器

client <--> openvpn <--> udp2raw <--> === === === <--> udp2raw <--> openvpn <--> server

# 安装
<https://github.com/wangyu-/udp2raw-tunnel/releases>  


# 启动
`--log-level ${log level}` 可以打印调试信息
## server
```
openvpn 监听 7777
./udp2raw_amd64 -s -l0.0.0.0:4096 -r 127.0.0.1:7777  -a -k "passwd" --raw-mode faketcp --cipher-mode none --auth-mode none --enable-color
# --cipher-mode xor --auth-mode simple

```

## client
```
先启动 udp2raw

udp2raw_mp_nolibnet -c -l0.0.0.0:3333  -r${SERVER_IP}:4096 -k "passwd" --raw-mode easy-faketcp --cipher-mode none --auth-mode none --enable-color
# faketcp 
# udp2raw_mp_nolibnet -c -l0.0.0.0:3333  -rSERVER_IP:4096 -k "passwd" --raw-mode faketcp -g

openvpn 发往 127.0.0.1 3333
```

# 注意
openvpn 有一个 interactive service 这个服务要开启，否则openvpn无法接收服务端的路由推送  
官方说明如下  
<https://community.openvpn.net/openvpn/wiki/OpenVPNInteractiveService>

# reference
路由跟踪 best trace
快速入门  
<https://github.com/wangyu-/udp2raw-multiplatform/wiki/%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8>  