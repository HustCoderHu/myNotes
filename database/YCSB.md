Cloud Serving Benchmark

---


# 1 RocksJava

三层
- java 类 ，用户使用
- JNI
- rocksdb 本身

用户需要形如 `rocksdbjni-X.X.X.jar`

也就是说需要编译 rocksdb 生成这个 jar

## 1.1 make directly

### make rocksdbjava -jn
配置 `DEBUG_LEVEL` 变量为 0，即为 release，最终会在 `java/target` 目录下生成一些文件，so文件大小还是过大，有 100MB，怀疑还是有符号信息，  
或者是链接了多余的函数和类。  
生成的 jni.jar 可以正常通过 ycsb 的 load 和 run
不过 Makefile 添加依赖比较麻烦，最终考虑还是用 cmake

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
暂时放弃

## 1.2 cmake
CmakeLists.txt 47 到 52 行那些选项的 OFF 可能是导致最终生成的 so 比较小的原因
```
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DWITH_SNAPPY=1 -DWITH_JNI=1 -DUSE_RTTI=1 ..
# Release 会增加 -O3
# WITH_JNI 看 CMakeList 可以找到答案
# USE_RTTI 不加就会导致 libpmemobj++ 编译错误 (typeid找不到)

make rocksdbjni-shared -j12
make rocksdbjni_classes
make rocksdbjni_headers

mkdir unzip_classes
cd unzip_classes
cp ../java/librocksdbjni-shared.so librocksdbjni-linux64.so
jar -xf ../java/rocksdbjni_classes.jar
jar -cf rocksdbjni-5.18.0.jar ../librocksdb.so librocksdbjni-linux64.so org/rocksdb/*
```

将 `rocksdbjni-5.18.0.jar` 放到 `ycsb/lib` 目录下
```
./bin/ycsb load rocksdb -s -P workloads/workloada -p rocksdb.dir=/tmp/ycsb-rocksdb-data
./bin/ycsb run rocksdb -s -P workloads/workloada -p rocksdb.dir=/tmp/ycsb-rocksdb-data

[OVERALL], RunTime(ms), 335
[OVERALL], Throughput(ops/sec), 2985.0746268656717
[TOTAL_GCS_PS_Scavenge], Count, 0
[TOTAL_GC_TIME_PS_Scavenge], Time(ms), 0
[TOTAL_GC_TIME_%_PS_Scavenge], Time(%), 0.0
[TOTAL_GCS_PS_MarkSweep], Count, 0
[TOTAL_GC_TIME_PS_MarkSweep], Time(ms), 0
[TOTAL_GC_TIME_%_PS_MarkSweep], Time(%), 0.0
[TOTAL_GCs], Count, 0
[TOTAL_GC_TIME], Time(ms), 0
[TOTAL_GC_TIME_%], Time(%), 0.0
[READ], Operations, 509
[READ], AverageLatency(us), 35.7426326129666
[READ], MinLatency(us), 11
[READ], MaxLatency(us), 436
[READ], 95thPercentileLatency(us), 79
[READ], 99thPercentileLatency(us), 142
[READ], Return=OK, 509
[CLEANUP], Operations, 1
[CLEANUP], AverageLatency(us), 5778.0
[CLEANUP], MinLatency(us), 5776
[CLEANUP], MaxLatency(us), 5779
[CLEANUP], 95thPercentileLatency(us), 5779
[CLEANUP], 99thPercentileLatency(us), 5779
[UPDATE], Operations, 491
[UPDATE], AverageLatency(us), 112.44806517311609
[UPDATE], MinLatency(us), 47
[UPDATE], MaxLatency(us), 2179
[UPDATE], 95thPercentileLatency(us), 230
[UPDATE], 99thPercentileLatency(us), 495
[UPDATE], Return=OK, 491
*** Error in `/home/kv-pmem/rocksdb/YCSB/jdk1.8.0_181/bin/java': double free or corruption (out): 0x00007f36540ade90 ***
======= Backtrace: =========
/lib64/libc.so.6(+0x81499)[0x7f37109ca499]
/tmp/librocksdbjni4432488629981473834.so(_ZNSt13unordered_mapISsN7rocksdb14OptionTypeInfoESt4hashISsESt8equal_toISsESaISt4pairIKSsS1_EEED1Ev+0x86)[0x7f36be511306]
```

全局搜索 `OptionTypeInfo`

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

### 1.2.1 排错
cmake buildtype=debug 方式构建jar, 开启 core dump `ulimit -c unlimited`  
run 出错
```
/tmp/librocksdbjni6222571687757090684.so(_ZN9__gnu_cxx13new_allocatorIPNSt8__detail15_Hash_node_baseEE10deallocateEPS3_m+0x20)[0x7f0f03766778]
/tmp/librocksdbjni6222571687757090684.so(_ZNSt10_HashtableISsSt4pairIKSsN7rocksdb14OptionTypeInfoEESaIS4_ENSt8__detail10_Select1stESt8equal_toISsESt4hashISsENS6_18_Mod_range_hashingENS6_20_Default_ranged_hashENS6_20_Prime_rehash_policyENS6_17_Hashtable_traitsILb1ELb0ELb1EEEE21_M_deallocate_bucketsEPPNS6_15_Hash_node_baseEm+0x4a)[0x7f0f037ee1c4]
/tmp/librocksdbjni6222571687757090684.so(_ZNSt10_HashtableISsSt4pairIKSsN7rocksdb14OptionTypeInfoEESaIS4_ENSt8__detail10_Select1stESt8equal_toISsESt4hashISsENS6_18_Mod_range_hashingENS6_20_Default_ranged_hashENS6_20_Prime_rehash_policyENS6_17_Hashtable_traitsILb1ELb0ELb1EEEED1Ev+0x36)[0x7f0f037e6b9e]
/tmp/librocksdbjni6222571687757090684.so(_ZNSt13unordered_mapISsN7rocksdb14OptionTypeInfoESt4hashISsESt8equal_toISsESaISt4pairIKSsS1_EEED1Ev+0x18)[0x7f0f037e22fc]
```



总是提示 `Error attaching to core file: cannot open binary file` 参考以下链接怀疑是 java 和 jstack 版本不匹配  
<https://blog.csdn.net/liyf155/article/details/65628209>  

`which java` `which jstack` 发现两者位置不在一个地方

通过以下办法避免版本不同
```
gdb java core.pid
$JAVA_HOME/bin/jstack $JAVA_HOME/bin/java core.30110
```
得到输出
```
Attaching to core core.30110 from executable /home/kv-pmem/rocksdb/YCSB/jdk1.8.0_181/bin/java, please wait...
Debugger attached successfully.
Server compiler detected.
JVM version is 25.181-b13
Deadlock Detection:

No deadlocks found.

Thread 30128: (state = BLOCKED)
 - java.lang.System.arraycopy(java.lang.Object, int, java.lang.Object, int, int) @bci=0 (Compiled frame; information may be imprecise)
 - java.lang.ThreadGroup.remove(java.lang.Thread) @bci=59, line=969 (Interpreted frame)
 - java.lang.ThreadGroup.threadTerminated(java.lang.Thread) @bci=6, line=942 (Interpreted frame)
 - java.lang.Thread.exit() @bci=12, line=758 (Interpreted frame)


Thread 30127: (state = BLOCKED)
 - java.lang.Object.wait(long) @bci=0 (Interpreted frame)
 - java.lang.ref.ReferenceQueue.remove(long) @bci=59, line=144 (Interpreted frame)
 - java.lang.ref.ReferenceQueue.remove() @bci=2, line=165 (Interpreted frame)
 - java.lang.ref.Finalizer$FinalizerThread.run() @bci=36, line=216 (Interpreted frame)


Thread 30126: (state = BLOCKED)
 - java.lang.Object.wait(long) @bci=0 (Interpreted frame)
 - java.lang.Object.wait() @bci=2, line=502 (Interpreted frame)
 - java.lang.ref.Reference.tryHandlePending(boolean) @bci=54, line=191 (Interpreted frame)
 - java.lang.ref.Reference$ReferenceHandler.run() @bci=1, line=153 (Interpreted frame)


Thread 30111: (state = BLOCKED)
 - java.lang.Shutdown.halt0(int) @bci=0 (Interpreted frame)
 - java.lang.Shutdown.halt(int) @bci=7, line=139 (Interpreted frame)
 - java.lang.Shutdown.exit(int) @bci=99, line=213 (Interpreted frame)
 - java.lang.Runtime.exit(int) @bci=14, line=109 (Interpreted frame)
 - java.lang.System.exit(int) @bci=4, line=971 (Interpreted frame)
 - com.yahoo.ycsb.Client.main(java.lang.String[]) @bci=920, line=848 (Interpreted frame)
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