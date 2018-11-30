
预定义变量|含义
-|-
PROJECT_SOURCE_DIR | 工程的根目录
PROJECT_BINARY_DIR | 运行cmake命令的目录,通常是${PROJECT_SOURCE_DIR}/build
CMAKE_INCLUDE_PATH | 环境变量,非cmake变量
CMAKE_LIBRARY_PATH | 环境变量
CMAKE_CURRENT_SOURCE_DIR | 当前处理的CMakeLists.txt所在的路径


# pkg_check_modules 搜索路径
``` CMakeList
set( ENV{PKG_CONFIG_PATH} $ENV{PKG_CONFIG_PATH} /lib/pkgconfig)
```
注意 `ENV`

<https://stackoverflow.com/questions/20447821/add-temporarily-path-to-pkg-config-within-cmake-script>  

# 常用调试

```
message(STATUS "SRCS: " ${SRCS})
```


# reference
cmake使用示例与整理总结  
<https://blog.csdn.net/wzzfeitian/article/details/40963457>  
CMake 查找库: find_package vs pkg_check_modules  
<http://durant35.github.  io/2017/07/14/tool_CMake_find_package$pkg_check_module/>  