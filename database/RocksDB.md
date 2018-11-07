
---

# Block Cache
读缓存，分两个
- 缓存未被压缩的block
- 缓存压缩的block

# LRUCache


# flush
从 memtable -> L0
内存到持久介质

flush 的流程中必定有打开 sstfile 并往其中写入 chunk 的操作

https://github.com/facebook/rocksdb/wiki/Creating-and-Ingesting-SST-files

# reference
RocksDB. LRUCache源码分析  
<https://www.jianshu.com/p/75b93a664ebe>  
RocksDB 笔记  
<http://alexstocks.github.io/html/rocksdb.html>  