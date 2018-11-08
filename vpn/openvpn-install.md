

---

# 安装
## client

<https://openvpn.net/>  下载，安装的时候如果要建服务器，就勾上 easy rsa 那些

生成证书
```
init-config
vars
clean-all
build-ca

# Common Name 要自己设置，其他的直接回车
```
server 组件
```
build-key-server server
输入一次 Common Name
输入两次y
```
client 组件
```
build-key client1
build-key client2
build-key client3
```
DH 参数
```
build-dh
```

## server
```
apt install openvpn

curl -O curl -O https://swupdate.openvpn.org/community/releases/openvpn-2.4.6.tar.xz
tar Jxf openvpn-2.4.6.tar.xz
cd openvpn-2.4.6
./configure --prefix=$(pwd)/install
```

```
apt install libssl-dev
```

服务器端需要下面的文件
```
ca ca.crt
cert server.crt
key server.key
dh dh2048.pem
```
转发以及 nat
```
echo 1 > /proc/sys/net/ipv4/ip_forward
service iptables restart
iptables -P INPUT ACCEPT
iptables -P FORWARD ACCEPT
iptables -P OUTPUT ACCEPT
iptables -F
iptables -t nat -A POSTROUTING -o ${IFNAME} -j MASQUERADE
iptables -t nat -A POSTROUTING -o eth0 -j MASQUERADE
```


参考安装客户端时 sample-config 目录下的 server.ovpn 文件设置服务器启动参数

```
openvpn --config xxx.ovpn
```

客户端如果要用  openvpn-gui ，配置文件里的 crt key 文件要用绝对路径

# 纯 openvpn 测试

```
proto udp

尝试关闭数据加密失败
cipher none
;AES-256-CBC
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