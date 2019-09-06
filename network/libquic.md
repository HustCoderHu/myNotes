
# 包相关
`quic_packets` 文件
```cpp

struct QuicPacketHeader;

class QuicPacket : public QuicData

class QuicEncryptedPacket : public QuicData

class QuicReceivedPacket : public QuicEncryptedPacket

struct QUIC_EXPORT_PRIVATE SerializedPacket

```

接口类 `QuicPacketWriter`

实现类之一 `QuartcPacketWriter`

```cpp
WritePacket() override

```

发送
```cpp
QuicConnection::WritePacket(SerializedPacket* packet) // SerializedPacket定义在 quic_packets.h
-> writer_->WritePacket() // write_ 是 QuicPacketWriter 对象
```

接收
```cpp
QuicDispatcher::MaybeDispatchPacket()
find sess by conn_id
-> QuicSession::ProcessUdpPacket()
  -> QuicConnection::ProcessUdpPacket()


QuicSession::ProcessUdpPacket(const IPEndPoint& self_address,
                                   const IPEndPoint& peer_address,
                                   const QuicReceivedPacket& packet)
{
  QuicConnection::ProcessUdpPacket()
```

# 流相关
```
// Buffers frames until we have something which can be passed
// up to the next layer.
class QuicStreamSequencer // quiche 版本有改动

// It does not contain the entire interface needed by an application to interact
// with a QUIC stream.  Some parts of the interface must be obtained by
// accessing the owning session object.  A subclass of ReliableQuicStream
// connects the object and the application that generates and consumes the data
// of the stream.

// The ReliableQuicStream object has a dependent QuicStreamSequencer object,
// which is given the stream frames as they arrive, and provides stream data in
// order by invoking ProcessRawData().
class ReliableQuicStream;
```

# 连接相关
quic_connection.h
```cpp
// The entity that handles framing writes for a Quic client or server.
// Each QuicSession will have a connection associated with it.
//
// On the server side, the Dispatcher handles the raw reads, and hands off
// packets via ProcessUdpPacket for framing and processing.
//
// On the client side, the Connection handles the raw reads, as well as the
// processing.
//
// Note: this class is not thread-safe.
```


# 会话相关