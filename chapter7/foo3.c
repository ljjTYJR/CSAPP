#include <stdio.h>

void f(void);
int x = 15123;

int main()
{
    f();
    printf("x = %d\n", x);
    return 0;
}