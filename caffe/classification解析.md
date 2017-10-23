```
Classifier 对象构造 {
  载入模型和参数
  检查网络输入输出个数
  检查数据通道数
  记录网络期望的输入数据的尺寸
  载入均值
  载入labels
  检查网络输出数据通道数 (应该和labels相等)
}
imread读取图像
Classify {
  Predict {
    配置输入层参数 input_layer->Reshape(图片数 = 1, channels, height, width)
    /* Forward dimension change to all layers. */ 参数配置还可以forward？要看net->Reshape的实现代码了
    WrapInputLayer {
      用input_data初始化
    }
    Preprocess {
      
    }
    net->Forward()
    /* Copy the output layer to a std::vector */
  }
  找出前 N 大的预测结果(label 的标号)
  将label str 和 预测概率存入vector
  return vec
}

输出 top N 预测结果
```