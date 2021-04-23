#ifdef RUNTIME
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

void *malloc(size_t size)
{
    static int count = 0;
    count++;
    void *(*mallocp)(size_t size);
    char *error;

    /* Find the run-time address in the shared object HANDLE refers to of the symbol called NAME. */
    mallocp = dlsym(RTLD_NEXT, "malloc");
    if ((error = dlerror()) != NULL){
        fputs(error, stderr);
        exit(1);
    }

    char *ptr = mallocp(size);  /* call malloc of glibc */
    /* 这里增加count的作用是因为printf()函数也会调用malloc，
     * 导致了递归调用，调用栈溢出
     */
    if (count == 1){
        printf("malloc(%d) = %p\n", (int)size, ptr);
    }
    count = 0;
    return ptr;
}

void free(void *ptr)
{
    void (*freep)(void *ptr);
    char *error;

    if (!ptr)
        return;

    freep = dlsym(RTLD_NEXT, "free");
    if ((error = dlerror()) != NULL){
        fputs(error, stderr);
        exit(1);
    }
    freep(ptr);
    printf("free(%p)\n", ptr);
    return;
}
#endif