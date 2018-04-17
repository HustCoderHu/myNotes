
---
# 1 成员

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
```