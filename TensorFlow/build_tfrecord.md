制作 tfrecord

---
会用到类 tf.python_io.TFRecordWriter
* A class to write records to a TFRecords file.

这个函数实现了 `__enter__`和`__exit__` 所以可以和一般文件一样搭配with用

构造函数有两个参数
```
__init__(
    path,
    options=None
)
```
- path 是record文件路径
- options可以填入 `tf.python_io.TFRecordOptions` 对象，1.4版本时只有个压缩算法，  
下面代码里可以看到
## 1 open writer
下面是打开writer的代码
```
# compression_type = tf.python_io.TFRecordCompressionType.NONE
compression_type = tf.python_io.TFRecordCompressionType.ZLIB
compression_type = tf.python_io.TFRecordCompressionType.GZIP
# GZIP 就是常见的GZIP 压缩，在win上可以用7z看到里面的文件和不压缩时的大小一样，
#估计内容也是一样的，压缩不知道会不会影响读取时的速度
options_ = tf.python_io.TFRecordOptions(compression_type)
with tf.python_io.TFRecordWriter(path, options_) as writer:
  ...
  # end
```
## 2 准备 Example
别人自己写的代码看得我头晕，官方的代码也只是简单的用了下，缺少总结，  
直接看proto的定义我才彻底弄清楚

首先大概的过程就是
- 每次写入一个example 循环这个过程，把所有数据都写进tfrecord文件

核心点在 **tf.train.Example** 以及它用到的一系列protobuf定义的类型上
```
message Example {
  Features features = 1;
};
message SequenceExample {
  Features context = 1;
  FeatureLists feature_lists = 2;
};

message Features {
  // Map from feature name to feature.
  map<string, Feature> feature = 1;
};
// Containers for non-sequential data.
message Feature {
  // Each feature can be exactly one kind.
  oneof kind {
    BytesList bytes_list = 1;
    FloatList float_list = 2;
    Int64List int64_list = 3;
  }
};
// Containers to hold repeated fundamental values.
message BytesList {
  repeated bytes value = 1;
}
message FloatList {
  repeated float value = 1 [packed = true];
}
message Int64List {
  repeated int64 value = 1 [packed = true];
}
```
也就是说 Example 其实就是个map

SequenceExample 我还没用，应该类似

样本有时可能是文本，数字之类的，不单单只有图像。文本我暂时用不到，  
后面用到再加上，这里先来图像的
```
int64_lda = lambda val : tf.train.Feature(
  int64_list=tf.train.Int64List(value=[val]) )
bytes_lda = lambda val : tf.train.Feature(
  bytes_list=tf.train.BytesList(value=[val]) )
```
分别产生标签和图像数据的 feature，map的value部分
```
# data = cv2.imread(file) # 通道顺序 BGR
feature_map = {
    "image": byte_lda(data.tobytes()), # tobytes() type: <class 'bytes'>
    "label": int64_lda(int(label))
}
features_ = tf.train.Features(feature = feature_map)
example = tf.train.Example(features = features_)
```
# 3 写入
暂时没看到有什么要注意的，就一句
```
writer.write(example.SerializeToString())
```

# LAST
开始的时候提到过压缩，GZIP的压缩比其实一般，如果是为了方便携带，制作的时候就不压缩，  
做好之后用xz算法压，我测试的情况是11张图

压缩|体积(MB)
-|-
None|27.8
GZIP|20.7
xz|13.5

不过xz有点费时间，谨慎使用

# 参考
官方example  
<https://github.com/tensorflow/tensorflow/blob/master/tensorflow/examples/how_tos/reading_data/convert_to_records.py>  
example.proto  
<https://github.com/tensorflow/tensorflow/blob/master/tensorflow/core/example/example.proto>  
feature.proto  
<https://github.com/tensorflow/tensorflow/blob/master/tensorflow/core/example/feature.proto>  
feature.proto
YJango：TensorFlow中层API Datasets+TFRecord的数据导入  
<https://zhuanlan.zhihu.com/p/33223782>  
tensorflow TFRecords文件的生成和读取方法  
<https://zhuanlan.zhihu.com/p/31992460>  
tf.data是如何支持多线程的？为何会比基于queue的batch方案慢4到5倍？  
<https://www.zhihu.com/question/67668892>  
Performance Guide  
<https://www.tensorflow.org/performance/performance_guide>