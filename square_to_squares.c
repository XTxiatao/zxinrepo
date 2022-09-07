#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct composer
{
    long long data;
    long long rest;
    struct composer *next;
}Composer;

Composer* newComposer(long long data, Composer *lastComposer)
{
    if (data <= 0)
    {
        return NULL;
    }
    Composer *newComposer = (Composer *)malloc(sizeof(Composer));
    if (NULL == newComposer)
    {
        return NULL;
    }
    if (lastComposer == NULL)
    {
        return NULL;
    }
    newComposer->rest = lastComposer->rest - data * data;
    newComposer->data = data;
    newComposer->next = lastComposer;
    printf("created composer with data: %lld    rest: %lld\n", data, newComposer->rest);
    return newComposer;
}
Composer* initComposer(long long n)
{
    if (n <= 0)
    {
        return NULL;
    }
    Composer *newComposer = (Composer *)malloc(sizeof(Composer));
    if (NULL == newComposer)
    {
        return NULL;
    }
    newComposer->rest = n * n;
    newComposer->data = 0;
    newComposer->next = NULL;
    printf("init composer, rest: %lld\n", n*n);
    return newComposer;
}
int deleteComposer(Composer* composer)
{
    if (NULL == composer)
    {
        return -1;
    }
    printf("deleted composer with data :%lld from composers\n", composer->data);
    free(composer);
    return 0;
}
char* decompose(long long n) {
    printf("start decomposer\n");
    if (n <= 0)
    {
        return NULL;
    }
    int composers = 0;
    Composer *lastComposer = initComposer(n);
    for (long long lastData = n-1; lastData >= n/2; lastData--)
    {
        lastComposer = newComposer(lastData, lastComposer);
        composers++;
        while(1)
        {
            if (lastComposer->rest < 1)
            {
                break;
            }
            long long nextData = 1;
            while (nextData * nextData <= lastComposer->rest)
            {
                nextData++;
            }
            nextData--;
            if (nextData >= lastComposer->data)
            {
                Composer* tmp = lastComposer;
                if (lastComposer->next->next == NULL)
                {
                    deleteComposer(tmp->next);
                    deleteComposer(tmp);
                    composers--;
                    break;
                }
                lastComposer = newComposer(lastComposer->next->data-1,\
                                                     lastComposer->next->next);
                deleteComposer(tmp->next);
                deleteComposer(tmp);
                composers --;
                continue;
            }
            lastComposer = newComposer(nextData, lastComposer);
            composers++;
        }
        if (lastComposer->data == 0)
        {
            continue;
        }
        break;
    }
    printf("composers number: %d\n", composers);
    if (composers == 0)
    {
        char *res = (char *)malloc(4);
        sprintf(res, "%s","[]");
        return res;
    }
    char *res = (char *)malloc(20*composers);
    if (NULL == res)
    {
        return NULL;
    }
    char buff[20] = {0};
    snprintf(res, 20, "%s", "[");
    while (lastComposer->next->next != NULL)
    {
        snprintf(buff, 20, "%lld, ", lastComposer->data);
        strcat(res, buff);
        Composer* tmp = lastComposer;
        lastComposer = lastComposer->next;
        deleteComposer(tmp);
    }
    snprintf(buff, 20, "%lld]", lastComposer->data);
    strcat(res, buff);
    Composer* tmp = lastComposer;
    lastComposer = lastComposer->next;
    deleteComposer(tmp);
    return res;
}

int main(void)
{
    long long n = 0;
    while(1)
    {
        printf("please input a positive number:");
        scanf("%lld", &n);
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
