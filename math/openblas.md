BLAS
---
# 1 OpenBLAS
[developer manual](https://github.com/xianyi/OpenBLAS/wiki/Developer-manual)

## 1.1 OpenBLAS 接口层
在OpenBLAS接口层中，运算又分为三个类型，分别是level1到3，其中level3对应矩阵和矩阵的运算，level2和level1依次维度越来越低。不过这些level1~3都是BLAS内部计算时使用的接口（源代码基本在driver/level下），对外界用户的接口是不涉及这个概念的（对外接口基本都在interface文件夹下） 
[github](https://github.com/xianyi/OpenBLAS/tree/develop/interface)  
每一个源代码文件对应一种操作，如gemm指的是普通矩阵乘法（General Matrix Multiplication）而gemv指的是普通矩阵向量乘法（General Matrix Vector）

[详细说明](https://www.ibm.com/support/knowledgecenter/SSFHY8_5.3.0/com.ibm.cluster.essl.v5r3.essl100.doc/am5gr_hsgemm.htm)，大致为C代表复数计算，Z代表双精度复数，而S代表单精度常数
全函数声明 https://github.com/xianyi/OpenBLAS/blob/develop/cblas.h  

## 1.2 举例
**矩阵相乘**
```
void cblas_sgemm(const enum CBLAS_ORDER Order,
	const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_TRANSPOSE TransB, 
	const int M, const int N, const int K, 
	const float alpha, 
	const float  *A, const int lda, 
	const float  *B, const int ldb,
	const float beta, 
	float  *C, const int ldc)
```
C := alpha*op(A)*op(B) + beta*C  
op: 不转置，转置，共轭  

参数 | 值
:- | :-
Order          | CblasRowMajor, CblasColMajor
TransA, TransB | CblasNoTrans, CblasTrans
M              | op(A) 和 C 的行数
N              | op(B) 和 C 的列数
K              | op(A) 的列数和 op(B) 的行数


# 2 Caffe 中 BLAS 的使用
矩阵运算函数在文件math_functions.hpp中可以找到。其中的函数多是对BLAS相应API的包装  
[参考](http://blog.csdn.net/seven_first/article/details/47378697)

## 2.1 矩阵与矩阵，矩阵与向量的乘法
```
template<>
void caffe_cpu_gemm<float>(
	const CBLAS_TRANSPOSE TransA, const CBLAS_TRANSPOSE TransB, 
	const int M, const int N, const int K,
    const float alpha, const float* A, const float* B, 
	const float beta, float* C)
{
  int lda = (TransA == CblasNoTrans) ? K : M;
  int ldb = (TransB == CblasNoTrans) ? N : K;
  cblas_sgemm(CblasRowMajor, TransA, TransB, M, N, K, alpha, A, lda, B,
      ldb, beta, C, N);
}
```
[参考](https://xmfbit.github.io/2017/03/08/mathfunctions-in-caffe/)  

## 2.2 矩阵/向量的加减
- caffe_axpy(N, alpha, x, mutable y)        向量加法 Y = alpha * X + Y
- caffe_axpby(N, alpha, x, beta, mutable y) 向量加法 Y = alpha * X + beta * Y

## 2.3 内存相关

