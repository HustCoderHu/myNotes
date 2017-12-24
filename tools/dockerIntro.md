docker 入门
---
# 安装docker
## 安装
先卸载旧版本的docker
```
sudo apt remove docker docker-engine docker.io
```
有三种安装方式
- Docker’s repositories
- DEB package
- automated convenience scripts
这里我直接选 deb，到下面地址下载deb包  
<https://download.docker.com/linux/ubuntu/dists/>  
Ubuntu version -> pool/stable/ -> amd64 选好的下载，并安装
```
sudo dpkg -i docker.deb
```
## 镜像源等一些设置
如果 `sudo groupadd docker` 提示已经有这个组，说明已经被创建了  
默认会创建一个名为docker的 group 键入`cat /etc/group | grep docker` 可以看到，  
现在我们把当前用户加入到 docker 组，免去每次输入sudo的麻烦
```
sudo usermod -aG docker $USER
```
logout 然后 login 使之生效

如果 `docker run hello-world` 提示下载超时，就说明官方的地址被墙了，  
编辑 /etc/docker/daemon.json，加入
```
{
  "registry-mirrors": [
    "https://registry.docker-cn.com"
  ],
  "dns": [
    "114.114.114.114", "8.8.8.8"
  ]
}
```
上面的配置完成两个功能
- 1) 设置从国内的镜像下载
- 2) 配置全部容器的 DNS
然后重启 docker daemon
```
sudo systemctl daemon-reload
sudo systemctl restart docker
# 或者
/etc/init.d/docker restart
```
重试helloword 应该就会看到提示信息

# 常用命令
## 1 ps
```
docker ps [-a] # 查看容器 -a会把已经停止的显示出来
```
## 2 run
```
docker run -it --rm \
    ubuntu:16.04 \
    bash
```
- -it：这是两个参数，一个是 -i：交互式操作，一个是 -t 终端。我们这里打算进入 bash 执行一些命令并查看返回结果，因此我们需要交互式终端。
- --rm：这个参数是说容器退出后随之将其删除。默认情况下，为了排障需求，退出的容器并不会立即删除，除非手动 docker rm。我们这里只是随便执行个命令，看看结果，不需要排障和保留结果，因此使用 --rm 可以避免浪费空间。
- ubuntu:16.04：这是指用 ubuntu:16.04 镜像为基础来启动容器。
- bash：放在镜像名后的是命令，这里我们希望有个交互式 Shell，因此用的是 bash
- -d ，这里没有用，表示后台运行 输出结果可以用 `docker logs [OPTIONS] CONTAINER` 看
## 3 stop
```
docker stop ID(前面几位就ok了)  # 停止运行中的容器
docker start ID  # 可以启动那些已经停止的
```
## 4 删除
```
docker rm ID  # 删除容器
docker container prune  # 清理掉所有处于终止状态的容器
```

## 5 exec
```
docker exec -it ID bash  # 进入运行中的容器，即使不是后台运行的
```

## 6 导入导出
### 不要镜像历史
```
docker export ID > TAR
cat TAR | docker import - REPOSITORY:TAG # 从导出的机器里面抄过来就ok
```
### 需要镜像历史
```
docker save ID > TAR
docker load < TAR
docker tag ID REPOSITORY:TAG  # 标记一下
```


# 常见问题
## 1 时间差8h
在中国 时间是 CST  China Shanghai Time，东八区时间  
image来自国外，时间是 UTC  Coordinated Universal Time，标准时间  
**解决方法**  
启动时加入以下参数
```
--mount type=bind,source=/etc/localtime,target=/etc/localtime,readonly
```



## 2 分布式无法通信
**解决方法**  
关闭主机防火墙  
# 参考
Docker — 从入门到实践  
<https://yeasy.gitbooks.io/docker_practice/content/>  
docker 中国  
<https://www.docker-cn.com/>  
获取适用于 Ubuntu 的 Docker CE  
<https://docs.docker-cn.com/engine/installation/linux/docker-ce/ubuntu>  
Get Docker CE for Ubuntu  
<https://docs.docker.com/engine/installation/linux/docker-ce/ubuntu/>  