以下内容参考 内核 v2.6.35

---
![](http://www.wowotech.net.img.800cdn.com/content/uploadfile/201507/3f0b279644ba4646b73f303367bbe47220150715104521.gif)  

使用了等待队列记录被阻塞的 `worker_thread`

# 数据结构
``` c
/*
 * The externally visible workqueue abstraction is an array of
 * per-CPU workqueues:
 */
struct workqueue_struct {
  struct cpu_workqueue_struct *cpu_wq;
  // per-cpu work queue struct
  struct list_head list; // 每个cpu一个cpu_workqueue_struct的链
  const char *name;
  int singlethread;  // single thread or multi thread
  int freezeable;		/* Freeze threads during suspend */
  int rt;
#ifdef CONFIG_LOCKDEP
  struct lockdep_map lockdep_map;
#endif
};
```
workqueue就是一种把某些任务（work）推迟到一个或者一组内核线程中去执行，  
那个内核线程被称作worker thread（每个processor上有一个work thread）。系统中所有的workqueue  
会挂入一个全局链表，链表头定义如下：

``` c
static LIST_HEAD(workqueues);
```

``` c
/*
 * The per-CPU workqueue (if single thread, we always use the first
 * possible cpu).
 */
struct cpu_workqueue_struct {
  spinlock_t lock;

  struct list_head worklist;
  wait_queue_head_t more_work;
  struct work_struct *current_work;

  struct workqueue_struct *wq; // 指向父对象
  struct task_struct *thread;
} ____cacheline_aligned;
```

``` c
typedef void (*work_func_t)(struct work_struct *work);

struct work_struct {
  atomic_long_t data;
#define WORK_STRUCT_PENDING 0		/* T if work item pending execution */
#define WORK_STRUCT_STATIC  1		/* static initializer (debugobjects) */
#define WORK_STRUCT_FLAG_MASK (3UL)
#define WORK_STRUCT_WQ_DATA_MASK (~WORK_STRUCT_FLAG_MASK)
  struct list_head entry;
  work_func_t func;
#ifdef CONFIG_LOCKDEP
  struct lockdep_map lockdep_map;
#endif
};
```

# 接口
## 提交任务
``` c
/**
 * queue_work - queue work on a workqueue
 * @wq: workqueue to use
 * @work: work to queue
 *
 * Returns 0 if @work was already on a queue, non-zero otherwise.
 *
 * We queue the work to the CPU on which it was submitted, but if the CPU dies
 * it can be processed by another CPU.
 */
int queue_work(struct workqueue_struct *wq, struct work_struct *work)
{
    int ret;

    ret = queue_work_on(get_cpu(), wq, work);
    put_cpu();

    return ret;
}
EXPORT_SYMBOL_GPL(queue_work);

/**
 * queue_work_on - queue work on specific cpu
 * @cpu: CPU number to execute work on
 * @wq: workqueue to use
 * @work: work to queue
 *
 * Returns 0 if @work was already on a queue, non-zero otherwise.
 *
 * We queue the work to a specific CPU, the caller must ensure it
 * can't go away.
 */
int
queue_work_on(int cpu, struct workqueue_struct *wq, struct work_struct *work)
{
    int ret = 0;

    if (!test_and_set_bit(WORK_STRUCT_PENDING, work_data_bits(work))) {
        BUG_ON(!list_empty(&work->entry));
        __queue_work(wq_per_cpu(wq, cpu), work);
        ret = 1;
    }
    return ret;
}
EXPORT_SYMBOL_GPL(queue_work_on);

static void __queue_work(struct cpu_workqueue_struct *cwq,
             struct work_struct *work)
{
    unsigned long flags;

    debug_work_activate(work);
    spin_lock_irqsave(&cwq->lock, flags);
    insert_work(cwq, work, &cwq->worklist);
    spin_unlock_irqrestore(&cwq->lock, flags);
}

static void insert_work(struct cpu_workqueue_struct *cwq,
            struct work_struct *work, struct list_head *head)
{
    trace_workqueue_insertion(cwq->thread, work);

    set_wq_data(work, cwq);
    /*
     * Ensure that we get the right work->data if we see the
     * result of list_add() below, see try_to_grab_pending().
     */
    smp_wmb();
    list_add_tail(&work->entry, head);
    wake_up(&cwq->more_work);
}
```

## 执行任务
``` c
#define create_workqueue(name) __create_workqueue((name), 0, 0, 0)

#define __create_workqueue(name, singlethread, freezeable, rt)	\
    __create_workqueue_key((name), (singlethread), (freezeable), (rt), NULL, NULL)

struct workqueue_struct *__create_workqueue_key(const char *name,
                        int singlethread,
                        int freezeable,
                        int rt,
                        struct lock_class_key *key,
                        const char *lock_name)
{
    struct workqueue_struct *wq;
    struct cpu_workqueue_struct *cwq;
    int err = 0, cpu;

    wq = kzalloc(sizeof(*wq), GFP_KERNEL);
    if (!wq)
        return NULL;

    wq->cpu_wq = alloc_percpu(struct cpu_workqueue_struct);
    if (!wq->cpu_wq) {
        kfree(wq);
        return NULL;
    }

    wq->name = name;
    lockdep_init_map(&wq->lockdep_map, lock_name, key, 0);
    wq->singlethread = singlethread;
    wq->freezeable = freezeable;
    wq->rt = rt;
    INIT_LIST_HEAD(&wq->list);

    if (singlethread) {
        cwq = init_cpu_workqueue(wq, singlethread_cpu);
        err = create_workqueue_thread(cwq, singlethread_cpu);
        start_workqueue_thread(cwq, -1);
    } else {
        cpu_maps_update_begin();
        /*
         * We must place this wq on list even if the code below fails.
         * cpu_down(cpu) can remove cpu from cpu_populated_map before
         * destroy_workqueue() takes the lock, in that case we leak
         * cwq[cpu]->thread.
         */
        spin_lock(&workqueue_lock);
        list_add(&wq->list, &workqueues);
        spin_unlock(&workqueue_lock);
        /*
         * We must initialize cwqs for each possible cpu even if we
         * are going to call destroy_workqueue() finally. Otherwise
         * cpu_up() can hit the uninitialized cwq once we drop the
         * lock.
         */
        for_each_possible_cpu(cpu) {
            cwq = init_cpu_workqueue(wq, cpu);
            if (err || !cpu_online(cpu))
                continue;
            // 每个cpu一个
            err = create_workqueue_thread(cwq, cpu);
            start_workqueue_thread(cwq, cpu);
        }
        cpu_maps_update_done();
    }

    if (err) {
        destroy_workqueue(wq);
        wq = NULL;
    }
    return wq;
}
EXPORT_SYMBOL_GPL(__create_workqueue_key);

static int create_workqueue_thread(struct cpu_workqueue_struct *cwq, int cpu)
{
    struct sched_param param = { .sched_priority = MAX_RT_PRIO-1 };
    struct workqueue_struct *wq = cwq->wq;
    const char *fmt = is_wq_single_threaded(wq) ? "%s" : "%s/%d";
    struct task_struct *p;

    p = kthread_create(worker_thread, cwq, fmt, wq->name, cpu);
    /*
     * Nobody can add the work_struct to this cwq,
     *	if (caller is __create_workqueue)
     *		nobody should see this wq
     *	else // caller is CPU_UP_PREPARE
     *		cpu is not on cpu_online_map
     * so we can abort safely.
     */
    if (IS_ERR(p))
        return PTR_ERR(p);
    if (cwq->wq->rt)
        sched_setscheduler_nocheck(p, SCHED_FIFO, &param);
    cwq->thread = p;

    trace_workqueue_creation(cwq->thread, cpu);

    return 0;
}

static int worker_thread(void *__cwq)
{
    struct cpu_workqueue_struct *cwq = __cwq;
    DEFINE_WAIT(wait);

    if (cwq->wq->freezeable)
        set_freezable();

    for (;;) {
        // 定义在 kernel/wait.c
        // 将当前线程加入到等待队列
        prepare_to_wait(&cwq->more_work, &wait, TASK_INTERRUPTIBLE);
        if (!freezing(current) &&
            !kthread_should_stop() &&
            list_empty(&cwq->worklist))
            schedule();
        // 从等待队列里移除
        finish_wait(&cwq->more_work, &wait);

        try_to_freeze();

        if (kthread_should_stop())
            break;

        run_workqueue(cwq);
    }

    return 0;
}

static void run_workqueue(struct cpu_workqueue_struct *cwq)
{
    spin_lock_irq(&cwq->lock);
    while (!list_empty(&cwq->worklist)) {
        struct work_struct *work = list_entry(cwq->worklist.next,
                        struct work_struct, entry);
        work_func_t f = work->func;
#ifdef CONFIG_LOCKDEP
        /*
         * It is permissible to free the struct work_struct
         * from inside the function that is called from it,
         * this we need to take into account for lockdep too.
         * To avoid bogus "held lock freed" warnings as well
         * as problems when looking into work->lockdep_map,
         * make a copy and use that here.
         */
        struct lockdep_map lockdep_map = work->lockdep_map;
#endif
        trace_workqueue_execution(cwq->thread, work);
        debug_work_deactivate(work);
        cwq->current_work = work;
        list_del_init(cwq->worklist.next);
        spin_unlock_irq(&cwq->lock); // 取下work后就可以解锁

        BUG_ON(get_wq_data(work) != cwq);
        work_clear_pending(work);
        lock_map_acquire(&cwq->wq->lockdep_map);
        lock_map_acquire(&lockdep_map);
        f(work); // 执行
        lock_map_release(&lockdep_map);
        lock_map_release(&cwq->wq->lockdep_map);

        if (unlikely(in_atomic() || lockdep_depth(current) > 0)) {
            printk(KERN_ERR "BUG: workqueue leaked lock or atomic: "
                    "%s/0x%08x/%d\n",
                    current->comm, preempt_count(),
                           task_pid_nr(current));
            printk(KERN_ERR "    last function: ");
            print_symbol("%s\n", (unsigned long)f);
            debug_show_held_locks(current);
            dump_stack();
        }

        spin_lock_irq(&cwq->lock);
        cwq->current_work = NULL;
    }
    spin_unlock_irq(&cwq->lock);
}
```

# reference

<http://www.wowotech.net/irq_subsystem/workqueue.html>  
<https://elixir.bootlin.com/linux/v2.6.35/source/include/linux/workqueue.h>  
<https://elixir.bootlin.com/linux/v2.6.35/source/kernel/workqueue.c>