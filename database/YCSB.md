Cloud Serving Benchmark

---


# RocksJava

三层
- java 类 ，用户使用
- JNI
- rocksdb 本身

用户需要形如 `rocksdbjni-X.X.X.jar`

也就是说需要编译 rocksdb 生成这个 jar

## make directly
下面的 make 目标依赖 vagrant，于是先安装

```
curl -O -L https://releases.hashicorp.com/vagrant/2.2.2/vagrant_2.2.2_x86_64.rpm
# 下载太慢的话在win上下载好传过去
yum install vagrant_2.2.2_x86_64.rpm
```

```
make rocksdbjavastaticrelease # 还需要虚拟机 VirtualBox 之类的
make rocksdbjavastatic
```

因为 GFW 会出错

```
make[1]: Leaving directory `/home/kv-pmem/xiaohu/nvm_hzx/rocksdb-5.17.2/zstd-1.3.3/lib'
cp zstd-1.3.3/lib/libzstd.a .
  0     0    0     0    0     0      0      0 --:--:--  0:02:09 --:--:--     0curl: (7) Failed connect to web.archive.org:443; Connection timed out
make: *** [libbz2.a] Error 7
```
查看 Makefile 发现 因为下载 bzip2 时出错

到 source forge 上下载 bzip2-1.0.6.tar.gz  放到当前目录下，并注释掉下载的那些行

## cmake

```
cmake -DWITH_SNAPPY=yes -DWITH_JNI=yes ..
```

```
CMake Deprecation Warning at /usr/local/share/cmake-3.13/Modules/UseJava.cmake:1318 (message):
  create_javah: this command will no longer be supported starting with
  version 10 of JDK.  Update your project by using command
  add_jar(GENERATE_NATIVE_HEADERS) instead.
Call Stack (most recent call first):
  java/CMakeLists.txt:373 (create_javah)
```

```
make rocksdbjava -jN
```



# reference
YCSB初步介绍  
<https://www.cnblogs.com/foxmailed/archive/2012/02/29/2374595.html>  

<https://github.com/brianfrankcooper/YCSB/tree/master/rocksdb>  
RocksJava Basics  
<https://github.com/facebook/rocksdb/wiki/RocksJava-Basics>  