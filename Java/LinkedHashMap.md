遍历时的顺序受此前的访问情况影响，类似LRU。遍历从头部开始，也就是从eldest to newest。被访问过的元素移到尾部(newest)  
参考 jdk 9.0.4

---
- 成员
- afterNodeAccess
- afterNodeInsertion
- afterNodeRemoval

上面3个函数在 HashMap 中定义为空，在 Linked 中被覆盖

# 成员

``` java
public class LinkedHashMap<K,V>
    extends HashMap<K,V>
    implements Map<K,V>
{
  /**
   * The head (eldest) of the doubly linked list.
   */
  transient LinkedHashMap.Entry<K,V> head;
  
  /**
   * The tail (youngest) of the doubly linked list.
   */
  transient LinkedHashMap.Entry<K,V> tail;
  /**
   * The iteration ordering method for this linked hash map: {@code true}
   * for access-order, {@code false} for insertion-order.
   *
   * @serial
   */
  final boolean accessOrder;
  // 默认 false 遍历顺序和hashmap一致
  // 显示初始化为 true 的时候 遍历顺序受访问情况影响
```

# afterNodeAccess
``` java
void afterNodeAccess(Node<K,V> e) { // move node to last
  LinkedHashMap.Entry<K,V> last;
  if (accessOrder && (last = tail) != e) {
    LinkedHashMap.Entry<K,V> p =
      (LinkedHashMap.Entry<K,V>)e, b = p.before, a = p.after;
    p.after = null;
    if (b == null)
      head = a;
    else
      b.after = a;
    if (a != null)
      a.before = b;
    else
      last = b;
    if (last == null)
      head = p;
    else {
      p.before = last;
      last.after = p;
    }
    tail = p;
    ++modCount;
  }
}
```
执行 get() getOrDefault() put() 时会调用上面的函数，将被访问的元素放到双向链表的末尾

# afterNodeInsertion
``` java
void afterNodeInsertion(boolean evict) { // possibly remove ldest
  LinkedHashMap.Entry<K,V> first;
  if (evict && (first = head) != null && removeEldestEntry(first)) {
    K key = first.key;
    removeNode(hash(key), key, null, false, true);
  }
}
```
执行 put() 的最后调用，根据 `removeEldestEntry` 的返回情况决定是否删除头部(eldest)

# removeEldestEntry
``` java
protected boolean removeEldestEntry(Map.Entry<K,V> eldest) {
  return false;
}
```
如果要实现 LRU 可以覆盖这个函数，以下例子摘自  
A LRU Cache in 10 Lines of Java  
<http://chriswu.me/blog/a-lru-cache-in-10-lines-of-java/>

``` java
import java.util.LinkedHashMap;
import java.util.Map;

public LRUCache<K, V> extends LinkedHashMap<K, V> {
  private int cacheSize;

  public LRUCache(int cacheSize) {
    super(16, 0.75, true);
    this.cacheSize = cacheSize;
  }

  protected boolean removeEldestEntry(Map.Entry<K, V> eldest) {
    return size() >= cacheSize;
  }
}
```

# afterNodeRemoval
``` java
void afterNodeRemoval(Node<K,V> e) { // unlink
  LinkedHashMap.Entry<K,V> p =
    (LinkedHashMap.Entry<K,V>)e, b = p.before, a = p.after;
  p.before = p.after = null;
  if (b == null)
    head = a;
  else
    b.after = a;
  if (a == null)
    tail = b;
  else
    a.before = b;
}
```
执行 remove() 时调用，将结点从双向链表里删除

# reference
使用LinkedHashMap实现LRU缓存  
<http://colobu.com/2015/09/07/LRU-cache-implemented-by-Java-LinkedHashMap/>  
Java LinkedHashMap工作原理及实现  
<https://yikun.github.io/2015/04/02/Java-LinkedHashMap%E5%B7%A5%E4%BD%9C%E5%8E%9F%E7%90%86%E5%8F%8A%E5%AE%9E%E7%8E%B0/>