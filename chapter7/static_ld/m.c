// void swap();

#ifdef DEBUG
#include <stdio.h>
int test[10000] = {0};
#endif

int buf[2] = {1, 2};

int main()
{
    // swap();
#ifdef DEBUG
    int i = 0;
    for (i = 0; i < (sizeof(test) / sizeof(int)); i++){
        printf("%d", test[i]);
    }
#endif
    return 0;
}