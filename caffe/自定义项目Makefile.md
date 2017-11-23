使用caffe类，库函数的项目 Makefile
---
```
BLAS_INCLUDE := /opt/OpenBLAS/include
BLAS_LIB := /opt/OpenBLAS/lib

CUDA_INCLUDE := /usr/local/cuda/include
CUDA_LIB := /usr/local/cuda/lib

# 16041-server
CAFFE_ROOT := /root/caffe
# 1080ti
# CAFFE_ROOT := /home/caffe/caffe
# CAFFE_ROOT := /home/caffe/caffe-master
# CAFFE_ROOT := /home/cnn/caffe/


####### Compiler, tools and options

CXX         = g++
CXXFLAGS    = -std=c++11 -O2 -Wall
# -D CPU_ONY
INCPATH     := -I.
# -I$(src)

LINKER      = g++
# LFLAGS      = -static # -Wl,-s -Wl
LIBS        := -L.


# caffe
INCPATH     += -I$(CAFFE_ROOT)/include
INCPATH     += -I$(CAFFE_ROOT)/build/src # caffe.pb.h
LIBS        += -L$(CAFFE_ROOT)/build/lib # libcaffe.so
LIBS        += -lcaffe

# opencv
# INCPATH += -I/usr/include
# LIBS += -L/usr/lib/x86_64-linux-gnu
# LIBS += -lopencv_core -lopencv_imgproc -lopencv_highgui

# blas
INCPATH     += -I$(BLAS_INCLUDE)
LIBS        += -L$(BLAS_LIB)
LIBS        += -lopenblas

# cuda
INCPATH     += -I$(CUDA_INCLUDE)
LIBS        += -L$(CUDA_LIB)

# others
LIBS        += -lglog -lgflags -lprotobuf -lboost_system -lboost_filesystem 
# -lboost_system -lboost_thread -lleveldb -llmdb -latlas
# -lstdc++ -lcblas

# LIBRARIES += glog gflags protobuf boost_system boost_filesystem m hdf5_hl hdf5

####### Files

# SOURCES     = filtersPruning.cpp
# SOURCES     = $(shell find -iname '*.cpp') # 自动寻找当前目录下的源文件
SOURCES     = $(shell echo *.cpp)
OBJECTS     = $(SOURCES:.cpp=.o)
DESTDIR_TARGET  = a.bin # $(SOURCES:.cpp=.bin)

.PHONY: all clean

all: clean $(DESTDIR_TARGET)

clean:
	rm -rf $(OBJECTS)
	rm -rf $(DESTDIR_TARGET)

####### Compile
# $(OBJECTS:%.o): %.cpp
# $(SOURCES:.cpp=.o)
# %.o: %.cpp
$(OBJECTS): %.o: %.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS) $(INCPATH)

$(DESTDIR_TARGET): $(OBJECTS)
	$(LINKER) -o $(DESTDIR_TARGET) $(OBJECTS) $(-Wall) $(LFLAGS) $(LIBS)
```

<http://blog.csdn.net/zizi7/article/details/54951657>  
<http://blog.csdn.net/xjz18298268521/article/details/54138175>  
<https://seisman.github.io/how-to-write-makefile/rules.html>