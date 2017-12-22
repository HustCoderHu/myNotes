ubuntu 1604 上通过 docker 运行tensorflow
# 安装 docker
参考 [dockerIntro](tools/dockerIntro.md)

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
docker run -di \
--mount type=bind,source=/etc/localtime,target=/etc/localtime,readonly \
--mount type=bind,source=/etc/apt/sources.list,target=/etc/apt/sources.list,readonly \
--mount type=bind,source=/home/xiaohu/tensorflow,target=/home/tensorflow \
tensorflow/tensorflow bash
```
- 第1个挂载的是主机的时间 readonly 方式，dockerIntro里提到过
- 第2个是主机的源，如果机器没有网就不需要了
- 第3个是源码
因为 -i 和 最后的bash存在，所以这个容器不会直接退出

# 参考
Installing TensorFlow on Ubuntu  
<https://www.tensorflow.org/install/install_linux#InstallingDocker>  