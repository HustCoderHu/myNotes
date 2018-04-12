kernel_thread 系统调用 fork vfork clone 等最后都会调用这个函数, 通过下面内容可以解释
为什么子进程可以访问父进程的众多资源，除了寄存器
以下描述基于 kernel 3.8

---
# 
``` c
/*
 *  Ok, this is the main fork-routine.
 *
 * It copies the process, and if successful kick-starts
 * it and waits for it to finish using the VM if required.
 */
long do_fork(unsigned long clone_flags,
	      unsigned long stack_start,
	      unsigned long stack_size,
	      int __user *parent_tidptr,
	      int __user *child_tidptr) {
  struct task_struct *p;
  ...
  p = copy_process(clone_flags, stack_start, stack_size,
			 child_tidptr, NULL, trace);
  ...
}

/*
 * This creates a new process as a copy of the old one,
 * but does not actually start it yet.
 *
 * It copies the registers, and all the appropriate
 * parts of the process environment (as per the clone
 * flags). The actual kick-off is left to the caller.
 */
static struct task_struct *copy_process(
    unsigned long clone_flags,
					unsigned long stack_start,
					unsigned long stack_size,
					int __user *child_tidptr,
					struct pid *pid,
					int trace) {
  ...
  retval = copy_mm(clone_flags, p);
  ...
}

static int copy_mm(unsigned long clone_flags, struct task_struct *tsk) {
  ...
  oldmm = current->mm;
  if (clone_flags & CLONE_VM) {
    atomic_inc(&oldmm->mm_users);
    mm = oldmm;
    goto good_mm;
  }
good_mm:
  tsk->mm = mm;
  tsk->active_mm = mm;
  return 0;
  ...
}
```
从上面的过程可以看到设置了 CLONE_VM 时，新的进程描述符的 `mm_struct` 内存描述符直接使用的 `current` 也就是父进程的mm，  
而且 `mm` 和 `active_mm` 都是这个
``` c
struct mm_struct {
  ...
  pgd_t * pgd;  // 进程切换里 switch_mm 会把这个转换到物理地址之后，写入
  // %%cr3
  atomic_t mm_users;		/* How many users with user space? */
  ..	
};
```

# reference
switch_mm()  
<https://blog.csdn.net/kernel_details/article/details/1484230>  

<https://blog.csdn.net/gatieme/article/details/51484562>  