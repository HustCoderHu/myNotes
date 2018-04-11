task_struct 包含进程所需的所有信息
{
volatile long state
5 互斥状态
  TASK_RUNNING, INTERRUPTIBLE UNxxx(两个睡眠状态)
  STOPPED, TRACED
  
  TASK_KILLABLE 类似 UNxxx 但可以响应致命信号
2 终止状态
  EXIT_ZOMBIE, _DEAD
  
线程组
pid_t pid;
pid_t tgid; // 线程组相同 pid, getpid() 返回

进程内核栈
void *stack // 紧邻 thread_info 8192B(两个页框)
}


glibc 默认内存管理器，类似内存池

Linux c 开发 - 内存管理器ptmalloc  
<https://blog.csdn.net/initphp/article/details/50833036>  

内存屏障  
<https://www.jianshu.com/p/2ab5e3d7e510>  
一文解决内存屏障  
<https://monkeysayhi.github.io/2017/12/28/%E4%B8%80%E6%96%87%E8%A7%A3%E5%86%B3%E5%86%85%E5%AD%98%E5%B1%8F%E9%9A%9C/>  
谈乱序执行和内存屏障  
<https://blog.csdn.net/dd864140130/article/details/56494925>