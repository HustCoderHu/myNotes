https://github.com/SudarsunKannan/lsm_nvm

---
使用 beyond compare 对比文件 memtable.h 发现新增了以下内容

```
explicit MemTable(const InternalKeyComparator& cmp, ArenaNVM&  arena, bool recovery);
void SetMemTableHead(void *ptr);
void* GeTableoffset();

//NoveLSM Swap/Alternate between nvm and DRAM arena
bool isNVMMemtable;
BloomFilter bloom_;
std::unordered_set<std::string> predict_set;
void AddPredictIndex(std::unordered_set<std::string> *set, const uint8_t*);
int  CheckPredictIndex(std::unordered_set<std::string> *set, const uint8_t*);
void ClearPredictIndex(std::unordered_set<std::string> *set);
```

arena.h
```
新增 ArenaNVM 类
```

使用位置
文件 | 函数
-|-
db_impl.cc | RecoverMapFile
memtable.cc | ApproximateMemoryUsage, Add

内存管理部分使用了 libhoard  

skiplist.h 中 `NewNode()` 中使用了
```
nvm_arena->AllocateAlignedNVM()
{
    if 空间足够
      指针后移
    else 不够
      AllocateFallbackNVM()
       -> AllocateNVMBlock()
}
```
如果定义了 ENABLE_RECOVERY 则会从文件恢复  
否则

# port to rocksdb
尝试替换掉 MemTable 的 ConcurrentArena 成员  

此成员被定为 private，所以只在头文件和 memtable.cc 中使用

使用位置确定

- MemTable 初始化列表，自身初始化
- MemTable 初始化列表，unique_ptr<MemTableRep> table_ 成员初始化参数
- MemTable 初始化列表，unique_ptr<MemTableRep> range_del_table_ 成员入参
- MemTable 构造函数，DynamicBloom 构造入参
- ApproximateMemoryUsage()
- ShouldFlushNow()

## table_ 成员初始化参数
文件 memtablerep.h  
工厂模式 create 方法的入参

以下3个工厂
- SkipListFactory
- HashSkipListRep
- VectorRepFactory
```
MemTableRep* CreateMemTableRep(const MemTableRep::KeyComparator&,
                                         Allocator*, const SliceTransform*,
                                         Logger* logger)
```

### SkipListFactory
文件 skiplistrep.cc


极限写入速度 s

buf 大小 N， 阈值 x
L0 L1 

时间平衡
T(compact) <= T(剩余空间被写满)

```
while(1) {
    spin_lock()
    if (cbuf != nullptr) {
        spin_unlock();
        mutex_down();
    } else
        break;
}
写数据
切buf
spin_unlock()
```

# reference
深入Linux的内存管理，关于PTMalloc3、Hoard和TCMalloc  
<http://www.itboth.com/d/jaY73a/linux>  

