ʹ�� 
---
# 1 ��װ protoc
## win
�����url���� protoc-x.x-win32����ѹ֮���`bin`�ļ�����ӵ�`PATH`
https://github.com/google/protobuf/releases  
protobuf-python-x.x�Ǹ�py�õ�һЩpkg

## linux
TODO

# 2 ����TF��proto
```
cd models/research
protoc object_detection/protos/*.proto --python_out $TARGET_PATH
# ���԰�װ
export PYTHONPATH=/e/github_repo/models/research:/e/github_repo/models/research/slim:$PYTHONPATH
python object_detection/builders/model_builder_test.py
```

# 3 ��װ PIL matplotlib��
PIL ֻ֧�ֵ�Python 2.7  
Pillow��PIL��һ��������֧��������Ѿ���չ��Ϊ��PIL������߻�����ͼ�����
```
pip3 install Pillow
```

# 4

# �����������ƶϵ�ģ��


# �ο�
Python��ʹ��protobuf on Windows  
<https://www.jianshu.com/p/0c563b2c0fdb>  
python3 ��ô��װ PIL  
<http://blog.csdn.net/dcz1994/article/details/71642979>  

Exporting a trained model for inference  
<https://github.com/tensorflow/models/blob/master/research/object_detection/g3doc/exporting_models.md>  
Object Detection Demo  
<https://github.com/tensorflow/models/blob/master/research/object_detection/object_detection_tutorial.ipynb>  

Tensorflow�����⣨Object Detection��API��ʹ��  
<http://www.cnblogs.com/arkenstone/p/7237292.html>  