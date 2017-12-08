alexnet单图测试
---

# 1 准备
- 1 deploy.prototxt
- 2 caffemodel
- 3 mean.binaryproto
- 4 label.txt
- 5 img

# 2 运行

examples\cpp_classification 目录下有 classification.cpp，这份代码利用模型对输入图片进行预测，得出概率分布

因为用到了cv的类，所以需要安装opencv


# 参考
[Caffe学习系列(20)：用训练好的caffemodel来进行分类](http://www.cnblogs.com/denny402/p/5111018.html)
[官方文档](http://caffe.berkeleyvision.org/)
[](http://nbviewer.jupyter.org/github/BVLC/caffe/blob/master/examples/00-classification.ipynb)
