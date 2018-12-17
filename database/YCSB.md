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

### make rocksdbjavastatic

如果 出现了下面的错误
```
make[1]: Leaving directory `/home/kv-pmem/xiaohu/nvm_hzx/rocksdb-5.17.2/zstd-1.3.3/lib'
cp zstd-1.3.3/lib/libzstd.a .
  0     0    0     0    0     0      0      0 --:--:--  0:02:09 --:--:--     0curl: (7) Failed connect to web.archive.org:443; Connection timed out
make: *** [libbz2.a] Error 7
```
说明 GFW 屏蔽了下载 bzip2 的网站

**解决办法**

到 source forge 上下载 bzip2-1.0.6.tar.gz  放到当前目录下，并注释掉下载的那些行

最终会在 `java/target` 目录下生成一些文件

### 合并
观察 YCSB rocksdb binding 里面 rocksdbjni-xxx.jar 的大小猜测，发现比上面生成的要大不少，基本是上面 so + jar 大小，猜测需要合并

观察 rocksdb 的 Makefile 里变量 `ROCKSDB_JAR_ALL` 的使用情况也可以看出来，所以

```
jar -tvf rocksdbjni-xxx.jar # 也会显示里面所有的内容
# 看到有接口 class 和 .so

jar -uf TARGET SRC1 SRC2 ... # 合并, .so 也可以
```

### make rocksdbjavastaticrelease

依赖 vagrant，于是先安装

```
curl -O -L https://releases.hashicorp.com/vagrant/2.2.2/vagrant_2.2.2_x86_64.rpm
# 下载太慢的话在win上下载好传过去
yum install vagrant_2.2.2_x86_64.rpm

make rocksdbjavastaticrelease # 还需要虚拟机 VirtualBox 之类的
```
## cmake

```
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DWITH_SNAPPY=1 -DWITH_JNI=1 -DUSE_RTTI=1 ..
# Release 会增加 -O3
# WITH_JNI 看 CMakeList 可以找到答案
# USE_RTTI 不加就会导致 libpmemobj++ 编译错误 (typeid找不到)

make rocksdbjni-shared
make rocksdbjni_classes
make rocksdbjni_headers
mv librocksdbjni-shared.so librocksdbjni-linux64.so
mkdir unzip_classes
cd unzip_classes
jar -xf ../rocksdbjni_classes.jar
jar -cf rocksdbjni-5.18.0.jar ../../librocksdb.so ../librocksdbjni-linux64.so org/rocksdb/*
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
create_javah( TARGET rocksdbjni_headers
  CLASSES ${NATIVE_JAVA_CLASSES}
  CLASSPATH rocksdbjni_classes ${JAVA_TESTCLASSPATH}
  OUTPUT_DIR ${JNI_OUTPUT_DIR} )


add_jar(target_name
        [SOURCES] source1 [source2 ...] [resource1 ...]
        [INCLUDE_JARS jar1 [jar2 ...]]
        [ENTRY_POINT entry]
        [VERSION version]
        [OUTPUT_NAME name]
        [OUTPUT_DIR dir]
        )
add_jar(rocksdbjni_headers
  INCLUDE_JARS ${NATIVE_JAVA_CLASSES}
  OUTPUT_DIR ${JNI_OUTPUT_DIR})
```

```
make rocksdbjava -jN
```



# YCSB
<https://github.com/brianfrankcooper/YCSB/releases> 页面可以找到 rocksdb 的包，解压之后里面的 README 也有部分说明

查看 `bin` 目录下的 .sh，文件末尾发现了运行方法，

- JAVA_OPTS: 暂时为空
- CLASSPATH: 记录了解压目录/lib 目录下所有的 jar

也就是说把上面打包好的 jar 放到 lib 目录下就行了

`bin/ycsb` 是 python 脚本

main 函数在 `core/src/main/java/com/yahoo/ycsb/Client.java`

rocksdb 的适配层在 `rocksdb/src/main/java/com/yahoo/ycsb/db/rocksdb/RocksDBClient.java`

## run
```
export ROCKSDB_SHAREDLIB_DIR=/home/ycsb-rocksdb-binding-0.15.0/lib # 即使加了这行
# 运行
Exception in thread "Thread-2" java.lang.UnsatisfiedLinkError: /home/ycsb-rocksdb-binding-0.15.0/lib/librocksdbjni-linux64.so: librocksdb.so.5: cannot open shared object file: No such file or directory
```

# reference
YCSB初步介绍  
<https://www.cnblogs.com/foxmailed/archive/2012/02/29/2374595.html>  

<https://github.com/brianfrankcooper/YCSB/tree/master/rocksdb>  
RocksJava Basics  
<https://github.com/facebook/rocksdb/wiki/RocksJava-Basics>  

<https://cmake.org/cmake/help/v3.11/module/UseJava.html>  