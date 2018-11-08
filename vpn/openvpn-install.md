

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
### IP 转发
```
# 编辑 /etc/sysctl.conf 其中 ipv4 forward 置为 1
echo 1 > /proc/sys/net/ipv4/ip_forward

# nat
每条 chain 全部接收
iptables -P INPUT ACCEPT
iptables -P FORWARD ACCEPT
iptables -P OUTPUT ACCEPT
清空其他规则
iptables -F
# 源地址 nat
iptables -t nat -A POSTROUTING -o ${IFNAME} -j MASQUERADE

# 开机自动加载 /etc/iptables/ 下的规则
apt isntall iptables-persistent
# 规则文件由 iptables-save 生成
```

### ovpn
编译过程很麻烦  
参考github 仓库的 `INSTALL` 文件找到ubuntu 和 debian 发行版的官方repo，说明如下
<https://community.openvpn.net/openvpn/wiki/OpenvpnSoftwareRepos>  

Then import the public GPG key that is used to sign the packages:
```
wget -O - https://swupdate.openvpn.net/repos/repo-public.gpg|apt-key add -
```

Next you need to create a sources.list fragment (as root) so that apt can find the new OpenVPN packages. One way to do it is this:
```
echo "deb http://build.openvpn.net/debian/openvpn/<version> <osrelease> main" > /etc/apt/sources.list.d/openvpn-aptrepo.list
```
安装
```
apt update & apt install openvpn
```

服务器端需要下面的文件
```
ca ca.crt
cert server.crt
key server.key
dh dh2048.pem
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
编译 OpenVPN 及解决相关依赖问题  
<https://segmentfault.com/a/1190000009414135>  

<https://www.linuxquestions.org/questions/debian-26/debian-stretch-iptables-and-iptables-persistent-service-start-and-stop-actions-no-longer-supported-4175622528/>  

<https://openvpn.net/community-resources/how-to/#quick>  

加解密设置  
<https://www.igfw.net/archives/13659>  
<https://openvpn.net/community-resources/openvpn-protocol/>  
