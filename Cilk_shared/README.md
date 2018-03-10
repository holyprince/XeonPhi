1:#pragma offload_attribute(push,_Cilk_shared)  and #pragma offload_attribute(pop) 可以让shared函数在MIC卡上使用malloc

2：在MIC卡使用的Shared内存需要使用sharedmalloc来申请

3：在MIC上本身使用的内存还是可以使用malloc
