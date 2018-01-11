使用 
---
# 1 安装 protoc
## win
下面的url下载 protoc-x.x-win32，解压之后把`bin`文件夹添加到`PATH`
https://github.com/google/protobuf/releases  
protobuf-python-x.x是给py用的一些pkg

## linux
TODO

# 2 编译TF的proto
```
cd models/research
protoc object_detection/protos/*.proto --python_out $TARGET_PATH
# 测试安装
export PYTHONPATH=/e/github_repo/models/research:/e/github_repo/models/research/slim:$PYTHONPATH
python object_detection/builders/model_builder_test.py
```

# 3 安装 PIL matplotlib等
PIL 只支持到Python 2.7  
Pillow是PIL的一个派生分支，但如今已经发展成为比PIL本身更具活力的图像处理库
```
pip3 install Pillow
```

# 4

# 导出用来的推断的模型


# 参考
Python下使用protobuf on Windows  
<https://www.jianshu.com/p/0c563b2c0fdb>  
python3 怎么安装 PIL  
<http://blog.csdn.net/dcz1994/article/details/71642979>  

Exporting a trained model for inference  
<https://github.com/tensorflow/models/blob/master/research/object_detection/g3doc/exporting_models.md>  
Object Detection Demo  
<https://github.com/tensorflow/models/blob/master/research/object_detection/object_detection_tutorial.ipynb>  

Tensorflow物体检测（Object Detection）API的使用  
<http://www.cnblogs.com/arkenstone/p/7237292.html>  