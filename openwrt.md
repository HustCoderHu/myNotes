路由器

---
# tftp
配置静态 ip 192.168.1.xx (只要不是1)，路由器进入recovery 模式
一般是按住 reset 键 然后上电，电源灯开始闪烁后，松开reset
尝试 `ping 192.168.1.1'，timeout的话就重复一次进recovery。

进入之后，`tftp -h` 可以看到可选参数，一般用下面的参数
```sh
-i 192.168.1.1 PUT $img
```
# NETGEAR wndr4300
wndr4300刷任意系统及刷回官方原厂系统  
<https://www.cnblogs.com/acgpiano/p/7058806.html>  

---
# 1 限速
<https://forum.openwrt.org/t/info-limiting-download-speed-based-on-mac/14092>  
[Info] Limiting Download Speed based on MAC  
<https://forum.openwrt.org/t/info-limiting-download-speed-based-on-mac/14092>  
Bandwidth limit per IP/Mac  
<https://forum.openwrt.org/t/bandwidth-limit-per-ip-mac/35943/2>  
