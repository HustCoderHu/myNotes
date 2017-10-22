ubuntu1404 安装 opencv3
--- 
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
-D BUILD_opencv_feature2d=OFF \
-D BUILD_opencv_flann=OFF \
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

cmake如果成功,就会详细的模块配置
```
General configuration for OpenCV 3.3.0 =======
...
OpenCV modules
To be built:                 cudev core imgproc imgcodecs python2
```

因为很多模块 Disabled,所以编译时间不长

如果cmake遇到 
```
"FATAL: In-source builds are not allowed.
You should create separate directory for build files."
then rm CMakeCache.txt -f
```
参考["In-source builds are not allowed" in cmake](https://stackoverflow.com/questions/45518317/in-source-builds-are-not-allowed-in-cmake)  
解决之后，cmake再来一次

install 完之后把 build/lib 的绝对路径加到 caffe 的 Makefile.config 的 LIBRARY_DIRS 后面  
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

caffe runtest 的时候
如果提示 error while loading shared libraries opencv
就先 看下 /usr/lib下有没有 相应lib
没有的话有两种解决办法
**1 软链接**
软链接到 /usr/lib，比如
```
ln -s /home/cnn/Downloads/opencv-3.3.0/build/lib/libopencv_core.so.3.3 /usr/lib/
```
**2 链接的配置文件**
类似cuda的处理办法
[issues](https://github.com/GaoHongchen/DIPDemoQt5/issues/1)


# 参考
http://www.cnblogs.com/freeweb/p/7509480.html
https://docs.opencv.org/master/d7/d9f/tutorial_linux_install.html

http://blog.csdn.net/youngpan1101/article/details/58027049

[模块介绍](https://docs.opencv.org/master/d9/df8/tutorial_root.html)
