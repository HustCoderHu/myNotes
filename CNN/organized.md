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
