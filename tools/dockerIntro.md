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
    "114.114.114.114", "223.5.5.5",  "223.6.6.6"
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
## 1 pull
``` sh
docker pull IMAGE
```
上面没法直接指定源，可以用run替代
``` sh
docker run IMAGE --registry-mirror=https://docker.mirrors.ustc.edu.cn
# https://registry.docker-cn.com
```
registry指定镜像源

## 2 images
```
docker images # 查看已有镜像
```

## 3 ps
```
docker ps [-a] # 查看容器 -a会把已经停止的显示出来
```
## 4 run
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

## 5 stop
```
docker stop ID(前面几位就ok了)  # 停止运行中的容器
docker start ID  # 可以启动那些已经停止的
```

## 6 start
```
docker start [OPTIONS] CONTAINER [CONTAINER...]
Options:
  -a, --attach               Attach STDOUT/STDERR and forward signals
      --detach-keys string   Override the key sequence for detaching a container
      --help                 Print usage
  -i, --interactive          Attach container's STDIN
```
- i 和run的效果类似，直接开始输入，否则就启动到后台了，和`run -d`的效果一样，  
后续的执行内容就可以用`exec 容器ID COMMAND`一次执行一点，用完之后 `stop`

## 7 删除
```
docker rm ID  # 删除容器
docker container prune  # 清理掉所有处于终止状态的容器
```

## 8 exec
```
docker exec [OPTIONS] CONTAINER COMMAND [ARG...]
Options:
  -d, --detach               Detached mode: run command in the background
      --detach-keys string   Override the key sequence for detaching a container
  -e, --env list             Set environment variables
      --help                 Print usage
  -i, --interactive          Keep STDIN open even if not attached
      --privileged           Give extended privileges to the command
  -t, --tty                  Allocate a pseudo-TTY
  -u, --user string          Username or UID (format: <name|uid>[:<group|gid>])
工作路径可以执行一次pwd看到

docker exec -it ID bash  # 进入运行中的容器，即使不是后台运行的
```

## 9 导入导出
分两种
### 1) 不要镜像历史
其实也可以导出镜像
```
docker export [OPTIONS] CONTAINER
Export a container's filesystem as a tar archive
Options:
      --help            Print usage
  -o, --output string   Write to a file, instead of STDOUT

docker export ID > TAR
# 或者
docker export -o FILE CONTAINER

cat TAR | docker import - REPOSITORY:TAG # 从导出的机器里面抄过来就ok
```

### 2) 需要镜像历史
```
# --- 导出
docker save [OPTIONS] IMAGE [IMAGE...]
Save one or more images to a tar archive (streamed to STDOUT by default)
Options:
      --help            Print usage
  -o, --output string   Write to a file, instead of STDOUT
# --- 载入
docker load [OPTIONS]
Load an image from a tar archive or STDIN
Options:
      --help           Print usage
  -i, --input string   Read from tar archive file, instead of STDIN
  -q, --quiet          Suppress the load output

docker save ID > TAR
docker save ID | xz -zc > name.tar.xz # 有的镜像可能很大，压缩一下
docker load < TAR # xz 也可以
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
Docker 镜像源的修改方式  
<https://airycanon.me/docker-jing-xiang-yuan-de-xiu-gai-fang-shi/>  
Docker 中国官方镜像加速  
<https://www.docker-cn.com/registry-mirror>  
Docker Hub 源使用帮助  
<http://mirrors.ustc.edu.cn/help/dockerhub.html>  