win 上用 mingw64 构建 opencv3
---
# 1 安装 cmake
下载 win32-x64.zip  
<https://cmake.org/download/>  
解压之后将 bin 目录加到 PATH 中

# 2 安装 mingw64
下载 x86_64-win32-seh  
<https://sourceforge.net/projects/mingw-w64/files/>  
解压之后将 bin 目录加到 PATH 中

# 3 cmake
- `CMAKE_SH-NOTFOUND` -- 解决 cmake 和 sh.exe (和 git 在同一路径下)的冲突
- `ENABLE_PRECOMPILED_HEADERS` -- 解决make过程中的 **windres.exe: unknown option -- W**

确定要编译的模块，全部编译要很久，参考 <https://opencv.org/releases.html> 对应版本的 Documentation  
格式是: **BUILD_模块**  
cmake 会给出一个 summary，留意一下，下面是我用的
```
cmake -G "MinGW Makefiles" \
-D CMAKE_SH="CMAKE_SH-NOTFOUND" \
-D ENABLE_PRECOMPILED_HEADERS=OFF \
CMAKE_BUILD_TYPE=RELEASE \
\
-D BUILD_opencv_python3=OFF \
-D BUILD_opencv_python2=OFF \
\
-D BUILD_opencv_gpu=OFF \
-D BUILD_DOCS=OFF \
-D BUILD_PERF_TESTS=OFF \
-D BUILD_TESTS=OFF \
-D BUILD_WITH_DEBUG_INFO=OFF \
-D BUILD_opencv_apps=OFF \
-D BUILD_opencv_calib3d=OFF \
-D BUILD_opencv_contrib=OFF \
-D BUILD_opencv_features2d=OFF \
-D BUILD_opencv_flann=OFF \
-D BUILD_opencv_highgui=ON \
-D BUILD_opencv_ml=OFF \
-D BUILD_opencv_nonfree=OFF \
-D BUILD_opencv_objdetect=OFF \
-D BUILD_opencv_photo=OFF \
-D BUILD_opencv_stitching=OFF \
-D BUILD_opencv_superres=OFF \
-D BUILD_opencv_ts=OFF \
-D BUILD_opencv_video=OFF \
-D BUILD_opencv_videoio=OFF \
-D BUILD_opencv_videostab=OFF \
-D WITH_Cuda=OFF \
-D BUILD_opencv_cudaarithm=OFF \
-D BUILD_opencv_cudafilters=OFF \
-D BUILD_opencv_cudaimgproc=OFF \
-D BUILD_opencv_cudawarping=OFF \
-D BUILD_opencv_dnn=OFF \
-D WITH_IPP=OFF \
..
```
## possible errors
### 1 MinGW sh.exe must NOT be in your path  
参考 <https://stackoverflow.com/questions/41333215/mingw-sh-exe-must-not-be-in-your-path>  

如果不把python2 和 3 off 掉，linking就会出错，link below
```
Linking CXX shared module ..\..\lib\python3\cv2.cp36-win32.pyd 
CMakeFiles\opencv_python3.dir/objects.a(cv2.cpp.obj):cv2.cpp:(.data+0x7d78): undefined reference to `PyType_GenericNew'
CMakeFiles\opencv_python3.dir/objects.a(cv2.cpp.obj):cv2.cpp:(.text$_Z13pyopencv_fromIN2cv8FileNodeEEP7_objectRKT_+0x11): undefined reference to `__imp_PyObject_Malloc'
CMakeFiles\opencv_python3.dir/objects.a(cv2.cpp.obj):cv2.cpp:(.text$_Z13pyopencv_fromIN2cv8FileNodeEEP7_objectRKT_+0x21): undefined reference to `__imp_PyObject_Init
```

# 4 make
```
mingw32-make -jn
```
这里报的错误基本是 cmake 那里没解决掉的
## possible errors
### 1 - Building OpenCV with MinGW (gcc 7.1.0 and cmake 3.9.0)-> windres.exe: unknown option -- W  
ENABLE_PRECOMPILED_HEADERS 没有 off，参考  
<http://answers.opencv.org/question/168538/building-opencv-with-mingw-gcc-710-and-cmake-390-windresexe-unknown-option-w/>  

### 2 - error: '::hypot' has not been declared when compiling with MingGW64  
最后 99% 链接的时候出这个错简直让人崩溃。  
原因很简单，没有关掉 `opencv_python3` 和 `opencv_python2` 模块  
要用py的，直接去找别人要 cv2.cyd文件

测试 cmake 检测 python 路径的时候用过这两行
```
-D PYTHON3_INCLUDE_DIR="c:/mydir/Python36-32/include"
-D PYTHON3_LIBRARY="c:/mydir/Python36-32/libs/libpython36.a"
```

# 参考
error: '::hypot' has not been declared when compiling with MingGW64  
<https://github.com/Theano/Theano/issues/4926>  
cmath:1136:11: error: '::hypot' has not been declared  
<http://www.jianshu.com/p/9c96a026f900>  