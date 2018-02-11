有时候文件做成的tfrecord可能会有几十G，想先直接从众多的图像文件构建Dataset ?  
下面的内容应该能帮到你

---
简介看下面参考文献第一个，他讲的很清楚了  

本篇笔记脉络如下
- 1 创建
- 2 变换
- 3 获取 样本Tensors
- 4 使用
- 5 Performance Considerations
- reference

# 1 创建
因为图像很多，我们不能将图像全部读到Dataset里，所以这一步我们先在Dataset里存放所有文件路径
```
filelist = os.listdir(img_dir)
# lable_list = ... # 标签列表根据自己的情况获取
# 两个tensor
t_flist = tf.constant(filelist)
t_labellist = tf.constant(lable_list)
# 构造 Dataset
dataset = tf.data.Dataset().from_tensor_slices(t_flist, t_labellist)
```
至此构造完成，set里面就是一个个样本 [file, label]

# 2 变换
常用的有这么几个，逐一介绍
- cache
- map
- shuffle
- repeat
- batch
- prefetch

最后给出常用组合

## cache
将dataset缓存在内存或者本地硬盘
```
cache(filename='')
```
就一个参数 tf.string类型的tf.Tensor，表示文件系统里的一个文件路径，默认是内存

## map
```
map(
    map_func,
    num_parallel_calls=None
)
```
两个参数
- 第一个map函数，用来变换样本
- 并发数，一般填cpu线程数

针对上面的图像，我们需要将 [file, label] 变换到 [img_data, label]，所以函数形式如下
```
def _mapfunc(file, label):
  with tf.device('/cpu:0'):
    img_raw = tf.read_file(file)
    decoded = tf.image.decode_bmp(img_raw)
    resized = tf.image.resize_images(decoded, [h, w])
  return resized, label
  # 上面return还可以加上文件名，这里的个数随意
  # 下面 Iter.get_next().run 返回的每个成员和这里对应
```
GPU和CPU异步处理整个计算图，GPU集中于各种大数据量的计算，所以图像解码的任务交给CPU，这是google的建议

![](https://raw.githubusercontent.com/HustCoderHu/myStorage/master/parallel_map.png)

另外官方有个推荐如果batch是百或千数量级，就用map_and_batch，样例代码如下
```
dataset = dataset.apply(tf.contrib.data.map_and_batch(
    map_func=parse_fn, batch_size=FLAGS.batch_size))
```
看map_and_batch说明是 map -> batch，而且后面可能会被移除  
说是会优化成：map和batch邻接时自动优化
## shuffle
打乱set里的样本，各种样本混在一起后必须要的操作
```
shuffle(
    buffer_size,
    seed=None,
    reshuffle_each_iteration=None
)
```
reshuffle_each_iteration 默认 True  
buffer_size 比样本数+1

## repeat
一个参数，重复的次数  
重复整个Dataset，实际并没有重复(我猜是这么实现的)，就是用来控制 Iterator 的边界

## batch
将多个元素组成batch，batch size 64 为例
```
dset = dataset.batch(64)
```
对应上面的map函数，Iter.get_next().run 返回的结果会变成
`(resized[64], label[64])`  
这里的64意思是 tensor.shape[0] == 64 ( tensor in [resized, label] )  

如果map函数 return 多了个文件名 run的返回就是
`(resized[64], label[64], fname[64])`  

## prefetch
就一个参数，预取的元素数量  
官方言：在输入pipeline的末尾执行这个，掩盖IO的时间，如果IO时间占比较大，效果就会很明显  
![](https://raw.githubusercontent.com/HustCoderHu/myStorage/master/no_prefetch.png)  
<center>不用就是串行</center>

![](https://raw.githubusercontent.com/HustCoderHu/myStorage/master/prefetch.png)  
<center>用了就是爽</center>

我觉得应该是预取到内存里了，同时n应该有个临界值，  
只要时间上 `train n > prefetch` 就掩盖所有的红色区域了，否则n大了还占内存

## 常用组合
```
dset = dataset.map(_mapfunc)
dset = dataset.cache() # 内存能装下就上
# 我猜后面的prefetch可能都免了
dset = dset.shuffle(n_example+1, reshuffle_each_iteration=False)
dset = dset.repeat(n_epochs)
dset = dset.batch(batch_sz)
dset = dset.prefetch(n) # n个batch了 注意
```

# 3 获取 样本Tensors
先是迭代器
- make_one_shot_iterator()
- make_initializable_iterator(shared_name=None)  

还有俩没列出来，用到的时候再加进来吧  

然后是 样本Tensors
```
get_next(name=None)
```
* Returns a nested structure of tf.Tensors containing the next element.

样例代码
```
_iter = dset.make_one_shot_iterator()
next_one = _iter.get_next() # type: tuple
```
# 4 使用
把next_one 当成 tensor 开始构建图  
比如算个全局平均池化
```
img, label = next_one
# tf 1.4 这个tensor没有dim的信息，各种op都报dim的错误
# 必须加这个reshape
out = tf.reshape(img, [-1, h, w, c])
out = tf.reduce_mean(out, axis=(1, 2)) # channels last
# out shape: (n, c)
```

下面的代码来自google，每个epoch后可以统计些信息
注意如果batch不能整除整个数据集，末尾就会有些数据漏掉
```
# Compute for 100 epochs.
for _ in range(100):
  sess.run(iterator.initializer) #　 one shot 不需要这句
  while True:
    try:
      sess.run(out)
    except tf.errors.OutOfRangeError:
      break
  
  # [Perform end-of-epoch calculations here.]
```

# 5 Performance Considerations
上面那些变换很好用，但是顺序对性能有些影响
## Map and Batch
map用到了自定义函数，如果很短就会导致调用开销(overhead)大，所以推荐batch之后map  
不过map函数要处理batch，就要加循环了，针对我上面的map，我只能想到用tf.while_loop

## Map and Cache
官方言：如果map函数开销大，只要你内存或者硬盘放得下，就map -> cache。  
因为不用cache时，这个map每次都在GPU需要前计算，如果mapfunc开销大，  
确实会拖慢GPU的脚步，提前算好并缓存能减少突发需求的等待时间  
不过上面prefetch里也提到了，n达到一定值就够了，不需要全部。  
这个Cache的作用可能会沦为减少频繁的硬盘读

## Map and Interleave / Prefetch / Shuffle
后面仨会一直占用一块缓存(缓存了dataset里的元素)，如果map改变了样本的大小，  
这个顺序就会影响内存使用

## Repeat and Shuffle
官方推荐用 tf.contrib.data.shuffle_and_repeat，不行就  
shuffle -> repeat

# reference
TensorFlow全新的数据读取方式：Dataset API入门教程  
<https://zhuanlan.zhihu.com/p/30751039>  
Importing Data  
<https://www.tensorflow.org/programmers_guide/datasets>  
Module: tf.data  
<https://www.tensorflow.org/versions/master/api_docs/python/tf/local_variables>  
Introduction to TensorFlow Datasets and Estimators  
<https://developers.googleblog.com/2017/09/introducing-tensorflow-datasets.html>  
Input Pipeline Performance Guide  
https://github.com/tensorflow/tensorflow/blob/master/tensorflow/docs_src/performance/datasets_performance.md