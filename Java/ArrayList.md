ArrayList
以下的内容参考自 jdk 9.0.4

---
# 1 ArrayList
``` java
public E get(int index)
public E set(int index, E element)
```
get 和 set 用的多但是没什么细节

## 初始化
有3个构造函数
``` java
/**
 * Constructs an empty list with an initial capacity of ten.
 */
public ArrayList() {
    this.elementData = DEFAULTCAPACITY_EMPTY_ELEMENTDATA;
}
// DEFAULTCAPACITY_EMPTY_ELEMENTDATA 和 EMPTY_ELEMENTDATA 的区别
// 在于第1次 add 时容量的变化

// initialCapacity == 0 时
// elementData = EMPTY_ELEMENTDATA
public ArrayList(int initialCapacity)

// 将 集合 c中的元素 用 Arrays.copyOf 复制过来
// 如果集合为空 elementData = EMPTY_ELEMENTDATA
public ArrayList(Collection<? extends E> c)
```

## add
``` java
public boolean add(E e)
private void add(E e, Object[] elementData, int s)
private Object[] grow() {
    return grow(size + 1);
}
private Object[] grow(int minCapacity) {
    return elementData = Arrays.copyOf(elementData,
        newCapacity(minCapacity));
}
/**
 * Returns a capacity at least as large as the given minimum capacity.
 * Returns the current capacity increased by 50% if that suffices.
 * Will not return a capacity greater than MAX_ARRAY_SIZE unless
 * the given minimum capacity is greater than MAX_ARRAY_SIZE.
 *
 * @param minCapacity the desired minimum capacity
 * @throws OutOfMemoryError if minCapacity is less than zero
 */
private int newCapacity(int minCapacity)
```
上面是这个版本的 jdk 里 add 的(嵌套)调用过程, 第1次调用 add 时，minCapacity 为 0，  
结合注释和代码可以看出，无参构造函数生成的 ArrayList 会使 newCapacity(1) 返回 10(DEFAULT_CAPACITY)  
而通过 ArrayList(0) 构造的情况下 newCapacity(1) 返回 1  

## 总结
    1 Object[] 的内存申请发生在第1次 add，而且申请的大小和构造方式有关
        ArrayList() -> Array.copyof({}, 10)  
        ArrayList(0) -> Array.copyof({}, 1)  
    2 容量增加发生在空间用尽时，用 ArrayList.copyof(dat, size + size>>1), 即增加一半空间


# reference
关于ArrayList的5道面试题  
<http://www.importnew.com/9928.html>  