# TestPeterson

测试多线程 Peterson 互斥算法

**测试环境**  
CPU: Intel Core i5-7200  
CPU Cores: 4  
OS: Windows 10 x64 (64bit)

**编译配置**  
VS 2008 (MSVC 9.0)  
x86 and x64 Release  
Optimization: /O2 /Oi

## 测试结果

**运行参数**  
THREAD_NUM = 4  
TEST_ROUND = 50

x86 与 x64 的测试结果相同

|       Test                  | Data Race  |
| --------------------------- | ---------- |
| Baseline (Critical Section) | Never      |
| Peterson2 (membar off)      | Frequently |
| Peterson2 (membar on)       | Few        |
| PetersonN (membar off)      | Frequently |
| PetersonN (membar on)       | Frequently |

## 总结

由于现代 CPU 支持指令乱序执行 (Out-of-Order Execution)，加上编译器的指令重排优化 (Compiler Reordering Optimizations)，使得传统的 Peterson 互斥算法很难起作用。

测试中使用了内存屏障 (memory barrier)，但也没有达到目标，仍有数据竞争 (data race) 发生。原因可能是：
1. volatile 修饰符的使用有缺失
2. membar 的调用有缺失

参考 [Wikipedia: Memory barrier](https://en.wikipedia.org/wiki/Memory_barrier) 及其章节 [Out-of-order execution versus compiler reordering optimizations](https://en.wikipedia.org/wiki/Memory_barrier#Out-of-order_execution_versus_compiler_reordering_optimizations)

**Peterson 算法的替代方法**

在 Windows 系统上通常有这些原生的 low-level 线程同步设施：

1. 处理器原子操作，如使用 MSVC 的 Interlocked 系列内建函数 (intrinsics) 进行序列化操作（如累加）。  
   处理器原子操作的效率很高，程序无需进入内核态，序列化过程由处理器实现。

2. 如果程序中需要用户态线程锁，可用 InterlockedExchange() 内建函数实现用户态自旋锁 (spinlock)。  
   spinlock 的自旋过程 (spinning) 是一种忙等循环 (busy wait)，如果等待时间长会冲高处理器占用。

3. 使用临界区对象 (Critical Section) 进行序列化。  
   临界区对象是一种 Windows 内核同步对象，通常比其它内核同步对象 Event, Mutex, Semaphore 的效率高。  
   临界区对象内部使用 Spinlock + Event on Demand (WinXP+) 的方式实现。  
   临界区对象只限于同一进程内使用，而 Event, Mutex, Semaphore 可用于多进程间同步。

