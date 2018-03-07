从 9.0-cudnn7-runtime-ubuntu16.04 构建一个 py3.5 的pytorch环境

---

Dockerfile
```
FROM nvidia/cuda:9.0-cudnn7-runtime-ubuntu16.04
MAINTAINER HustCoderHu <coderhzx@gmail.com>

RUN rm /etc/apt/sources.list.d/*
# RUN rm /etc/apt/sources.list.d/cuda.list

COPY sources.list /etc/apt/ # 源自己看情况选
# 教育网用清华，其他网就用网易或者阿里
# COPY tuna-srclist /etc/apt/sources.list
# ADD /etc/apt/sources.list /etc/apt/


RUN toinstall="python3.5 python3-pip" \
  && apt update \
  && apt install -y $toinstall

WORKDIR /home
COPY torch-0.3.1-cp35-cp35m-linux_x86_64.whl ./
RUN toinstall="torch-0.3.1-cp35-cp35m-linux_x86_64.whl" \
  && pip3 install --upgrade pip \
  && pip3 install -i https://pypi.tuna.tsinghua.edu.cn/simple $toinstall \
  && pip3 install -i https://pypi.tuna.tsinghua.edu.cn/simple torchvision \
  && rm -f $toinstall \
  && apt clean \
  && apt autoremove -y
```

出错
```
Installing collected packages: pyyaml, numpy, cu90-torch
Exception:
Traceback (most recent call last):
  File "/usr/local/lib/python3.5/dist-packages/pip/basecommand.py", line 215, in main
    status = self.run(options, args)
  File "/usr/local/lib/python3.5/dist-packages/pip/commands/install.py", line 342, in run
    prefix=options.prefix_path,
  File "/usr/local/lib/python3.5/dist-packages/pip/req/req_set.py", line 784, in install
    **kwargs
  File "/usr/local/lib/python3.5/dist-packages/pip/req/req_install.py", line 851, in install
    self.move_wheel_files(self.source_dir, root=root, prefix=prefix)
  File "/usr/local/lib/python3.5/dist-packages/pip/req/req_install.py", line 1064, in move_wheel_files
    isolated=self.isolated,
  File "/usr/local/lib/python3.5/dist-packages/pip/wheel.py", line 347, in move_wheel_files
    assert info_dir, "%s .dist-info directory not found" % req
AssertionError: cu90-torch==0.3.1 .dist-info directory not found
The command '/bin/sh -c toinstall="cu90-torch-0.3.1-cp35-cp35m-linux_x86_64.whl"   && pip3 install --upgrade pip   && pip3 install -i https://pypi.tuna.tsinghua.edu.cn/simple $toinstall   && pip3 install -i https://pypi.tuna.tsinghua.edu.cn/simple torchvision   && rm -f $toinstall   && apt clean   && apt autoremove -y' returned a non-zero code: 2
```
错误原因我猜是 whl的名字被我加了 `cu90-`，因为去掉之后就成功了

build 成功之后 docker images看到的是 `<none>`，下面标记一下
```
docker tag ID REPO:TAG
```

# 测试
```
docker run -it --runtime=nvidia --rm \
--mount type=bind,source=/etc/localtime,target=/etc/localtime,readonly \
--mount type=bind,source=/home,target=/home \
REPO:TAG bash
```


# reference
docker_practice  
<https://yeasy.gitbooks.io/docker_practice/content/image/build.html>  
Docker:修改Dockerfile替换软件源加快构建过程  
<https://www.jianshu.com/p/87ef2f6529d4>
怎样构建一个好的 Docker 镜像  
<https://segmentfault.com/a/1190000000729982>  
nvidia/cuda  
<https://hub.docker.com/r/nvidia/cuda/>  
CUDA / Docker & GPG error  
<https://github.com/NVIDIA/nvidia-docker/issues/619>  