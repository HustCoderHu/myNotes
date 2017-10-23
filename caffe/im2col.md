im2col 解释
---
y = Wx + b

卷积层输出 = 权值矩阵 * 输入特征图转化得到的矩阵

权值矩阵尺度 = (卷积组输出通道数) * (卷积组输入通道数*卷积核高*卷积核宽)

输入特征图转化得到的矩阵尺度 = (卷积组输入通道数*卷积核高*卷积核宽) * (卷积层输出单通道特征图高 * 卷积层输出单通道特征图宽)

C: 卷积组输出通道数

L: 卷积组输入通道数 * 卷积核高 * 卷积核宽

N: 输出单通道特征图高 * 输出单通道特征图宽

维度:  
y: C * N

W: C * L

x: L * N
![image](https://raw.githubusercontent.com/HustCoderHu/myNotes/master/caffe/conv_im2col_cpu.PNG)  

# 参考
http://blog.csdn.net/langb2014/article/details/51546208  
https://github.com/BVLC/caffe/blob/master/src/caffe/util/im2col.cpp  