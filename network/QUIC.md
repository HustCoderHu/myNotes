代码位于 chromium 中 ，有抽取项目 libquic

quiche 层次结构

---
# quiche 层次结构
## `quic/core`

## qbone
似乎是一种打包方式  

## quartc
似乎基于 quic 封装了一套给 rtc 用的接口  
<https://chromiumcodereview.appspot.com/2287723003/>

### udp




### frame
struct QuicFrame


### stream

`quic_packet_reader` 文件
```cpp
// A class to read incoming QUIC packets from the UDP socket.
```

# quic_dispatcher
`class QuicDispatcher`
```cpp
QuicDispatcher
```


理解 记录需要被 转发的目标，接收到某路流时，按照转发表，逐一转发可以考虑并行 openmp
