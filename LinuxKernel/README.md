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