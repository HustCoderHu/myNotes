
# 2 novelsm-jni
安装 pmdk 之后，使用下面的代码，commits 里可以看到修改内容
<https://github.com/HustCoderHu/lsm_nvm>

```
cd hoard
./compile_install_hoard.sh
cd ..
make -j8
```
out-* 目录下有各种库

## leveldbjni
```
export LEVELDB_HOME=
mvn clean install -P linux64
mvn clean install -e -X -P linux64
```

三类 class 文件
- iq80 leveldb api
- hawtjni
- leveldbjni

pom.xml 文件 54 行附近有写前两类的版本，到 <https://mvnrepository.com/> 上下载

```
export JAVA_HOME=
export LEVELDB_HOME=
mvn clean
rm -rf ~/.m2/repository/org/fusesource/leveldbjni/
rm -rf leveldbjni/target
rm -rf leveldbjni-linux64/target
git clean -f
# YCSB 把 $LEVELDB_HOME 当作链接库的搜索路径
ln -s $LEVELDB_HOME/out-shared/libleveldb.so.1.18 $LEVELDB_HOME/libleveldb.so
mvn install -P linux64 -D skipTests # 不要在 screen 里用

# 打包
mkdir pack-leveldbjni && cd pack-leveldbjni
curl -O http://central.maven.org/maven2/org/fusesource/hawtjni/hawtjni-runtime/1.11/hawtjni-runtime-1.11.jar
curl -O http://central.maven.org/maven2/org/iq80/leveldb/leveldb-api/0.6/leveldb-api-0.6.jar
jar xf ../leveldbjni-linux64/target/leveldbjni-linux64-99-master-SNAPSHOT.jar # so
mv META-INF/native/linux64/libleveldbjni.so .
rm -rf META-INF

jar xf ../leveldbjni/target/leveldbjni-99-master-SNAPSHOT.jar # jni.class
jar xf hawtjni-runtime-1.11.jar
jar xf leveldb-api-0.6.jar
jar cf novelsmjni.jar libleveldbjni.so META-INF org
```

### YCSB
```
cp novelsmjni.jar leveldbjni/
mvn -pl com.yahoo.ycsb:leveldbjni-binding -am clean package
```
会构建出一个 tar.gz 包

## run
```
export LEVELDB_HOME=/home/kv-pmem/xiaohu/lsm_nvm
export LEVELDBJNI_HOME=/home/kv-pmem/xiaohu/leveldbjni
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$LEVELDB_HOME/out-shared:$LEVELDBJNI_HOME/leveldbjni-linux64/target/native-build/target/lib

tar zxf leveldbjni/target/ycsb-leveldbjni-binding-0.16.0-SNAPSHOT.tar.gz
cd ycsb-leveldbjni-binding-0.16.0-SNAPSHOT
# leveldbjni.jar 要放到 lib 目录下
cp novelsmjni.jar ycsb-leveldbjni-binding-0.16.0-SNAPSHOT/lib/

# 67108864 == 64 << 20
export FLAGS_use_existing_db=0
export FLAGS_db_disk=/mnt/ssd/xiaohu/novelsm
export FLAGS_db_mem=/pmem/xiaohu/novelsm
export FLAGS_nvm_buffer_size=67108864
export FLAGS_num_levels=2
export FLAGS_num_read_threads=1

bin/ycsb load leveldbjni -P workloads/workloada -p rocksdb.dir=/mnt/ssd/xiaohu/novelsm
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
不要在 screen 创建的虚拟终端里构建

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
使用位置在 `db_bench.cc` 546 行附近，本人仓库里已经改好  
<https://github.com/HustCoderHu/lsm_nvm>

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

*** 解决 ***
日志中有这么一段
```
[INFO] --- maven-hawtjni-plugin:1.11:generate (default) @ leveldbjni ---
[INFO] Analyzing classes...
[INFO] Generating...
[INFO] Wrote: /home/kv-pmem/xiaohu/leveldbjni-master/leveldbjni/target/generated-sources/hawtjni/native-src/leveldbjni.cpp
[INFO] Wrote: /home/kv-pmem/xiaohu/leveldbjni-master/leveldbjni/target/generated-sources/hawtjni/native-src/leveldbjni_stats.h
[INFO] Wrote: /home/kv-pmem/xiaohu/leveldbjni-master/leveldbjni/target/generated-sources/hawtjni/native-src/leveldbjni_stats.cpp
[INFO] Wrote: /home/kv-pmem/xiaohu/leveldbjni-master/leveldbjni/target/generated-sources/hawtjni/native-src/leveldbjni_structs.h
[INFO] Wrote: /home/kv-pmem/xiaohu/leveldbjni-master/leveldbjni/target/generated-sources/hawtjni/native-src/leveldbjni_structs.cpp
[INFO] Wrote: /home/kv-pmem/xiaohu/leveldbjni-master/leveldbjni/target/generated-sources/hawtjni/native-src/hawtjni.h
[INFO] Wrote: /home/kv-pmem/xiaohu/leveldbjni-master/leveldbjni/target/generated-sources/hawtjni/native-src/hawtjni.c
[INFO] Wrote: /home/kv-pmem/xiaohu/leveldbjni-master/leveldbjni/target/generated-sources/hawtjni/native-src/windows/stdint.h
[INFO] Done.
```
`hawtjni` 这个插件 <https://github.com/fusesource/hawtjni>

### can not be used when making a shared object; recompile with -fPIC
编译生成 jni.so 时不能链接 leveldb 的静态库(.a)

### leveldb skiplist错误
```
Stack: [0x00007f17a70f1000,0x00007f17a71f2000],  sp=0x00007f17a71eff00,  free space=1019k
Native frames: (J=compiled Java code, j=interpreted, Vv=VM code, C=native code)
C  [libleveldb.so.1+0x2b116]  leveldb::SkipList<char const*, leveldb::MemTable::KeyComparator>::SkipList(leveldb::MemTable::KeyComparator, leveldb::Arena*, bool)+0x216

Java frames: (J=compiled Java code, j=interpreted, Vv=VM code)
J 532  org.fusesource.leveldbjni.internal.NativeDB$DBJNI.Put(JLorg/fusesource/leveldbjni/internal/NativeWriteOptions;Lorg/fusesource/leveldbjni/internal/NativeSlice;Lorg/fusesource/leveldbjni/internal/NativeSlice;)J (0 bytes) @ 0x00007f17c53d0b7d [0x000    07f17c53d0b00+0x7d]
J 529 C1 org.fusesource.leveldbjni.internal.NativeDB.put(Lorg/fusesource/leveldbjni/internal/NativeWriteOptions;[B[B)V (76 bytes) @ 0x00007f17c53dd9ac [0x00007f17c53dcfc0+0x9ec]
J 525 C1 org.fusesource.leveldbjni.internal.JniDB.put([B[BLorg/iq80/leveldb/WriteOptions;)Lorg/iq80/leveldb/Snapshot; (50 bytes) @ 0x00007f17c53da4e4 [0x00007f17c53da3e0+0x104]
J 523 C1 org.fusesource.leveldbjni.internal.JniDB.put([B[B)V (15 bytes) @ 0x00007f17c53d7ebc [0x00007f17c53d7d60+0x15c]
J 448 C1 com.yahoo.ycsb.db.LevelDbJniClient.insert(Ljava/lang/String;Ljava/lang/String;Ljava/util/Map;)Lcom/yahoo/ycsb/Status; (102 bytes) @ 0x00007f17c53ad73c [0x00007f17c53acf60+0x7dc]
J 446 C1 com.yahoo.ycsb.DBWrapper.insert(Ljava/lang/String;Ljava/lang/String;Ljava/util/Map;)Lcom/yahoo/ycsb/Status; (163 bytes) @ 0x00007f17c53a66b4 [0x00007f17c53a61e0+0x4d4]
J 481 C1 com.yahoo.ycsb.workloads.CoreWorkload.doInsert(Lcom/yahoo/ycsb/DB;Ljava/lang/Object;)Z (201 bytes) @ 0x00007f17c53c3ccc [0x00007f17c53c38c0+0x40c]
j  com.yahoo.ycsb.ClientThread.run()V+226
j  java.lang.Thread.run()V+11
v  ~StubRoutines::call_stub
```