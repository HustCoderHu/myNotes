
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
class QuicStreamSequencer // quiche 版本有改动
```