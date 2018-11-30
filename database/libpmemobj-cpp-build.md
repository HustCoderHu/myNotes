libpmemobj-cpp 编译排错

---

# cmake 错误

```
--   No package 'libpmemobj' found
CMake Error at /usr/local/share/cmake-3.13/Modules/FindPkgConfig.cmake:452 (message):
  A required package was not found
Call Stack (most recent call first):
  /usr/local/share/cmake-3.13/Modules/FindPkgConfig.cmake:622 (_pkg_check_modules_internal)
  CMakeLists.txt:122 (pkg_check_modules)
```

## 解决
`pkg_check_modules ` 没有找到 LIBPMEMOBJ
在 `CMakeList.txt` 出错前增加以下内容
``` CMakeList
set(ENV{PKG_CONFIG_PATH} $ENV{PKG_CONFIG_PATH} /lib64/pkgconfig)
```

这个添加的路径可以通过 locate libpmemboj 找到
使用 LIBPMEMOBJ++ 的时候也是类似的方法

# make 错误
```
/home/kv-pmem/libpmemobj-cpp/tests/external/libcxx/array/array.data/data_const.pass.cpp:37:11: error: ‘max_align_t’ in namespace ‘std’ does not name a type
   typedef std::max_align_t T;
```

## 解决
yum 安装的 gcc 和 自己编译安装的 gcc 冲突了  
yum remove gcc 之后就好了 (我想打死 yiwen)
