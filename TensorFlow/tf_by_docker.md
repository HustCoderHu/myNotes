ubuntu 1604 上通过 docker 运行tensorflow

# GPU 模式
## 驱动
下载独立的驱动 runfile，直接用cuda8的驱动会出现找不到gpu的问题

## CUDA 8

## CuDNN


# 安装 docker
## docker-ce 
参考 [dockerIntro](tools/dockerIntro.md)

## nv docker
下载4个东西，github nvidia团队里有
- libnvidia-container
- libnvidia-container-tools
- nvidia-container-runtime
- nvidia-docker2*.deb
<https://github.com/NVIDIA/libnvidia-container/tree/gh-pages/ubuntu16.04/amd64>  
<https://github.com/NVIDIA/nvidia-docker/tree/gh-pages/ubuntu16.04/amd64>

# 准备各种东西
## 1 下载 img
```
docker pull tensorflow/tensorflow  # 和 git pull 一样有更新效果
```
## 2 下载源码
```
wget https://codeload.github.com/tensorflow/tensorflow/zip/master
unzip tensorflow-master
mv tensorflow-master tensorflow
```

# 启动容器
```
docker run -ti --runtime=nvidia --rm \
--mount type=bind,source=/etc/localtime,target=/etc/localtime,readonly \
--mount type=bind,source=/etc/apt/sources.list,target=/etc/apt/sources.list,readonly \
--mount type=bind,source=/home/hzx,target=/home/hzx \
tensorflow/tensorflow:latest-gpu bash

docker run -di \
--mount type=bind,source=/etc/localtime,target=/etc/localtime,readonly \
--mount type=bind,source=/etc/apt/sources.list,target=/etc/apt/sources.list,readonly \
--mount type=bind,source=/home/xiaohu,target=/home/xiaohu \
tensorflow/tensorflow bash
```
- 第1个挂载的是主机的时间 readonly 方式，dockerIntro里提到过
- 第2个是主机的源，如果机器没有网就不需要了
- 第3个是源码
因为 -i 和 最后的bash存在，所以这个容器不会直接退出

# 参考
Installing TensorFlow on Ubuntu  
<https://www.tensorflow.org/install/install_linux#InstallingDocker>  