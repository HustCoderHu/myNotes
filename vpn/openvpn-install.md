

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
# 有些机器的公网 ip 在子网卡上，会导致 -o 获取源地址失败，SNAT也就不成功，这时需要自己指定源地址

iptables -t nat -A POSTROUTING -o ${Iface} -j SNAT --to-source ${机器公网IP}
# -o 表示转发的包会从哪个网卡出去，通过 route 看 default 那条的 Iface

# 开机自动加载 /etc/iptables/ 下的规则
apt install iptables-persistent
# 规则文件由 iptables-save 生成
iptables-save > /etc/iptables/rules.v4
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
openvpn --config xxx.ovpn [ > log_ovpn 2 > &1 &] # [] 可以到后台
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
<https://github.com/HustCoderHu/WinRoute>

# multi client

# 测试延迟
tcping 

# 用户名 密码
两种方式

##  非 PAM
bash脚本验证密码
server 配置文件增加
```
username-as-common-name
verify-client-cert none
auth-user-pass-verify /path/to/checkpsw.sh via-env
script-security 3
```
checkpsw.sh 文件里要配置 密码文件 和 日志文件 的路径

## auth-pam.pl
`/usr/share/openvpn/auth-pam.pl`  
```
auth-user-pass-verify /usr/share/openvpn/auth-pam.pl via-file
```
貌似是通过系统帐户认证，没有成功
<https://xu3352.github.io/linux/2017/06/08/openvpn-use-username-and-password-authentication>  
<http://blog.51cto.com/jerry12356/1854949>  
<https://www.jianshu.com/p/4c631afedb6c>  

<http://blog.51niux.com/?id=73>  

<http://www.beijinghuayu.com.cn/openvpn%E7%9A%84%E5%88%9B%E5%BB%BA%E4%B8%8E%E7%AE%A1%E7%90%86/>  

<https://forums.openvpn.net/viewtopic.php?t=12965>  

目录 `/etc/pam.d` 目录下 用 login


# reference
编译 OpenVPN 及解决相关依赖问题  
<https://segmentfault.com/a/1190000009414135>  

<https://www.linuxquestions.org/questions/debian-26/debian-stretch-iptables-and-iptables-persistent-service-start-and-stop-actions-no-longer-supported-4175622528/>  

<https://openvpn.net/community-resources/how-to/#quick>  

加解密设置  
<https://www.igfw.net/archives/13659>  
<https://openvpn.net/community-resources/openvpn-protocol/>  
