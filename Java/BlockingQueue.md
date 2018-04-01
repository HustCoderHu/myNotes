阻塞队列
以下的内容参考自 jdk 9.0.4

---
接口内容如下
``` java
public interface BlockingQueue<E> extends Queue<E> {
    boolean add(E e);
    boolean offer(E e);
    void put(E e) throws InterruptedException; // 阻塞
    boolean offer(E e, long timeout, TimeUnit unit)
        throws InterruptedException;
    E take() throws InterruptedException; // 阻塞
    E poll(long timeout, TimeUnit unit)
        throws InterruptedException;
    int remainingCapacity();
    boolean remove(Object o);
    boolean contains(Object o);
    int drainTo(Collection<? super E> c);
    int drainTo(Collection<? super E> c, int maxElements);
}
```
实现类有 `ArrayBlockingQueue` 和 `LinkedBlockingQueue`  

# 1 ArrayBlockingQueue
## .1 成员
``` java
/** The queued items */
final Object[] items;
/** items index for next take, poll, peek or remove */
int takeIndex;
/** items index for next put, offer, or add */
int putIndex;
/** Number of elements in the queue */
int count;  // 每次take和put都修改，而不是通过上面两个index计算
/*
 * Concurrency control uses the classic two-condition algorithm
 * found in any textbook.
 */
/** Main lock guarding all access */
final ReentrantLock lock;
/** Condition for waiting takes */
private final Condition notEmpty;
/** Condition for waiting puts */
private final Condition notFull;
```

## .2 初始化
``` java
public ArrayBlockingQueue(int capacity) {
    this(capacity, false);
}
public ArrayBlockingQueue(int capacity, boolean fair) // fair 传给 ReentrantLock
public ArrayBlockingQueue(int capacity, boolean fair,
                              Collection<? extends E> c)
{
    if (capacity <= 0)
        throw new IllegalArgumentException();
    this.items = new Object[capacity];
    lock = new ReentrantLock(fair);
    notEmpty = lock.newCondition();
    notFull =  lock.newCondition();
}
```

## .3 入队
总体流程就是
- 加锁
- 队列满就等待 notFull 信号, 否则执行入队
- 解锁
``` java
/**
 * Inserts the specified element at the tail of this queue, waiting
 * for space to become available if the queue is full.
 *
 * @throws InterruptedException {@inheritDoc}
 * @throws NullPointerException {@inheritDoc}
 */
public void put(E e) throws InterruptedException {
    Objects.requireNonNull(e);
    final ReentrantLock lock = this.lock;
    lock.lockInterruptibly();
    try {
        while (count == items.length)
            notFull.await(); // 等队列有空位
        enqueue(e);
    } finally {
        lock.unlock();
    }
}

/**
 * Inserts element at current put position, advances, and signals.
 * Call only when holding lock.
 */
private void enqueue(E e) {
    // assert lock.isHeldByCurrentThread();
    // assert lock.getHoldCount() == 1;
    // assert items[putIndex] == null;
    final Object[] items = this.items;
    items[putIndex] = e;
    if (++putIndex == items.length) putIndex = 0; // 循环数组
    count++;
    notEmpty.signal(); // 唤醒消费者
}
```
观察put的过程，可以推测出 notFull.await() 有 lock.unlock() 的效果，否则消费者就无法从满队列里取出元素。  
查看 await 的实现发现有一个 fullyRelease 函数, 这个函数还调用了下面的release
```java
/**
 * Releases in exclusive mode.  Implemented by unblocking one or
 * more threads if {@link #tryRelease} returns true.
 * This method can be used to implement method {@link Lock#unlock}.
 *
 * @param arg the release argument.  This value is conveyed to
 *        {@link #tryRelease} but is otherwise uninterpreted and
 *        can represent anything you like.
 * @return the value returned from {@link #tryRelease}
 */
public final boolean release(int arg)

// ReentrantLock 的释放锁也是用的这个函数，印证了上面的推测，
// 和官方注释也是统一的
public void unlock() {
    sync.release(1);
}
```

## .4 出队
过程和入队类似，不过用到是 notEmpty信息
``` java
public E take() throws InterruptedException {
    final ReentrantLock lock = this.lock;
    lock.lockInterruptibly();
    try {
        while (count == 0)
            notEmpty.await(); // 
        return dequeue();
    } finally {
        lock.unlock();
    }
}

/**
 * Extracts element at current take position, advances, and signals.
 * Call only when holding lock.
 */
private E dequeue() {
    // assert lock.isHeldByCurrentThread();
    // assert lock.getHoldCount() == 1;
    // assert items[takeIndex] != null;
    final Object[] items = this.items;
    @SuppressWarnings("unchecked")
    E e = (E) items[takeIndex];
    items[takeIndex] = null;
    if (++takeIndex == items.length) takeIndex = 0;
    count--;
    if (itrs != null)
        itrs.elementDequeued();
    notFull.signal();
    return e;
}
```

# 2 LinkedBlockingQueue
## .1 成员
``` java
/** The capacity bound, or Integer.MAX_VALUE if none */
private final int capacity;
/** Current number of elements */
private final AtomicInteger count = new AtomicInteger();
/**
 * Head of linked list.
 * Invariant: head.item == null
 */
transient Node<E> head;
/**
 * Tail of linked list.
 * Invariant: last.next == null
 */
private transient Node<E> last;
/** Lock held by take, poll, etc */
private final ReentrantLock takeLock = new ReentrantLock();
/** Wait queue for waiting takes */
private final Condition notEmpty = takeLock.newCondition();
/** Lock held by put, offer, etc */
private final ReentrantLock putLock = new ReentrantLock();
/** Wait queue for waiting puts */
private final Condition notFull = putLock.newCondition();
```

## .2 入队
``` java
/**
 * Inserts the specified element at the tail of this queue, waiting if
 * necessary for space to become available.
 *
 * @throws InterruptedException {@inheritDoc}
 * @throws NullPointerException {@inheritDoc}
 */
public void put(E e) throws InterruptedException {
    if (e == null) throw new NullPointerException();
    // Note: convention in all put/take/etc is to preset local var
    // holding count negative to indicate failure unless set.
    int c = -1;
    Node<E> node = new Node<E>(e);
    final ReentrantLock putLock = this.putLock;
    final AtomicInteger count = this.count;
    putLock.lockInterruptibly();
    try {
        /*
         * Note that count is used in wait guard even though it is
         * not protected by lock. This works because count can
         * only decrease at this point (all other puts are shut
         * out by lock), and we (or some other waiting put) are
         * signalled if it ever changes from capacity. Similarly
         * for all other uses of count in other wait guards.
         */
        while (count.get() == capacity) {
            notFull.await();
        }
        enqueue(node);
        c = count.getAndIncrement();
        if (c + 1 < capacity)
            notFull.signal();
    } finally {
        putLock.unlock();
    }
    if (c == 0)
        signalNotEmpty();
}
```