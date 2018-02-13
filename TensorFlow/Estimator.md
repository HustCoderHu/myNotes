Estimator tf中的一大块，定义一套训练评估模型的模式

---
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
  model.train(input_fn= lambda : train_input_fn(), steps= eval_interval)
  model.evaluate(input_fn= lambda : eval_input_fn(), steps= eval_iter)
```

关键内容就是下面俩，我会参考官方内详细讲下
- input_fn: 输入函数，返回两个参数，塞给模型函数的前两个形参，  
训练和评估用的数据来源不一样，所以有俩
- model_fn: 模型函数，内部有模型的输出，loss，精度等计算过程，  
会接受一个mode参数，代表训练、评估还是预测

# 1 Write an Input function
定义函数或者lambda都行，输入参数随便来，后门这个函数作为参数时我们用lambda包一下，  
顺便把需要的参数也给它  
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

# 2 Create feature columns
描述model如何使用feature，feature比较复杂的时候要用到，  
比如输入从tfrecord里来的，后面要用到我再看看
``` py
# Feature columns describe how to use the input.
my_feature_columns = []
for key in train_x.keys():
    my_feature_columns.append(tf.feature_column.numeric_column(key=key))
```

# 3 Write a model function
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
  
  if (mode == tf.estimator.ModeKeys.TRAIN or
      mode == tf.estimator.ModeKeys.EVAL):
    loss = ...
    acc = ...
  else :
    loss = None
  if mode == tf.estimator.ModeKeys.TRAIN :
    train_op = ...
  elif mode == tf.estimator.ModeKeys.EVAL
    train_op = None
    predictions = None
  else :
    train_op = None
    predictions = ...
  
   spec = tf.estimator.EstimatorSpec(
       mode= mode,
       predictions= predictions,
       loss= loss,
       train_op= train_op)
   return spec
```
前两个参数是上面input函数返回的，mode区分train、eval、predict，取值如下
invoke | mode value
-|-
Estimator.train() | ModeKeys.TRAIN
Estimator.evaluate() | ModeKeys.EVAL
Estimator.predict() | ModeKeys.PREDICT

# param

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

<https://stackoverflow.com/questions/48241370/estimator-train-with-num-steps-in-tensorflow?rq=1>