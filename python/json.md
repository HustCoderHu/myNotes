python 的某些类型可以与json类型之间转换

---

# 常用方式
字典类型导出到文件和从文件导入

``` py
import json
# Writing JSON data
data = {1: 'aa', 2: 'bb', 3: 'cc'}
with open('data.json', 'w') as f:
  json.dump(data, f)
# Reading data back
with open('data.json', 'r') as f:
  data = json.load(f)
# data 的 key 类型会变成 str
# int(k) 转换回 int
```

# 格式化
``` py
from pprint import pprint, pformat
# 这两个函数可以对字典类型格式化 以更好的展示
```

# 注意
- np.float32
  > 即使包含在字典类型中，也会出现 `# TypeError: x is not JSON serializable`  
  此类型不能被序列化, 需要 float() 转换一下

# reference
6.2 读写JSON数据  
<http://python3-cookbook-personal.readthedocs.io/zh_CN/latest/c06/p02_read-write_json_data.html>  
Python3 JSON 数据解析  
<http://www.runoob.com/python3/python3-json.html>  