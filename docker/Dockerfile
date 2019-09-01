FROM ubuntu:latest
MAINTAINER HustCoderHu <coderhzx@gmail.com>

# dockerfile 目录
COPY sources.list /etc/apt/
# ADD /etc/apt/sources.list /etc/apt/sources.list
# COPY timezone /etc/timezone

WORKDIR /home
RUN \
  echo "Asia/Shanghai" > /etc/timezone \
  && rm -rf /etc/apt/sources.list.d/* \
  && echo "==> apt update ..." \
  && apt-get update \
\
  && apt-get install --no-install-recommends -y tzdata \
  && dpkg-reconfigure -f noninteractive tzdata \
\
  && echo "==> install ninja ..." \
  && apt-get install --no-install-recommends -y \
  ninja-build g++-5 cmake \
  && ln -s /usr/bin/g++-5 /usr/bin/g++ \
  && ln -s /usr/bin/gcc-5 /usr/bin/cc \
\
  && echo "==> clean ..." \
  && apt-get autoremove -y tzdata \
  && apt-get clean \
  && rm -rf /var/lib/apt/lists/*

# sed -i "s/#PermitRootLogin.*/PermitRootLogin yes/g" /etc/ssh/sshd_config