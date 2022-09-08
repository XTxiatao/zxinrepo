#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 把判断后续有多少个连续的0的逻辑分离出来 */
int zeroTimes(int order, const unsigned long int *arr, size_t arr_size)
{
    int res = 0;
    for (int i = order; i < arr_size; i++)
    {
        if (arr[i] == 0)
        {
            res = (res + 1)%2;
            continue;
        }
        break;
    }
    return res;
}
/* 要修改1、5、6后续可能为0时的结果 */
int last_digit(const unsigned long int *arr, size_t arr_size) {
    if (NULL == arr)
    {
        return -1;
    }
    if (arr_size == 0)
    {
        return 1;
    }
    if (arr_size == 1)
    {
        return arr[0];
    }
    int base = arr[0]%10;
    if (base == 0)
    {
        return zeroTimes(1, arr, arr_size);
    }
    if (base == 1 || base == 5 || base == 6)
    {
        return base;
    }
    if (base == 4 || base == 9)
    {
        if (arr[1]%2 == 1)
        {
            return base;
        }
        if (arr[1] == 0)
        {
            int flag = 0;
            for (int i = 2; i < arr_size; i++)
            {
                if (arr[i] == 0)
                {
                    flag = (flag + 1)%2;
                    continue;
                }
                break;
            }

        }
    }

    return -1;
}

int main(void)
{
    while (1)
    {
        printf("input some numbers divided by space:\n");
        unsigned long long arr[3];
        scanf("%lld %lld %lld", arr, arr+1, arr+2);
        int res = last_digit(arr, 3);
        printf("the last digit is: %d\n", res);
    }

    return 0;
}
