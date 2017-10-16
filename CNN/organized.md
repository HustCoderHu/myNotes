Speeding up Convolutional Neural Networks with Low Rank Expansions

# 1 Introduction
Layer链导致计算开销大，本文提出一种简单办法，通过现有的框架的少量修改，加速推断过程。
精度会有些损失，但是可以和推断速度trade off

redundancy that exists between different feature channels and filters
通道间的冗余，
两种方案*2种优化法，方案架构无关

## Related work
-- 1) low rank approximations 低秩近似 + clustering of filters = 单卷基层1.6x  
-- 2) 权重 8bit 量化，加速 & 小幅度精度损失  
-- 3) rank-1 filters 逼近 image filters，大幅度加速 & 小幅度精度损失  
**硬件相关优化**
cuda-convnet and Caffe、SSSE3 SSSE4指令集,还有FPGA  
[38]selective search 先找object 然后搜索

# 2 filter Approximations
N: 卷积核数 H,W: img height width  
直接实现卷积 O(d^2 * H * W * N)  
y[i] = f[i] * x  
[31][34][35]包含M个filters的集合，线性组合代替每个filter  
**计算复杂度**  
y[i] = a[i][k] * s[k] * x	- 对 k 求和  
1<= k <=M, 1<= i <= N  
O( (d^2 * M + M * N) * H * W)  
只要 M < (d^2 * N)/(d^2 + N) 就可以加速  

[31]还提出进一步拆分集合中的f，成行列向量  
s[i] * x = v[i] * (h[i] * x)  

## 2.1 Approximations CNN Filter Banks
scheme1  y[i] = a[i][k] * s[k] * x  
scheme2  s[i] * x = v[i] * (h[i] * x)  

## 2.2 Optimization
两种优化  

### 2.2.1 f 重构优化
-- scheme1 L2 重构错误率 公式 (2)  
-- scheme2 公式 (3)  

### 2.2.2 data 重构优化