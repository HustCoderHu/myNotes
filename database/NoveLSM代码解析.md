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