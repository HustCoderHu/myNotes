


# ubuntu server 1604 nv 显卡消失
```
~$ nvidia-smi
NVIDIA-SMI has failed because it couldn't communicate with the NVIDIA driver. Make sure that the latest NVIDIA driver is installed and running.
```
猜测是系统自动更新导致驱动不匹配，禁止自动更新方法如下
<https://askubuntu.com/questions/172524/how-can-i-check-if-automatic-updates-are-enabled>  
关键文件
```
/etc/apt/apt.conf.d/10periodic
/etc/apt/apt.conf.d/20auto-upgrades
```