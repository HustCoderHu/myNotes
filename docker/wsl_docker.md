
---
# win
命令行 `winver` 系统要求 > 18917

```sh
Enable-WindowsOptionalFeature -Online -FeatureName VirtualMachinePlatform
Enable-WindowsOptionalFeature -Online -FeatureName Microsoft-Windows-Subsystem-Linux

## 设置 wsl 版本
```sh
wsl --set-default-version 2
wsl --set-version <Distro> 2
```

# linux 发行版
商店搜索 ubuntu 安装 1804

运行一次完成安装

# docker
添加用户组 及 root 权限
```sh
sudo groupadd docker
sudo usermod -aG docker $USER
```
logout 然后 login 使之生效

## 安装
先配置好 系统的源
```sh
apt-get remove docker docker-engine docker.io containerd runc
apt-get update
apt-get install \
    apt-transport-https \
    ca-certificates \
    curl \
    gnupg-agent \
    software-properties-common
curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo apt-key add -
add-apt-repository \
   "deb [arch=amd64] https://download.docker.com/linux/ubuntu \
   $(lsb_release -cs) \
   stable"
apt-get install docker-ce docker-ce-cli containerd.io
```

配置 aliyun 镜像加速器 <cr.console.aliyun.com/cn-hangzhou/instances/mirrors>
```
/etc/init.d/docker restart
# or
service docker restart
service docker status
```


# reference
如何在 Windows 10 中安装 WSL2 的 Linux 子系统  
<https://blog.walterlv.com/post/how-to-install-wsl2.html>  
  
<https://docs.docker.com/install/linux/docker-ce/ubuntu/>  
Windows 10启用Ubuntu子系统  
<https://www.jianshu.com/p/9f7bd5180c48>  
【WSL+Docker】新手Win10下的WSL Ubuntu18并安装使用Docker  
<https://zhuanlan.zhihu.com/p/61542198>  