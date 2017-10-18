#cublas
---
# 1 INTRODUCTION
(基本线性代数子程序)数值程序库  
[官方原文](http://docs.nvidia.com/cuda/cublas/index.html#introduction)  

从CUDA6.0之后，cuBLAS 库有两套API，第一个称为**cuBLAS API**，第二个是**CUBLASXT API**  
使用**cuBLAS API** ，应用一定要在GPU的内存空间中分配需要的矩阵和向量，并进行赋值，然后调用适用的cuBLAS函数，之后从GPU内存读取数据到host端。cuBLAS API 提供了读写GPU内存的帮助函数   
使用**CUBLASXT API**，应用需要把数据放在HOST，然后库会根据用户的要求将数据调度到GPUS的处理上  

## 1.1 Data layout
为了最大限制满足已有的Fortran 环境，cuBLAS 采用的是列优先的存储方式和基于1的检索  
1  4  7  
2  5  8  
3  6  9  
```
#define IDX2C(i,j,ld) (((j)*(ld))+(i))
```
这个宏表示，矩阵第i行第j列的元素在C语言中 数组存储位置的索引，ld表示的是 矩阵的第一维的元素个数，就是 矩阵的行数  
**使用这种宏的场景描述：**  
你要操作的矩阵是为CUBLAS库函数准备的。就是说你正在准备将这个矩阵作为参数传递给CUBLAS的库函数。
你从CUBLAS中得到了某个矩阵，你可以使用这个宏来索引相应的元素。  
更详细的看[这里](http://dev.dafan.info/detail/157153?p=30-53)

## 1.2 New and Legacy cuBLAS API
从4.0版本之后，cuBLAS库提供传统的API的同时也提供新更新的API。这个地方将介绍新提供的API和它的优点以及传统的API的区别  
通过头文件"cublas_v2.h"使用新的API
下面的特征是 传统API没有的： 
- 1 cuBLAS的上下文句柄用函数初始化并且被传递到后续每一个使用的库函数。这使得用户在多主机线程和多GPU时对库的设置有更多的控制权。这使得cuBLAS API可以重入（reentrant）
- 2 标量α和β可以通过在主机或者设备中声明然后进行传递，而不是在主机上被分配然后通过值进行传递。这个改变使得库函数可以用流进行执行同步，即使α和β 是在前面的核产生的
- 3 当一个库函数返回一个标量结果，它可以通过一个在主机或者设备声明的变量返回，而不是返回主机中的数值。这个使得库函数可以直接使用返回值
- 4
错误状态 cublasStatus_t，更好调试
- 5
the cublasAlloc() and cublasFree() functions have been deprecated. This change removes these unnecessary wrappers around cudaMalloc() and cudaFree(), respectively
- 6
cublasSetKernelStream() 重命名为 cublasSetStream()

## 1.3 Example code

# 2 Using the cuBLAS API
## 2.1 General description
### 2.1.1 Error status
所有的cuBLAS库函数都返回the error status cublasStatus_t

### 2.1.2 cuBLAS context
应用必须通过调用cublasCreate()函数来初始化cuBLAS 库的上下文句柄。通过cublasDestory()来释放。应用程序可以使用cudaSetDevice()结合初始化的独特上下文句柄，应用程序根据不同的句柄将数据传递到不同的设备进行计算。如果在一个主机里面使用不同的设置，在使用新设备前调用cudaSetDevice()，然后cublasCreate()根据当前的设置设备来初始化不同的上下文句柄。

### 2.1.5 Scalar Parameters
有两种类别的函数使用标量参数：  
- 1 将在主机或者设备内定义的α或者β 作为变化因子的函数，如gemm
- 2 返回在设备或者主机上的标量的函数，如 amax(), amin(), asum(), rotg(), rotmg(), rotmg(), dot() 和 nrm2()  

对于第一种类别，当指针模式设置为”CUBLAS_POINTER_MODE_HOST”,这种情况标量α或者β可以在栈或者分配在堆中。Underneath the CUDA kernels related tothat functions will be launched with the value of alpha and/or beta. 因此如果他们在堆中被分配，他们可以在调用放回后被释放即使内核是异步的。当指针的模式设置为“CUBLAS_POINTER_MODE_DEVICE”α或者β 应该在设备上可以被访问并且不能被修改直到核调用完成。注意由于cudaFree隐式调用cudaDeviceSynchronize()，cudaFree() can still be called on alpha and/or beta just after the call but it would defeat the purpose of using this pointer mode in that case.  
第二种模式：指针模式为”CUBLAS_POINTER_MODE_HOST”，GPU计算完成后结果会赋值回Host。指针模式设置为 “CUBLAS_POINTER_MODE_DEVICE“这函数立即返回。这种情况，和矩阵或者向量结果相似，标量结果只有等例程在GPU上完成。为了从主机里面读结果，这个就要求正确的同步。 
在任何一种情况，指针模式为“CUBLAS_POINTER_MODE_DEVICE“库函数允许在完全异步时执行，即使alpha 和 beta是前一个核产生。比如，当用cuBLAS库循环的方法解决线性系统和特征值的问题是会出现

## 2.2 cuBLAS Datatypes
### 2.2.1 cublasHandle_t
用cublasCreate() 初始化，其他函数调用的时候传入

### 2.2.2 cublasStatus_t
所有函数返回这个类型
具体参见[官网](http://docs.nvidia.com/cuda/cublas/#)

### 2.2.7 cublasSetPointerMode_t
一次函数调用中多个标量scalar的mode必须相同
Value | Meaning
:- | :-:
CUBLAS_POINTER_MODE_HOST   | the scalars are passed by reference on the host
CUBLAS_POINTER_MODE_DEVICE | the scalars are passed by reference on the device


## 2.3 CUDA Datatypes

## 2.4 Helper functions
### 2.4.1 cublasCreate()
```
cublasStatus_t cublasCreate(cublasHandle_t *handle)
```
Return Value | Meaning
:- | :-:
CUBLAS_STATUS_SUCCESS         | the initialization succeeded
CUBLAS_STATUS_NOT_INITIALIZED | the CUDA? Runtime initialization failed
CUBLAS_STATUS_ALLOC_FAILED    | the resources could not be allocated  

### 2.4.2 cublasDestroy()
```
cublasStatus_t cublasDestroy(cublasHandle_t handle)
```

### 2.4.8 cublasSetVector()
```
cublasStatus_t cublasSetVector(int n, int elemSize,
		const void *x, int incx, 
		void *y, int incy)
```
将n个长度为elemSize的从主机内存地址 x 复制到 GPU内存地址y  
incx 和 incy 为每个元素头间距离


### 2.4.9 cublasGetVector()
参数同上，从GPU到host

### 2.4.10 cublasSetMatrix()
```
cublasStatus_t cublasSetMatrix(int rows, int cols, int elemSize, 
		const void *A, int lda, void *B, int ldb)
```

### 2.4.11 cublasGetMatrix()

## 2.5 Level-1 functions
scalar and vector based operations
### 2.5.4 axpy()
$y += αx$  
```
cublasStatus_t cublasSaxpy(cublasHandle_t handle, int n,
			const float *alpha, 
			const float *x, int incx, 
			float *y, int incy)
```

### 2.5.6 dot()
向量点积
```
cublasStatus_t cublasSaxpy(cublasHandle_t handle, int n,
			const float *x, int incx, 
			const float *y, int incy, 
			float *result)
```

### 2.5.6 scal()
向量*标量
$x += αx$  
```
cublasStatus_t cublasSscal(cublasHandle_t handle, int n,
			const float *alpha, 
			float *x, int incx)
```

## 2.6 Level-2 functions

## 2.7 Level-3 functions

$y = α op ( A ) x + β y$  
where A is a m × n matrix stored in column-major format, x and y are vectors, and α and β are scalars. Also, for matrix A  
op ( A ) = A  if transa == CUBLAS_OP_N A T  if transa == CUBLAS_OP_T A H  if transa == CUBLAS_OP_H



# 参考
[玩玩CUBLAS(3)——level2函数](http://dev.dafan.info/detail/157153?p=30-53)