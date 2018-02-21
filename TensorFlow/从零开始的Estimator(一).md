Estimator tf中的一大块，定义了一套训练评估模型的模式。训练过程用到了  
MonitoredTrainingSession

---
文章组织如下
- 1 overview
- 2 Write an Input function
- 3 Create feature columns
- 4 Write a model function
- 5 build Estimator
- 6 train & evaluate
- reference

predict 下一篇再说

# 1 overview
为了训练和评估的代码看起来比较短，G整出了这个玩意。  
把输入输出，图表等一堆操作全放在函数里，然后把这个操作传给一个类，  
后面我们要操作，调用这个类的函数就好了，鄙人称之为"套路"。

自己写个也是可以的，不过就重复造轮子了

那些函数定义好了之后，整体的代码看起来就是下面这个样子
``` py
# 这个conf就是原来Session()的config，还有些ckpt路径之类的
run_conf = tf.estimator.RunConfig(...)
classifier = tf.estimator.Estimator(
    model_fn=model_fn, model_dir=xxx, config=run_conf,
      params={
          'w': w,
          't': t,
          'f': f
      })
while cond:
  classifier.train(input_fn= lambda : train_input_fn(), steps= eval_interval)
  classifier.evaluate(input_fn= lambda : eval_input_fn(), steps= eval_iter)
```

需要完成的内容, 我会参考官方内详细讲下
- model_fn

  模型函数，内部有模型的输出，loss，精度等计算过程，  
会接受一个mode参数，代表训练、评估还是预测

- input_fn

  输入函数，返回两个参数，塞给模型函数的前两个形参，  
训练和评估用的数据来源不一样，所以有俩

- params

  一个dict，会直接传给model_fn

- run_conf

运行配置


# 2 Write an Input function
输入参数随便来，后门这个函数作为参数时我们用lambda包一下，  顺便把需要的参数也给它  
官网描述要返回一个pair，官方例子如下
``` py
def train_input_fn(features, labels, batch_size):
    """An input function for training"""
    # Convert the inputs to a Dataset.
    dataset = tf.data.Dataset.from_tensor_slices((dict(features), labels))

    # Shuffle, repeat, and batch the examples.
    dataset = dataset.shuffle(1000).repeat().batch(batch_size)

    # Return the read end of the pipeline.
    return dataset.make_one_shot_iterator().get_next()
```
这是train的输入，eval的输入类似  
例子里用了Dataset，相关内容我另一篇里讲了

**注意**
生成Dataset的时候会用到一些函数，比如from_tensor_slices 或者Fromtfrecord，  
这些操作要放在input_fn里，否则会出现以下的错误  
```
ValueError: Input graph and Layer graph are not the same: Tensor("IteratorGetNext:0", shape=(?, ?, ?, ?), dtype=float32) is not from the passed-in graph.
```
原因就是建立Dataset 的时候有些tensor 或者op 不在这个train()函数的图里
参考 <https://github.com/tensorflow/tensorflow/issues/8042>  
<https://github.com/tensorflow/tensorflow/issues/4026>


# 3 Create feature columns
描述model如何使用feature，feature比较复杂的时候要用到，  
比如输入从tfrecord里来的，后面要用到我再看看
``` py
# Feature columns describe how to use the input.
my_feature_columns = []
for key in train_x.keys():
    my_feature_columns.append(tf.feature_column.numeric_column(key=key))
```

# 4 Write a model function
模型函数，所有的操作应该都放这里，当然网络的构造过程可以自己再写个类，  
只暴露输入输出，这样代码结构更清晰。  
函数形式如下
``` py
def my_model_fn(
  features, # This is batch_features from input_fn
  labels,   # This is batch_labels from input_fn
  mode,     # An instance of tf.estimator.ModeKeys, see below
  params):  # Additional configuration
  ...
  model = MyModel(data_format='NHWC')
  logits = MyModel(features,
        training = (mode == tf.estimator.ModeKeys.TRAIN) )
  _pred = tf.argmax(logits, axis=1, output_type=tf.int32)
  
  train_op = None
  if (mode == tf.estimator.ModeKeys.TRAIN or
      mode == tf.estimator.ModeKeys.EVAL):
    # label 不是 one hot
    loss = tf.losses.sparse_softmax_cross_entropy(
        labels=labels, logits=logits)
    acc_mat = tf.equal(labels, _pred)
    acc = tf.reduce_mean(tf.cast(acc_mat, tf.float32))
    # tf.summary.scalar('accuracy', acc)
    predictions = None
    export_outputs = None
  else :
    loss = None

  if mode == tf.estimator.ModeKeys.TRAIN :
    train_op = tf.train.AdamOptimizer(lr).minimize(
      loss, tf.train.get_or_create_global_step()
    )
  elif mode == tf.estimator.ModeKeys.PREDICT
    predictions = {
      'classes': _pred
      'prob': tf.nn.softmax(logits, name='softmax_tensor')
    }
    export_outputs = {
      'classify': tf.estimator.export.PredictOutput(predictions)
    }
  # end main body before return
  spec = tf.estimator.EstimatorSpec(
    mode= mode, predictions= predictions,
    loss= loss, train_op= train_op,
    export_outputs= export_outputs
  )
  return spec
```
总体来说就是根据mode 给出3种模式下需要的tensor和op，最后返回一个EstimatorSpec。下面展开各个细节

## 输入参数
前两个参数是上面input函数返回的，mode区分train、eval、predict，取值如下

invoke | mode value
-|-
Estimator.train() | ModeKeys.TRAIN
Estimator.evaluate() | ModeKeys.EVAL
Estimator.predict() | ModeKeys.PREDICT

params 是个dict，构造Estimator时传进来的，上面说过

## 函数主体
train和eval阶段都要loss和accuracy，此外train阶段要加一个train_op，  
predict阶段对网络的输出logits取一下softmax，给出预测值的prob(概率)  
如果要top-k的话根据情况加代码


loss 被作为参数初始化了 EstimatorSpec，就被自动加到summary了  
accuracy 官方例子里的 metric 计算的不是当前batch的，而是把前面的都累积了。  
如果只想看当前的就得自己加summary，见reference最后一个

## 返回值
返回了个 EstimatorSpec 实例
``` py
@staticmethod
__new__(
    cls,
    mode,
    predictions=None,
    loss=None,
    train_op=None,
    eval_metric_ops=None,
    export_outputs=None,
    training_chief_hooks=None,
    training_hooks=None,
    scaffold=None,
    evaluation_hooks=None
)
```
`__new__` 这个也有不少内容，先当成`__init__` 吧。常用参数如下  
- cls：先当成self，构造的时候忽略
- mode：上面讲过了，直接把 model_fn 的参数填这里就好

- predictions

  可以放tensor 或者 dict，比如 类型->预测概率  
  ``` py
  predictions = {
      'classes': _pred
      'prob': tf.nn.softmax(logits, name='softmax_tensor')
  }
  ```
- loss： 计算得到的loss，上面代码里有
- train_op： 同上
- eval_metric_ops： 需要累积精度就用这个，参考官方例子
- export_outputs

  给输出加个名，详细内容需要看 SavedModel，  
  作用是是恢复时方便使用，否则还要根据tensor名字找输入输出

``` py
export_outputs={
  'classify': tf.estimator.export.PredictOutput(predictions)
}
```

# 5 build Estimator
前面的准备工作完成了，我们开始写文章开头的那几行代码，先构造个Estimator
``` py
run_conf = tf.estimator.RunConfig(...)
classifier = tf.estimator.Estimator(
    model_fn=model_fn, model_dir=xxx, config=run_conf,
      params={
          'w': w,
          't': t,
          'f': f
      })
```
## RunConfig
类 tf.estimator.RunConfig ，实例可以用来构造 Estimator
``` py
__init__(
    model_dir=None,
    tf_random_seed=None,
    save_summary_steps=100,
    save_checkpoints_steps=_USE_DEFAULT,
    save_checkpoints_secs=_USE_DEFAULT,
    session_config=None,
    keep_checkpoint_max=5,
    keep_checkpoint_every_n_hours=10000,
    log_step_count_steps=100
)
```


- model_dir

  ckpt和log的文件夹，Estimator的构造函数也有一个，填一个就ok
- save_summary_steps

  每n steps 保存一次summary
- save_checkpoints_steps

  同上 不过是ckpt
- save_checkpoints_secs

  和上面不能同时设，如果都空着，就默认600s保存一次，如果和上面一起设None就不保存
- session_config
  
  经常用的那个 session 的配置 tf.ConfigProto(), 可以设
  显存使用比例等
- keep_checkpoint_max

  新ckpt产生，旧的就会被删，这个参数意思是保留最近 n 个ckpt，  
  None或0 是保存所有，默认是 5
- keep_checkpoint_every_n_hours

  Number of hours between each checkpoint to be saved. The default value of 10,000 hours effectively disables the feature.  
  没太看懂，用上面的 save_checkpoints_* 吧
- log_step_count_steps

  n steps 计算一次 step/sec

## Estimator
类 tf.estimator.Estimator
``` py
__init__(
    model_fn,
    model_dir=None,
    config=None,
    params=None
)
```
- RunConfig里填了这里就可以空着
- config 就是上面的 RunConfig对象
- params 一个dict，传给model_fn


# 6 train & evaluatetrain & evaluate
我们一般train一段时间之后就评估下模型效果，否则发生过拟合我们都不知道  
Estimator 本身好像没提供周期eval的特性，不过 tf.contrib.learn.Experiment 这个类提供了
函数 tf.estimator.train_and_evaluate
``` py
__init__(
    estimator,
    train_input_fn,
    eval_input_fn,
    eval_metrics=None,
    train_steps=None,
    eval_steps=100,
    train_monitors=None,
    eval_hooks=None,
    local_eval_frequency=None,
    eval_delay_secs=120,
    continuous_eval_throttle_secs=60,
    min_eval_frequency=None,
    delay_workers_by_global_step=False,
    export_strategies=None,
    train_steps_per_iteration=None,
    checkpoint_and_export=False,
    saving_listeners=None
)
```
3 个参数
- estimator
- train_spec

  训练相关的配置，包括 input_fn max_step 等
- eval_spec

  eval相关的配置，包括 input_fn, step
 
train 和 evaluate 的steps参数，可以从训练或者eval循环里跳出来
``` py
while cond:
  classifier.train(input_fn= lambda : train_input_fn(), steps= eval_interval)
  classifier.evaluate(input_fn= lambda : eval_input_fn(), steps= eval_iter)
```
## train
``` py
train(
    input_fn,
    hooks=None,
    steps=None,
    max_steps=None,
    saving_listeners=None
)
```
常用的就几个
- input_fn： 上面讲了
- hooks

  SessionRunHook 构成的 list，train 循环里触发
- steps, max_steps

  两个只能设一个，都可以让循环达到 n steps 时跳出。举个例子说明区别  
  train(steps=n) 和 train(max_steps=n) 分别执行两次，  
  前者迭代了 2n 次，而后者除了第一次的 n 个batch迭代了，后面的都没有效果

## evaluate
``` py
evaluate(
    input_fn,
    steps=None,
    hooks=None,
    checkpoint_path=None,
    name=None
)
```
input_fn, steps, hooks 和上面train的一样
- checkpoint_path

  要eval的ckpt路径，默认是上面配置的`model_dir`
- name

  一个str，可以用来区分不同数据集(输入)的eval过程，日志目录里会形成若干个子目录，  
  tensorboard里也是独立显示

# reference
Creating Custom Estimators  
<https://www.tensorflow.org/get_started/custom_estimators>  
<https://github.com/tensorflow/tensorflow/blob/master/tensorflow/docs_src/get_started/custom_estimators.md>  
tf.estimator.Estimator  
<https://www.tensorflow.org/api_docs/python/tf/estimator/Estimator>  
tf.estimator.EstimatorSpec  
<https://www.tensorflow.org/api_docs/python/tf/estimator/EstimatorSpec>  
官方的mnist例子  
<https://github.com/tensorflow/models/blob/master/official/mnist/mnist.py>  
resnet 例子  
<https://github.com/tensorflow/models/blob/master/official/resnet/resnet.py>  
只看一个batch的accuracy  
<https://stackoverflow.com/questions/48241370/estimator-train-with-num-steps-in-tensorflow?rq=1>