

---

# openvpn
证书
```
init-config
vars
clean-all
build-ca

# Common Name 要自己设置，其他的直接回车
```

server
```
build-key-server server
输入一次 Common Name
输入两次y
```

client
```
build-key client1
build-key client2
build-key client3
```

DH 参数
```
build-dh
```

服务器端保留

```
ca ca.crt
cert server.crt
key server.key
dh dh2048.pem
```

# 纯 openvpn 测试

```
proto udp

尝试关闭数据加密失败
cipher none
;AES-256-CBC
```

server
```
./udp2raw_amd64 -s -l0.0.0.0:4096 -r 127.0.0.1:7777  -a -k "passwd" --raw-mode faketcp
```

client
```
先启动 udp2raw

udp2raw_mp_nolibnet -c -l0.0.0.0:3333  -rSERVER_IP:4096 -k "passwd" --raw-mode easy-faketcp
rem udp2raw_mp_nolibnet -c -l0.0.0.0:3333  -rSERVER_IP:4096 -k "passwd" --raw-mode faketcp -g

再启动 openvpn
```

# 部分代理
获取 ip 段  
<https://github.com/fivesheep/chnroutes>  

```
python chnroutes.py
```
行数 8000+ 系统负担可能很大

ip 段
<https://www.ip2location.com/blockvisitorsbycountry.aspx>

<https://www.cnblogs.com/wpjamer/p/8820174.html>  

路由导入  
<https://github.com/davideuler/chnroutes/issues/60>  

生成中国ip 段
<https://github.com/liudongmiao/chinaip/blob/master/Makefile>  

# multi client

# 测试延迟
tcping 

# reference

<https://openvpn.net/community-resources/how-to/#quick>  

加解密设置  
<https://www.igfw.net/archives/13659>  
<https://openvpn.net/community-resources/openvpn-protocol/>  

<https://github.com/wangyu-/udp2raw-multiplatform/wiki/%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8>  