ubuntu1404 安装 caffe 使用的 opencv3
---
# 1 opencv3安装

```
apt install cmake libjpeg-dev libpng-dev
cd opencv
mkdir build && cd build

cmake -D CMAKE_BUILD_TYPE=RELEASE \
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
make -j8
make install
```

可以 make uninstall 之后再 install

cmake如果成功,会显示详细的模块配置
```
General configuration for OpenCV 3.3.0 =======
...
OpenCV modules
To be built:                 cudev core imgproc imgcodecs python2
```
opencv3 常用的模块是 core imgproc imgcodecs highgui python2  
opencv3 中的imread从 imgproc 移到 imgcodecs 中了  
highgui 是caffe ncnn会用到的  
因为很多模块 Disabled,所以编译时间不长

# 2 caffe 库路径配置
opencv默认安装到 /usr/lib 下  
caffe 的 Makefile.config 的 LIBRARY_DIRS 不需要修改  
如果 opencv 指定了安装路径，即使把路径加到了 LIBRARY_DIRS caffe编译或者runtest也会出问题
then
```
make clean
make all j8
make test # 可选
make runtest # 可选
```

不要在 cmake的时候-D CMAKE_INSTALL_PREFIX=. 
指定安装路径，不然如下
如果是用
```
ln -s lib/libopencv_imgcodecs.so /usr/local/lib
// 或者
ln -s lib/libopencv_imgcodecs.so /usr/lib
```
创建的软链接
编译caffe的时候 还是找不到 opencv_imgcodecs 
```
AR -o .build_release/lib/libcaffe.a
LD -o .build_release/lib/libcaffe.so.1.0.0
/usr/bin/ld: cannot find -lopencv_imgcodecs
collect2: error: ld returned 1 exit status
make: *** [.build_release/lib/libcaffe.so.1.0.0] Error 1
```



# 3 错误排查
## 3.1 opencv cmake 错误
```
"FATAL: In-source builds are not allowed.
You should create separate directory for build files."
then rm CMakeCache.txt -f
```
参考["In-source builds are not allowed" in cmake](https://stackoverflow.com/questions/45518317/in-source-builds-are-not-allowed-in-cmake)  
解决之后，cmake再来一次

## 3.2 tools/caffe: runtest error while loading shared libraries opencv
先看下 /usr/lib下有没有相应的 lib，没有的话有两种解决办法

**1 软链接**

软链接到 /usr/lib，比如
```
ln -s /home/cnn/Downloads/opencv-3.3.0/build/lib/libopencv_core.so.3.3 /usr/lib/
```

**2 ld的配置文件**

类似cuda的处理办法
```
echo "path/to/theLib" > /etc/ld.so.conf.d/opencv.conf
sudo ldconfig -v
```
<https://github.com/GaoHongchen/DIPDemoQt5/issues/1>


# 参考
<http://www.cnblogs.com/freeweb/p/7509480.html>
<https://docs.opencv.org/master/d7/d9f/tutorial_linux_install.html>
<http://blog.csdn.net/youngpan1101/article/details/58027049>  
模块介绍  
<https://docs.opencv.org/master/d9/df8/tutorial_root.html>
