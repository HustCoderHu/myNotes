FROM ubuntu:latest
MAINTAINER HustCoderHu <coderhzx@gmail.com>

# dockerfile 目录
COPY sources.list /etc/apt/
# ADD /etc/apt/sources.list /etc/apt/sources.list
# COPY timezone /etc/timezone

WORKDIR /home
RUN \
  rm -rf /etc/apt/sources.list.d/* \
  && echo "==> apt update ..." \
  && apt-get update \
\
  && apt-get install --no-install-recommends -y tzdata \
  && rm -f /etc/localtime && echo "Asia/Shanghai" > /etc/timezone && dpkg-reconfigure -f noninteractive tzdata \
\
  && echo "==> install ninja ..." \
  && apt-get install --no-install-recommends -y  ninja-build g++-4.6.4 cmake \
  && ln -s /usr/bin/g++-4.6.4 /usr/bin/g++ \
  && ln -s /usr/bin/gcc-4.6.4 /usr/bin/cc \
\
  && echo "==> clean ..." \
  && apt-get clean && rm -rf /var/lib/apt/lists/*

# && apt-get autoremove -y tzdata \
# apt-get install --no-install-recommends openssh-server
# sed -i "s/#PermitRootLogin.*/PermitRootLogin yes/g" /etc/ssh/sshd_config

# Go
# tar -C /usr/local -xzf go$VERSION.$OS-$ARCH.tar.gz
# echo "export GOROOT=/usr/local/go" >> /root/.profile
# echo "export GOPATH=$GOROOT/bin" >> /root/.profile
# echo "export PATH=$GOPATH:$PATH" >> /root/.profile
# https://golang.org/doc/install#install