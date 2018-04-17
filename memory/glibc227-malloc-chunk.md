
---
- 分配函数
- 数据结构
  - chunk 内存分布
    - 空闲 chunk 内存分布
    - 若干宏
  - Bins
  - Fastbins
  - Unsorted chunks
  - 3种特殊 chunk

# 分配函数
- brk
- mmap
- sbrk

# 数据结构
## chunk 内存分布
``` c
/*
  This struct declaration is misleading (but accurate and necessary).
  It declares a "view" into memory allowing access to necessary
  fields at known offsets from a given base. See explanation below.
*/

struct malloc_chunk {

  INTERNAL_SIZE_T      mchunk_prev_size;  /* Size of previous chunk (if free).  */
  INTERNAL_SIZE_T      mchunk_size;       /* Size in bytes, including overhead. */

  struct malloc_chunk* fd;         /* double links -- used only if free. */
  struct malloc_chunk* bk;

  /* Only used for large blocks: pointer to next larger size.  */
  // large bin 使用
  struct malloc_chunk* fd_nextsize; /* double links -- used only if free. */
  struct malloc_chunk* bk_nextsize;
};
```

```
    chunk-> +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	    |             Size of previous chunk, if unallocated (P clear)  |
	    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	    |             Size of chunk, in bytes                     |A|M|P|
      mem-> +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	    |             User data starts here...                          .
	    .                                                               .
	    .             (malloc_usable_size() bytes)                      .
	    .                                                               |
nextchunk-> +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	    |             (size of chunk, but used for application data)    |
	    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	    |             Size of next chunk, in bytes                |A|0|1|
	    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
```
- P 前一个chunk是否被用
- M chunk的来源，1表示mmap，0是heap(sbrk)
- A 属于的分配区，1表示非主分配区，0是主分配区
- mem 是返回给用户的指针

### 空闲 chunk 内存分布
```
    chunk-> +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	    |             Size of previous chunk, if unallocated (P clear)  |
	    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    `head:' |             Size of chunk, in bytes                     |A|0|P|
      mem-> +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	    |             Forward pointer to next chunk in list             |
	    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	    |             Back pointer to previous chunk in list            |
	    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	    |             Unused space (may be 0 bytes long)                .
	    .                                                               .
	    .                                                               |
nextchunk-> +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    `foot:' |             Size of chunk, in bytes                           |
	    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	    |             Size of next chunk, in bytes                |A|0|0|
	    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
```
- **空间复用**
  - 空闲 chunk 头结构大小为`4*INTERNAL_SIZE_T`
  - 使用中的chunk，覆盖下一chunk的 `prev_size` 域
    > 因为下一 chunk 的这个域失效

所以使用中的chunk大小 `(req_sz+2*INTERNAL_SIZE_T-INTERNAL_SIZE_T) align MALLOC_ALIGNMENT`
- +2*INTERNAL_SIZE_T 分别是 prev_size 和 size
- -INTERNAL_SIZE_T 覆盖下一chunk的prev_size
- MALLOC_ALIGNMENT
  > glibc2.27 里定义在 `sysdeps/generic/malloc-alignment.h` 通常是2倍机器字长
  ``` c
  /* MALLOC_ALIGNMENT is the minimum alignment for malloc'ed chunks.  It
   must be a power of two at least 2 * SIZE_SZ, even on machines for
   which smaller alignments would suffice. It may be defined as larger
   than this though. Note however that code and data structures are
   optimized for the case of 8-byte alignment.  */
  #define MALLOC_ALIGNMENT (2 * SIZE_SZ < __alignof__ (long double) \
			  ? __alignof__ (long double) : 2 * SIZE_SZ)
  ```

### 若干宏
```c
typedef struct malloc_chunk* mchunkptr;
#ifndef INTERNAL_SIZE_T
# define INTERNAL_SIZE_T size_t
#endif
/* The corresponding word size.  */
#define SIZE_SZ (sizeof (INTERNAL_SIZE_T))
// chunk 指针 和 mem 的转换
#define chunk2mem(p)   ((void*)((char*)(p) + 2*SIZE_SZ))
#define mem2chunk(mem) ((mchunkptr)((char*)(mem) - 2*SIZE_SZ))
/* The smallest possible chunk */
#define MIN_CHUNK_SIZE        (offsetof(struct malloc_chunk, fd_nextsize))  // 空闲 chunk 4*size_t
/* The smallest size we can malloc is an aligned minimal chunk */
#define MINSIZE  \
  (unsigned long)(((MIN_CHUNK_SIZE+MALLOC_ALIGN_MASK) & ~MALLOC_ALIGN_MASK))  // 对齐
```

## Bins
``` c
typedef struct malloc_chunk *mbinptr;
#define NBINS             128
#define NSMALLBINS         64
#define SMALLBIN_WIDTH    MALLOC_ALIGNMENT
#define SMALLBIN_CORRECTION (MALLOC_ALIGNMENT > 2 * SIZE_SZ)
#define MIN_LARGE_SIZE    ((NSMALLBINS - SMALLBIN_CORRECTION) * SMALLBIN_WIDTH) // 64 * 2 * size_t = 1 MB
struct malloc_state
{
  ...
/* Normal bins packed as described above */
  mchunkptr bins[NBINS * 2 - 2];
  ...
}
// < 1 MB 的都在 smallbin
#define in_smallbin_range(sz)  \
  ((unsigned long) (sz) < (unsigned long) MIN_LARGE_SIZE)
// small bin 相邻间隔 2*size_t，是等差数列 最大编号 63
// 64 会进入 large bin
#define smallbin_index(sz) \
  ((SMALLBIN_WIDTH == 16 ? (((unsigned) (sz)) >> 4) : (((unsigned) (sz)) >> 3))\
   + SMALLBIN_CORRECTION)
/* addressing -- note that bin_at(0) does not exist */
// Bin1 是 unordered list
#define bin_at(m, i) \
  (mbinptr) (((char *) &((m)->bins[((i) - 1) * 2]))			      \
             - offsetof (struct malloc_chunk, fd))
/* analog of ++bin */
#define next_bin(b)  ((mbinptr) ((char *) (b) + (sizeof (mchunkptr) << 1)))
```
每个bin 是双向循环链表，申请从尾部开始，释放回到头部

## Fastbins
单链表，频繁分配和释放小内存时使用，P标志不变，所以不会合并。某个时间点 fast binb诶遍历，相邻的空闲chunk合并，随后加入 unsorted bin，然后unsorted加入 bins
``` c
typedef struct malloc_chunk *mfastbinptr;
#define fastbin(ar_ptr, idx) ((ar_ptr)->fastbinsY[idx])
/* offset 2 to use otherwise unindexable first 2 bins */
#define fastbin_index(sz) \
  ((((unsigned int) (sz)) >> (SIZE_SZ == 8 ? 4 : 3)) - 2)
/* The maximum fastbin request size we support */
#define MAX_FAST_SIZE     (80 * SIZE_SZ / 4)
#define NFASTBINS  (fastbin_index (request2size (MAX_FAST_SIZE)) + 1)
```

## Unsorted chunks
``` c
/*
   Unsorted chunks

    All remainders from chunk splits, as well as all returned chunks,
    are first placed in the "unsorted" bin. They are then placed
    in regular bins after malloc gives them ONE chance to be used before
    binning. So, basically, the unsorted_chunks list acts as a queue,
    with chunks being placed on it in free (and malloc_consolidate),
    and taken off (to be either used or placed in bins) in malloc.

    The NON_MAIN_ARENA flag is never set for unsorted chunks, so it
    does not have to be taken into account in size comparisons.
 */
#define unsorted_chunks(M)          (bin_at (M, 1))
```
bins 数组的[0]，看注释好像是用过一次之后回到常规的 bin

## 3种特殊 chunk
都不在 bin 中
- Top
  > 位于空闲内存的最高处，其他 chunk 的空间都不足以应对请求时，使用 Top
- mmaped chunk
  > 需要超过 top最大值，直接 mmap映射内存，free的时候还给OS
- last remainder
  > small bins里找不到合适的大小来满足small chunk，如果last remainder够大就切一部分给用户，剩下的成为 last remainder

# reference
深入理解Linux堆分配器-DLMalloc  
<http://4ch12dy.site/2017/04/23/Linux%E5%A0%86%E5%88%86%E9%85%8D%E5%99%A8-DLMalloc/Linux%E5%A0%86%E5%88%86%E9%85%8D%E5%99%A8-DLMalloc/>  

https://wps2015.org/drops/drops/%E6%B7%B1%E5%85%A5%E7%90%86%E8%A7%A3%20glibc%20malloc.html  
Glibc 内存管理  
<https://paper.seebug.org/papers/Archive/refs/heap/glibc%E5%86%85%E5%AD%98%E7%AE%A1%E7%90%86ptmalloc%E6%BA%90%E4%BB%A3%E7%A0%81%E5%88%86%E6%9E%90.pdf>  
Glibc内存管理--ptmalloc2源代码分析（一）  
<http://mqzhuang.iteye.com/blog/1005909>  