

---
# overview
示意图
客户端 -> 服务器

client <--> openvpn <--> udp2raw <--> === === === <--> udp2raw <--> openvpn <--> server

# 安装
<https://github.com/wangyu-/udp2raw-tunnel/releases>  


# 启动
server
```
openvpn 监听 7777
./udp2raw_amd64 -s -l0.0.0.0:4096 -r 127.0.0.1:7777  -a -k "passwd" --raw-mode faketcp
```

client
```
先启动 udp2raw

udp2raw_mp_nolibnet -c -l0.0.0.0:3333  -rSERVER_IP:4096 -k "passwd" --raw-mode easy-faketcp
rem udp2raw_mp_nolibnet -c -l0.0.0.0:3333  -rSERVER_IP:4096 -k "passwd" --raw-mode faketcp -g

openvpn 发往 127.0.0.1 3333
```

# reference
<https://github.com/wangyu-/udp2raw-multiplatform/wiki/%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8>  