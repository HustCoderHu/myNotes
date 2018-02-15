在run过程中的集成一些操作，比如输出log，保存，summary 等

---
MonitoredSession 有个子类 tf.train.MonitoredTrainingSession  
看过实现之后，只是简单的包装了一下，Training这个参数比较多，有些我们可能根本不想用，  
但是会造成side effect ，所以直接用 这个基类

# 1 MonitoredSession

``` py
"""Session-like object that handles initialization, recovery and hooks."""
__init__(
    session_creator=None,
    hooks=None,
    stop_grace_period_secs=120
)
```
三个参数
- session_creator

  A factory object to create session. Typically a
        `ChiefSessionCreator` or a `WorkerSessionCreator`  
  SessionCreator 类型实例

- hooks

  An iterable of `SessionRunHook' objects  
  一般用list，可迭代，hook类型下面会列出来

- stop_grace_period_secs

  Number of seconds given to threads to stop after
        `close()` has been called  
  close 之后释放资源什么的我猜

官方例子
``` py
saver_hook = CheckpointSaverHook(...)
summary_hook = SummarySaverHook(...)
with MonitoredSession(session_creator=ChiefSessionCreator(...),
                      hooks=[saver_hook, summary_hook]) as sess:
  while not sess.should_stop():
    sess.run(train_op)
```

# 2 SessionRunHook
可以被 session 的各种操作触发相应的函数
## after_create_session
``` py
after_create_session(
    session,
    coord
)
```
ession 创建时会被调用, 和下面的begin有两个区别  
- 1 这个执行之后，图就确定了，不能再加op了
- 2 恢复被封装的session调用之后也会调用这个，不只是 整体的session

## before_run
```
before_run(run_context)
```
- run_context: SessionRunContext 实例

run()每次执行前都会被调用，可以返回None或者一个SessionRunArgs实例，传给run()。Arg可以包含op, tensor, feeds， 这些都会被run

## after_run
``` py
after_run(
    run_context,
    run_values
)
```
第1个参数同 before_run  
如果的before_run 返回了SessionRunArgs，请求的tensors会成为这里的 run_values  
如果run() 抛出异常，这个就不会执行

## begin 
```
begin()
```

## end 
```
end()
```
# 1 MonitoredTrainingSession
# 1.1 构造函数
``` py
MonitoredTrainingSession(
    master='',
    is_chief=True,
    checkpoint_dir=None,
    scaffold=None,
    hooks=None,
    chief_only_hooks=None,
    save_checkpoint_secs=600,
    save_summaries_steps=USE_DEFAULT,
    save_summaries_secs=USE_DEFAULT,
    config=None,
    stop_grace_period_secs=120,
    log_step_count_steps=100,
    max_wait_secs=7200
)
```
官方例子
```
saver_hook = CheckpointSaverHook(...)
summary_hook = SummarySaverHook(...)
with MonitoredSession(session_creator=ChiefSessionCreator(...),
                      hooks=[saver_hook, summary_hook]) as sess:
  while not sess.should_stop():
    sess.run(train_op)
```
首先，当MonitoredSession初始化的时候，会按顺序执行下面操作：
- 调用hook的begin()函数，我们一般在这里进行一些hook内的初始化。比如在上面猫狗大战中的_LoggerHook里面的_step属性，就是用来记录执行步骤的，但是该参数只在本类中起作用。
- 通过调用scaffold.finalize()初始化计算图
创建会话
- 通过初始化Scaffold提供的操作(op)来初始化模型
- 如果checkpoint存在的话，restore模型的参数
- launches queue runners
- 调用hook.after_create_session()

然后，当run()函数运行的时候，按顺序执行下列操作：

- 调用hook.before_run()
- 调用TensorFlow的 session.run()
- 调用hook.after_run()
- 返回用户需要的session.run()的结果
- 如果发生了AbortedError或者UnavailableError，则在再次执行run()之前恢复或者重新初始化会话

最后，当调用close()退出时，按顺序执行下列操作：

- 调用hook.end()
- 关闭队列和会话
- 阻止OutOfRange错误

## 1.2 Hook
所以这些钩子函数就是重点关注的对象  
### .1 LoggingTensorHook
tf.train.LoggingTensorHook 官方说明  
* Prints the given tensors every N local steps, every N seconds, or at end.
```
__init__(
    tensors,
    every_n_iter=None,
    every_n_secs=None,
    formatter=None
)
```
- tensors: dict that maps string-valued tags to tensors/tensor names, or iterable of tensors/tensor names.

用法举例  
```
# Set up logging for predictions
  tensors_to_log = {"probabilities": "softmax_tensor"}
  logging_hook = tf.train.LoggingTensorHook(
      tensors=tensors_to_log, every_n_iter=50)
```

### .2 SummarySaverHook
tf.train.SummarySaverHook
* Saves summaries every N steps
```
__init__(
    save_steps=None,
    save_secs=None,
    output_dir=None,
    summary_writer=None,
    scaffold=None,
    summary_op=None
)
```
output_dir 填 路径  
summary_op 填 tf.summary.merge_all

### .3 CheckpointSaverHook
tf.train.CheckpointSaverHook  
MonitoredTrainingSession 只有 save_checkpoint_secs, 没有按step保存的选项
* Saves checkpoints every N steps or seconds
```
__init__(
    checkpoint_dir,
    save_secs=None,
    save_steps=None,
    saver=None,
    checkpoint_basename='model.ckpt',
    scaffold=None,
    listeners=None
)
```
必填 saver, save_secs 或者 save_steps

### .4 NanTensorHook
tf.train.NanTensorHook  
感觉是用来调试的，加到训练过程中可能会拖慢train  

* Monitors the loss tensor and stops training if loss is NaN.
Can either fail with exception or just stop training.
```
__init__(
    loss_tensor,
    fail_on_nan_loss=True
)
```

### .5 FeedFnHook
tf.train.FeedFnHook  
看着像用来产生 feed_dict
* Runs feed_fn and sets the feed_dict accordingly
```
__init__(feed_fn)
```

### .6 GlobalStepWaiterHook
tf.train.GlobalStepWaiterHook  
分布式用  

### .7 ProfilerHook
tf.train.ProfilerHook
* This hook delays execution until global step reaches to wait_until_step. It is used to gradually start workers in distributed settings. One example usage would be setting wait_until_step=int(K*log(task_id+1)) assuming that task_id=0 is the chief

# reference
tf.train.MonitoredSession  
<https://www.tensorflow.org/versions/master/api_docs/python/tf/train/MonitoredSession>  
tf.train.SessionRunHook  
<https://www.tensorflow.org/api_docs/python/tf/train/SessionRunHook>  
resnet_main.py  
<https://github.com/tensorflow/models/blob/master/research/resnet/resnet_main.py>  
tf.train.MonitoredTrainingSession
<https://www.tensorflow.org/versions/master/api_docs/python/tf/train/MonitoredTrainingSession>  
使用自己的数据集进行一次完整的TensorFlow训练  
<https://zhuanlan.zhihu.com/p/32490882>  
tf.train.LoggingTensorHook  
<https://www.tensorflow.org/api_docs/python/tf/train/LoggingTensorHook>  
tf.train.SummarySaverHook  
<https://www.tensorflow.org/versions/master/api_docs/python/tf/train/SummarySaverHook>  
tf.train.CheckpointSaverHook  
<https://www.tensorflow.org/versions/master/api_docs/python/tf/train/CheckpointSaverHook>  
tf.train.NanTensorHook  
<https://www.tensorflow.org/versions/master/api_docs/python/tf/train/NanTensorHook#__init__>