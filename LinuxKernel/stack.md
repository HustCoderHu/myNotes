x86-64 下发生函数调用时，栈的变化

---
# 概述
相对 x86，64 下的除了寄存器长度变成64bit，还新增了以下寄存器
%r8 ~ %r15

- %rax 作为函数返回值使用
- %rsp 栈指针寄存器，指向栈顶
- %rdi，%rsi，%rdx，%rcx，%r8，%r9 用作函数参数，依次对应第1参数，第2参数。。。 
- %rbx，%rbp，%r12，%r13，%14，%15 用作数据存储，遵循被调用者使用规则，简单说就是随便用，调用子函数之前要备份它，以防被修改 
- %r10，%r11 用作数据存储，遵循调用者使用规则，简单说就是使用之前要先保存原值

# 用户态
```
movq 参数, 寄存器
call func

func():
  pushq %rbp
  movq %rsp, %rbp # 更新栈底
  ...
  leave # mov %esp, %ebp; pop ebp
  ret # popq rip
```

# 内核态
用户态的地址需要经过 mmu 转换才能找到对应的物理地址，而内核态只是一个偏移就能找到，两者的转换方式不同，所以不能直接访问相互的地址空间

互相访问需要经过复制
``` c
unsigned long
copy_to_user(void __user *to, const void *from, unsigned long n);
unsigned long
copy_from_user(void *to, const void __user *from, unsigned long n);
```

``` c
// arch\x86\lib\usercopy_32.c
unsigned long
copy_to_user(void __user *to, const void *from, unsigned long n)
{
	if (access_ok(VERIFY_WRITE, to, n))
		n = __copy_to_user(to, from, n);
	return n;
}
EXPORT_SYMBOL(copy_to_user);

// arch\x86\include\asm\uaccess_64.h
static __always_inline __must_check
int __copy_to_user(void __user *dst, const void *src, unsigned size)
{
	int ret = 0;

	might_fault();
	if (!__builtin_constant_p(size))
		return copy_user_generic((__force void *)dst, src, size);
	switch (size) {
	case 1:__put_user_asm(*(u8 *)src, (u8 __user *)dst,
			      ret, "b", "b", "iq", 1);
		return ret;
	case 2:__put_user_asm(*(u16 *)src, (u16 __user *)dst,
			      ret, "w", "w", "ir", 2);
		return ret;
	case 4:__put_user_asm(*(u32 *)src, (u32 __user *)dst,
			      ret, "l", "k", "ir", 4);
		return ret;
	case 8:__put_user_asm(*(u64 *)src, (u64 __user *)dst,
			      ret, "q", "", "er", 8);
		return ret;
	case 10:
		__put_user_asm(*(u64 *)src, (u64 __user *)dst,
			       ret, "q", "", "er", 10);
		if (unlikely(ret))
			return ret;
		asm("":::"memory");
		__put_user_asm(4[(u16 *)src], 4 + (u16 __user *)dst,
			       ret, "w", "w", "ir", 2);
		return ret;
	case 16:
		__put_user_asm(*(u64 *)src, (u64 __user *)dst,
			       ret, "q", "", "er", 16);
		if (unlikely(ret))
			return ret;
		asm("":::"memory");
		__put_user_asm(1[(u64 *)src], 1 + (u64 __user *)dst,
			       ret, "q", "", "er", 8);
		return ret;
	default:
		return copy_user_generic((__force void *)dst, src, size);
	}
}
```


# reference
X86-64寄存器和栈帧  
<https://blog.csdn.net/u013982161/article/details/51347944>  
函数调用中堆栈的个人理解  
<https://www.cnblogs.com/MyLove-Summer/p/5034209.html>  
Linux系统调用过程分析  
<https://www.binss.me/blog/the-analysis-of-linux-system-call/>  
Linux系统调用机制int 0x80、sysenter/sysexit、syscall/sysret的原理与代码分析  
<https://www.jianshu.com/p/f4c04cf8e406>  
Linux 系统内核调用 第二节  
<https://github.com/MintCN/linux-insides-zh/blob/master/SysCall/linux-syscall-2.md>  
copy_to_user和copy_from_user两个函数的分析  
<https://blog.csdn.net/yangdelong/article/details/5491097>  
linux内核中的copy_to_user和copy_from_user(一)  
<https://blog.csdn.net/ce123_zhouwei/article/details/8454226>  