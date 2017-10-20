cuSPARSE 线性代数库，主要针对稀疏矩阵之类的
---

# 1 THE CUSPARSE LIBRARY
对稀疏矩阵之类的操作尤其独到的用法，使用很宽泛。他当对稠密和稀疏的数据格式都支持

下图是该库的一些函数调用，从中可以对其功能有一个大致的了解。cuSPARSE将函数以level区分，所有level 1的function仅操作稠密和稀疏的vector。所有level2函数操作稀疏矩阵和稠密vector。所有level3函数操作稀疏和稠密矩阵。
![](http://images0.cnblogs.com/blog2015/657339/201506/210225419044278.png)

# 2 cuSPARSE Data Storage Formats
cuSPARSE支持很多种稀疏矩阵的存储方式，本文只介绍其中三种


**1 稠密（dens）矩阵**  
![](http://images0.cnblogs.com/blog2015/657339/201506/210227131072082.png)  

**2 Coordinate（COO）**  
一个存储32-bit的浮点类型数据的稀疏矩阵，索引使用32-bit的整型格式，那么只有当非零数据少于于矩阵的三分之一的时候才会节约存储空间  
![](http://images0.cnblogs.com/blog2015/657339/201506/210230022952928.png)  


**3 Compressed Sparse Row（CSR）**  
CSR和COO相似，唯一不同就是非零值的行索引。COO模式下，所有非零值都会对应一个int的行索引，而CSR则是存储一个偏移值，这个偏移值是所有属于同一行的值拥有的属性。如下图所示，相比COO，减少了row：  

![](http://images0.cnblogs.com/blog2015/657339/201506/210230186234373.png)  

因为所有存储在同一行的数据在内存中是相邻的，要找到某一行对应的值只需要一个偏移量和length。例如，如果只想知道第三行的非零值，我们可以使用偏移量为2，length为2在V中检索，如下图所示：  
![](http://images0.cnblogs.com/blog2015/657339/201506/210230338105032.png)  
![](http://images0.cnblogs.com/blog2015/657339/201506/210231036235625.png)  

# 3 Formatting Conversion with cuSPARSE
这个过程应该尽量避免，转换不仅需要有计算的开销，还有额外存储的空间浪费  
![](http://images0.cnblogs.com/blog2015/657339/201506/210233461236647.png)  

# 4 Demonstrating cuSPARSE

```
// Create the cuSPARSE handle
cusparseCreate(&handle);
// Allocate device memory for vectors and the dense form of the matrix A
...
// Construct a descriptor of the matrix A
cusparseCreateMatDescr(&descr);
cusparseSetMatType(descr, CUSPARSE_MATRIX_TYPE_GENERAL);
cusparseSetMatIndexBase(descr, CUSPARSE_INDEX_BASE_ZERO);
// Transfer the input vectors and dense matrix A to the device
...
// Compute the number of non-zero elements in A
cusparseSnnz(handle, CUSPARSE_DIRECTION_ROW, M, N, descr, dA,M, dNnzPerRow, &totalNnz);
// Allocate device memory to store the sparse CSR representation of A
...
// Convert A from a dense formatting to a CSR formatting, using the GPU
cusparseSdense2csr(handle, M, N, descr, dA, M, dNnzPerRow,dCsrValA, dCsrRowPtrA, dCsrColIndA);
// Perform matrix-vector multiplication with the CSR-formatted matrix A
cusparseScsrmv(handle, CUSPARSE_OPERATION_NON_TRANSPOSE,M, N, totalNnz, &alpha, descr, dCsrValA, dCsrRowPtrA,dCsrColIndA, dX, &beta, dY);
// Copy the result vector back to the host
cudaMemcpy(Y, dY, sizeof(float) * M, cudaMemcpyDeviceToHost);
```

- 1 使用cusparseCreate创建库的handle。
- 2 使用cudaMalloc分配device内存空间用来存储矩阵和向量，并分别使用dense和CSR两种格式存储。
- 3 cusparseCreateMatDescr和cusparseSetMat*使用来配置矩阵属性的，cudaMemcpy用来拷贝数据到device，cusparseSdense2csr来生成CSR格式的数据，非零数据个数有cusparseSnnz计算得到。
- 4 cusparseScsrmv是矩阵和向量乘操作函数。
- 5 再次使用cudaMemcpy将结果拷贝回host。
- 6 释放资源。

# 参考
[cnblogs](http://www.cnblogs.com/1024incn/p/4591450.html)  
[官方docs](http://docs.nvidia.com/cuda/cusparse/index.html)  
[]()