alexnet vgg 单图测试
---

# 1 准备
- 1 deploy.prototxt
- 2 .caffemodel
- 3 mean.binaryproto
- 4 label.txt
- 5 img

# 2 运行

examples\cpp_classification 目录下有 classification.cpp，这份代码利用模型对输入图片进行预测，得出概率分布

因为用到了cv的类，所以需要安装opencv，参考 [openCV3安装](https://github.com/HustCoderHu/myNotes/blob/master/baseLibs/openCV3_installation.md)

## 2.1 alexnet 结果
```
./build/examples/cpp_classification/classification.bin \
  models/bvlc_alexnet/deploy.prototxt \
  models/bvlc_alexnet/bvlc_alexnet.caffemodel \
  data/ilsvrc12/imagenet_mean.binaryproto \
  data/ilsvrc12/synset_words.txt \
  examples/images/cat.jpg
输出
---------- Prediction for examples/images/cat.jpg ----------
0.3094 - "n02124075 Egyptian cat"
0.1761 - "n02123159 tiger cat"
0.1221 - "n02123045 tabby, tabby cat"
0.1132 - "n02119022 red fox, Vulpes vulpes"
0.0421 - "n02085620 Chihuahua"
```
cat_gray的结果如下
```
---------- Prediction for examples/images/cat_gray.jpg ----------
0.2792 - "n02326432 hare"
0.1485 - "n02441942 weasel"
0.1058 - "n02127052 lynx, catamount"
0.0777 - "n02120079 Arctic fox, white fox, Alopex lagopus"
0.0776 - "n02124075 Egyptian cat"
```

## 2.2 caffenet
```
./build/examples/cpp_classification/classification.bin \
  models/bvlc_alexnet/deploy.prototxt \
  models/bvlc_alexnet/bvlc_alexnet.caffemodel \
  data/ilsvrc12/imagenet_mean.binaryproto \
  data/ilsvrc12/synset_words.txt \
  examples/images/cat.jpg
输出
---------- Prediction for examples/images/cat.jpg ----------
0.3094 - "n02124075 Egyptian cat"
0.1761 - "n02123159 tiger cat"
0.1221 - "n02123045 tabby, tabby cat"
0.1132 - "n02119022 red fox, Vulpes vulpes"
0.0421 - "n02085620 Chihuahua"
```

## 2.3 vgg
```
./build/examples/cpp_classification/classification.bin \
  models/bvlc_alexnet/deploy.prototxt \
  models/bvlc_alexnet/bvlc_alexnet.caffemodel \
  data/ilsvrc12/imagenet_mean.binaryproto \
  data/ilsvrc12/synset_words.txt \
  examples/images/cat.jpg
```

# 参考

[]()
[bvlc_alexnet](https://github.com/BVLC/caffe/tree/master/models/bvlc_alexnet)

[Model-Zoo](https://github.com/BVLC/caffe/wiki/Model-Zoo)

[Very Deep Convolutional Networks for Large-Scale Visual Recognition](http://www.robots.ox.ac.uk/~vgg/research/very_deep/)

[Caffe学习系列(20)：用训练好的caffemodel来进行分类](http://www.cnblogs.com/denny402/p/5111018.html)
[官方文档](http://caffe.berkeleyvision.org/)
[Classifying ImageNet: using the C++ API](http://caffe.berkeleyvision.org/gathered/examples/cpp_classification.html)
[Classification: Instant Recognition with Caffe](http://nbviewer.jupyter.org/github/BVLC/caffe/blob/master/examples/00-classification.ipynb)


