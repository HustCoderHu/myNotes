
通过 caffe 加载 ResNet-50 的模型时，在产生两条及以上向上分支时，  
比如 pool1, pool5, 中间有分支的ReLU，caffe 会生成 一个 type 为 Split 的层  
具体的代码在文件 insert_splits.cpp 中