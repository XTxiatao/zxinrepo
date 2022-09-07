#include <stdio.h>

typedef 
char* decompose(long long n) {
    if (n <= 0)
    {
        return NULL;
    }


}

int main(void)
{
    long long n = 0;
    while(1)
    {
        printf("please input a positive number:");
        scanf("%lld", n);
        char *deComposers = decompose(n);
        if (NULL == deComposers)
        {
            printf("not a positive number!\n");
            continue;
        }
        printf("%s\n", deComposers);
        free(deComposers);
    }

    return 0;
}
