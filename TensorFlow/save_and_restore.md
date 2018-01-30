保存和恢复模型
---
# 1 saved_model
## API
```
class tf.saved_model.builder.SavedModelBuilder

# 初始化方法
__init__(export_dir)

# 导入graph与变量信息 
add_meta_graph_and_variables(
    sess,
    tags,
    signature_def_map=None,
    assets_collection=None,
    legacy_init_op=None,
    clear_devices=False,
    main_op=None
)

# 载入保存好的模型
tf.saved_model.loader.load(
    sess,
    tags,
    export_dir,
    **saver_kwargs
)
```
## 使用
### 保存
```
builder = tf.saved_model.builder.SavedModelBuilder(saved_model_dir)
builder.add_meta_graph_and_variables(sess, ['tag_string'])
builder.save()
```

### 载入
```
meta_graph_def = tf.saved_model.loader.load(sess, ['tag_string'], saved_model_dir)
```

# 2 输入输出 Tensor
使用 SignatureDef  
```
# 构建signature
tf.saved_model.signature_def_utils.build_signature_def(
    inputs=None,
    outputs=None,
    method_name=None
)

# 构建tensor info 
tf.saved_model.utils.build_tensor_info(tensor)
```

## API
```
# 构建signature
tf.saved_model.signature_def_utils.build_signature_def(
    inputs=None,
    outputs=None,
    method_name=None
)

# 构建tensor info 
tf.saved_model.utils.build_tensor_info(tensor)
```

## 使用
### 保存
```
builder = tf.saved_model.builder.SavedModelBuilder(saved_model_dir)
# x 为输入tensor, keep_prob为dropout的prob tensor 
inputs = {'input_x': tf.saved_model.utils.build_tensor_info(x), 
            'keep_prob': tf.saved_model.utils.build_tensor_info(keep_prob)}

# y 为最终需要的输出结果tensor 
outputs = {'output' : tf.saved_model.utils.build_tensor_info(y)}

signature = tf.saved_model.signature_def_utils.build_signature_def(inputs, outputs, 'test_sig_name')

builder.add_meta_graph_and_variables(sess, ['test_saved_model'], 
    assets_collection={'test_signature':signature})
builder.save()
```

### 载入
与使用的代码如下
```
## 略去构建sess的代码

signature_key = 'test_signature'
input_key = 'input_x'
output_key = 'output'

meta_graph_def = tf.saved_model.loader.load(sess, ['test_saved_model'], saved_model_dir)
# 从meta_graph_def中取出SignatureDef对象
signature = meta_graph_def.signature_def

# 从signature中找出具体输入输出的tensor name 
x_tensor_name = signature[signature_key].inputs[input_key].name
y_tensor_name = signature[signature_key].outputs[output_key].name

# 获取tensor 并inference
x = sess.graph.get_tensor_by_name(x_tensor_name)
y = sess.graph.get_tensor_by_name(y_tensor_name)

# _x 实际输入待inference的data
sess.run(y, feed_dict={x:_x})
```

# 参考
TensorFlow saved_model 模块  
<http://blog.csdn.net/thriving_fcl/article/details/75213361>  