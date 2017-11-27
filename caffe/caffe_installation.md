
```
sudo apt install libprotobuf-dev libleveldb-dev libsnappy-dev libopencv-dev libhdf5-serial-dev protobuf-compiler
sudo apt install --no-install-recommends libboost-all-dev
sudo apt install libgflags-dev libgoogle-glog-dev liblmdb-dev 
```

```
echo "/opt/OpenBLAS" > /etc/ld.so.conf.d/openblas.conf
ldconfig
# 准备配置
cp Makefile.config.example Makefile.config
```

after do the two steps @tangshi mentioned, I continued to add the following lines in MakeFile.config  
```
BLAS := open
BLAS_INCLUDE := /opt/OpenBLAS/include
BLAS_LIB := /opt/OpenBLAS/lib

INCLUDE_DIRS := $(PYTHON_INCLUDE) /usr/local/include /usr/include/hdf5/serial/
LIBRARY_DIRS := $(PYTHON_LIB) /usr/local/lib /usr/lib /usr/lib/x86_64-linux-gnu/hdf5/serial/
```


```
make -jn
make test -jn
make runtest -jn
# 非 root 用户 推荐
echo "export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/usr/caffe/build/lib" >> /home/usr/.bashrc
# echo "/root/caffe/build/lib" > /etc/ld.so.conf.d/caffe.conf
# ldconfig
```


# 参考
1 can't find hdf5.h when build caffe  
<https://github.com/NVIDIA/DIGITS/issues/156>