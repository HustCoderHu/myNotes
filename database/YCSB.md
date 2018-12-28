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

# 2 novelsm-jni
安装 pmdk 之后

编辑 `include/leveldb/slice.h`，注释成员变量前面的 `private`

编辑 `build_detect_platform`
``` shell
# 61 行附近
PLATFORM_LIBS="-lnuma -lpmem"

# 64 行附近
# PLATFORM_SHARED_CFLAGS=

# 193 行附近，去掉下面的注释
COMMON_FLAGS="$COMMON_FLAGS -D_ENABLE_PMEMIO"
```

```
cd hoard
./compile_install_hoard.sh
cd ..
make -j8
```
out-* 目录下有各种库

## leveldbjni
```
mvn clean install -e -X -P linux64
```

### 镜像导致错误
```
[ERROR] [ERROR] Some problems were encountered while processing the POMs:
[ERROR] Unresolveable build extension: Plugin org.apache.maven.wagon:wagon-webdav-jackrabbit:1.0-beta-7 or one of its dependencies could not be resolved: The following artifacts could not be resolved: org.apache.maven.wagon:wagon-webdav-jackrabbit:jar:1.0-beta-7, org.apache.maven.wagon:wagon-http-shared:jar:1.0-beta-7, nekohtml:xercesMinimal:jar:1.9.6.2, nekohtml:nekohtml:jar:1.9.6.2, org.apache.jackrabbit:jackrabbit-webdav:jar:1.5.0, org.apache.jackrabbit:jackrabbit-jcr-commons:jar:1.5.0, org.slf4j:slf4j-api:jar:1.5.3, org.slf4j:slf4j-nop:jar:1.5.3, org.apache.maven.wagon:wagon-provider-api:jar:1.0-beta-7, org.codehaus.plexus:plexus-utils:jar:1.4.2: Could not transfer artifact org.apache.maven.wagon:wagon-webdav-jackrabbit:jar:1.0-beta-7 from/to alimaven (http://maven.aliyun.com/nexus/content/groups/public/): No route to host (Host unreachable) @

# After correcting the problems, you can resume the build with the command
mvn <goals> -rf :leveldbjni-linux64
```
*** 解决 ***  
不要把 maven 镜像改到 ali


### jdk 错误

```
[INFO] configure: JAVA_HOME was set, checking to see if it's a JDK we can use...
[INFO] checking if '/home/kv-pmem/rocksdb/YCSB/jdk1.8.0_181' is a JDK... no
[INFO] configure: javac was on your path, checking to see if it's part of a JDK we can use...
[INFO] checking if '/home/kv-pmem/rocksdb/YCSB/jdk1.8.0_181' is a JDK... no
[INFO] configure: Taking a guess as to where your OS installs the JDK by default...
[INFO] checking if '/usr' is a JDK... no
[INFO] configure: error: JDK not found. Please use the --with-jni-jdk option
[INFO] rc: 1
```

``` shell
leveldbjni-linux64/target/native-build/configure --help
Optional Packages:
  --with-jni-jdk=PATH     Location of the Java Development Kit. Defaults to
                          your JAVA_HOME setting and falls back to where it is
                          typically installed on your OS
```

###　去掉
于是修改 `leveldbjni-linux64/pom.xml` 文件，configureArgs 增加一行
```
<arg>--with-jni-jdk=${env.JAVA_HOME}</arg>
```

继续出错
```
[INFO] configure: error: JDK not found. Invalid --with-jni-jdk PATH
```
遂分析文件 `leveldbjni/src/main/native-package/m4/jni.m4` 其中函数 `CHECK_JNI_JDK`

### 
`leveldbjni-linux64/target/native-build/src/leveldbjni.cpp`
`leveldbjni-linux64/target/native-build-extracted/leveldbjni-99-master-SNAPSHOT-native-src/src/leveldbjni.cpp`

### configure: error: cannot find headers for leveldb

直接在 repo 里搜索这几个字符串，定位到 `leveldbjni/src/main/native-package/m4/custom.m4`
配置环境 LEVELDB_HOME 指向 lsm_nvm 目录:

### DestroyDB() 参数个数不匹配
观察到 make 参数
``` 
[INFO] executing: /bin/sh -c make install
```
DestroyDB 函数定义有三个参数
```
src/leveldbjni.cpp:223:75: error: invalid initialization of reference of type 'const string& {aka const std::basic_string<char>&}' from expression of type 'leveldb::Options'
[INFO]   {  leveldb::Status temp = leveldb::DestroyDB((const char*)lparg0, *lparg1);
```
使用位置在 `db_bench.cc` 546 行附近

### libtool -L 错误
```
[INFO] /bin/sh ./libtool  --tag=CXX   --mode=link g++  -g -O2 -I/home/kv-pmem/xiaohu/lsm_nvm//include -I/home/kv-pmem/rocksdb/YCSB/jdk1.8.0_181/include -I/home/kv-pmem/rocksdb/YCSB/jdk1.8.0_181/include/linux  -lleveldb -L/home/kv-pmem/xiaohu/lsm_nvm/ -lsnappy -L  -release 99-master-SNAPSHOT -o libleveldbjni.la -rpath /home/kv-pmem/xiaohu/leveldbjni-master/leveldbjni-linux64/target/native-build/target/lib src/leveldbjni.lo src/leveldbjni_stats.lo src/leveldbjni_structs.lo src/buffer.lo src/hawtjni.lo  
[INFO] libtool: link: require no space between `-L' and `-release'
```
在 leveldbjni 仓库里搜索 `lsnappy` 找到了 `leveldbjni/src/main/native-package/m4/custom.m4` 56 行附近
```
# 去掉这里的 -L${withval}
LDFLAGS="$LDFLAGS -lsnappy -L${withval}"
```

### aclocal-1.14: command not found
```
[INFO] executing: /bin/sh -c make install
[INFO] CDPATH="${ZSH_VERSION+.}:" && cd . && /bin/sh /home/kv-pmem/xiaohu/leveldbjni-master/leveldbjni-linux64/target/native-build/autotools/missing aclocal-1.14 -I m4
[INFO] /home/kv-pmem/xiaohu/leveldbjni-master/leveldbjni-linux64/target/native-build/autotools/missing: line 81: aclocal-1.14: command not found
[INFO] WARNING: 'aclocal-1.14' is missing on your system.
[INFO]          You should only need it if you modified 'acinclude.m4' or
[INFO]          'configure.ac' or m4 files included by 'configure.ac'.
[INFO]          The 'aclocal' program is part of the GNU Automake package:
[INFO]          <http://www.gnu.org/software/automake>
[INFO]          It also requires GNU Autoconf, GNU m4 and Perl in order to run:
[INFO]          <http://www.gnu.org/software/autoconf>
[INFO]          <http://www.gnu.org/software/m4/>
[INFO]          <http://www.perl.org/>
[INFO] make: *** [aclocal.m4] Error 127
```
因为上面一个问题修改了 m4 文件，所以这里会发生 aclocal 的调用
leveldbjni README 里提到了 `GNU compiler toolchain`  

```
[INFO] executing: /bin/sh -c make install
[INFO]  cd . && /bin/sh /home/kv-pmem/xiaohu/leveldbjni-master/leveldbjni-linux64/target/native-build/autotools/missing automake-1.14 --foreign --ignore-deps
[INFO] /home/kv-pmem/xiaohu/leveldbjni-master/leveldbjni-linux64/target/native-build/autotools/missing: line 81: automake-1.14: command not found
[INFO] WARNING: 'automake-1.14' is missing on your system.
[INFO]          You should only need it if you modified 'Makefile.am' or
[INFO]          'configure.ac' or m4 files included by 'configure.ac'.
[INFO]          The 'automake' program is part of the GNU Automake package:
[INFO]          <http://www.gnu.org/software/automake>
[INFO]          It also requires GNU Autoconf, GNU m4 and Perl in order to run:
[INFO]          <http://www.gnu.org/software/autoconf>
[INFO]          <http://www.gnu.org/software/m4/>
[INFO]          <http://www.perl.org/>
[INFO] make: *** [Makefile.in] Error 1
[INFO] rc: 2
```

35 行 am___api_version = ''
```
cp leveldbjni/src/main/native-package/aclocal.m4 leveldbjni-linux64/target/native-build/aclocal.m4
alias automake='automake-1.14'
```

### install 失败之后
```
mvn clean
rm -rf ~/.m2/repository/org/fusesource/leveldbjni/
rm -rf leveldbjni/target
rm -rf leveldbjni-linux64/target/native-build-extracted

# 编辑 leveldbjni-linux64/target/native-build/src/leveldbjni.cpp

cp leveldbjni-linux64/target/native-build/src/leveldbjni.cpp leveldbjni-linux64/target/native-build-extracted/leveldbjni-99-master-SNAPSHOT-native-src/src/leveldbjni.cpp
cp leveldbjni-linux64/target/native-build/src/leveldbjni.cpp leveldbjni/target/generated-sources/hawtjni/native-src/leveldbjni.cpp
cp leveldbjni-linux64/target/native-build/src/leveldbjni.cpp leveldbjni/target/generated-sources/hawtjni/native-package/src/leveldbjni.cpp

# 编辑 leveldbjni/src/main/native-package/configure 15654 行附近 去掉 -${am__api_version}
cp leveldbjni/src/main/native-package/configure leveldbjni-linux64/target/native-build/configure

mvn install -P linux64 -D skipTests
```

aclocal-1.14: command not found  
<https://github.com/google/autofdo/issues/38>  

leveldbjni.cpp 中的 ResumeCompactions() SuspendCompactions() 还是会被恢复，发生错误 未定义的函数

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