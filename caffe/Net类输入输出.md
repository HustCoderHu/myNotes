文件caffe.cpp中 的 test 函数
函数  
``` 
void Solver<Dtype>::Solve(const char* resume_file)
{ 
  Step(param_.max_iter() - iter_); // line 286
}

void Solver<Dtype>::Step(int iters)
{
  for (int i = 0; i < param_.iter_size(); ++i) {  // line 209
      loss += net_->ForwardBackward();
  }
    loss /= param_.iter_size();
  // average the loss across iterations for smoothed reporting
  UpdateSmoothedLoss(loss, start_iter, average_loss)
}
```
iter_size 的解释
```
message SolverParameter {
  ...
// accumulate gradients over `iter_size` x `batch_size` instances
  optional int32 iter_size = 36 [default = 1];
  ...
```

ForwardBackward = forward + backward
函数
```
template <typename Dtype>
Dtype Net<Dtype>::ForwardFromTo(int start, int end) {
  Dtype loss = 0;
  for (int i = start; i <= end; ++i) {
    Dtype layer_loss = layers_[i]->Forward(bottom_vecs_[i], top_vecs_[i]);
    loss += layer_loss;
  }
  return loss;
}

/// top_vecs stores the vectors containing the output for each layer
  vector<vector<Blob<Dtype>*> > top_vecs_;

template <typename Dtype>
inline Dtype Layer<Dtype>::Forward(const vector<Blob<Dtype>*>& bottom,
    const vector<Blob<Dtype>*>& top) {
	Forward_cpu(bottom, top);
    for (int top_id = 0; top_id < top.size(); ++top_id) {
      if (!this->loss(top_id)) { continue; }
      const int count = top[top_id]->count();
      const Dtype* data = top[top_id]->cpu_data();
      const Dtype* loss_weights = top[top_id]->cpu_diff();
      loss += caffe_cpu_dot(count, data, loss_weights);
    }
}
```

Layer.loss 的含义
```
/** The vector that indicates whether each top blob has a non-zero weight in
   *  the objective function. */
  vector<Dtype> loss_;
```
关于 loss_weight
```
message LayerParameter {
// The amount of weight to assign each top blob in the objective.
  // Each layer assigns a default value, usually of either 0 or 1,
  // to each top blob.
  repeated float loss_weight = 5;
}
```

参考链接1的理解
```
loss := 0
for layer in layers:
  for top, loss_weight in layer.tops, layer.loss_weights:
    loss += loss_weight * sum(top)
```

net.cpp 的 init 函数中有如下一段注释
```
// Go through the net backwards to determine which blobs contribute to the
// loss.  We can skip backward computation for blobs that don't contribute
// to the loss.
// Also checks if all bottom blobs don't need backward computation (possible
// because the skip_propagate_down param) and so we can skip bacward
// computation for the entire layer
```

type带有loss的层 top blob 求和即为目标函数  
中间层也可以加 loss


# 参考
1. Caffe学习：Loss  
<http://blog.csdn.net/u011762313/article/details/47356285>  
2. Caffe官方教程中译本v1.0  
<http://caffecn.cn/?/page/tutorial>  
<http://pan.baidu.com/s/1c0Ri2Py>  
2. caffe中loss函数代码分析--caffe学习（16）  
<http://blog.csdn.net/u014381600/article/details/54340613>  