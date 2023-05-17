#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <limits.h>
#include <stdlib.h>
#include <math.h>

#define MAX_PATH 260

unsigned int hash(char *str, size_t m)
{
    unsigned int h = 0;
    int d = 13;
    for (int i = 0; *str != '\0' && i < m; i++)
    {
        h += pow(d, m) * str[i];
    }
    return h;
}

char *rabinKarp(char *pattern, char *text)
{
    int d = 13;
    int m = strlen(pattern);
    int n = strlen(text);
    unsigned int p = hash(pattern, m);
    unsigned int t = hash(text, m);
    int isMatch = 0;
    for (int i = 0; i <= n - m; i++)
    {
        if (p == t)
        {
            for (int j = 0; j < m; j++)
            {
                if ((text[i + j]) == (pattern[j]))
                    isMatch++;
                else
                    break;
            }
            if (isMatch == m)
                return text;
        }
        if (i < n - m)
        {
            t = (t - text[i] * pow(d, m)) + (text[i + m] * pow(d, m));
        }
    }
    return NULL;
}

int is_previous_path(char **str)
{
    size_t before = strlen(*str);
    for (size_t i = strlen(*str) - 1; i > 0; i--)
    {
        if ((*str)[i] == '/')
        {
            (*str)[i] = '\0';
            break;
        }
    }
    size_t after = strlen(*str);
    if (before == after)
        return 0;
    else
        return 1;
}

int check_str(char *str)
{
    int count = 0;
    for (size_t i = 0; str[i] != '\0'; i++)
    {
        if ((str)[i] == '*')
            count++;
    }
    if ((str[strlen(str) - 1] == '*') || (str[0] == '*'))
        return count;
    else if (count > 0)
        return count + 1;
    else
        return 0;
}

char *dir_check(const char *path, char *pattern)
{
    int astCount = check_str(pattern);
    printf("%d\n", astCount);
    char *tmp = NULL;
    int count = 0;
    char *result = NULL;
    struct dirent *entry;
    DIR *dp;
    dp = opendir(path);
    if (dp == NULL)
    {
        perror("opendir");
        return NULL;
    }
    while ((entry = readdir(dp)))
    {
        strtok(pattern, "*");
        count = 0;
        if (!astCount)
        {
            result = rabinKarp(pattern, entry->d_name);
            if (result != NULL)
            {
                printf("Найдено совпадение:\n%s\n", result);
                continue;
            }
        }
        else
        {
            while (tmp != NULL)
            {
                printf("%s %s\n", tmp, entry->d_name);
                result = rabinKarp(tmp, entry->d_name);
                if (result != NULL)
                {
                    count++;
                }
                tmp = strtok(NULL, "*");
            }
            printf("%d %d\n", count, astCount);
            if (count == astCount)
                printf("Найдено совпадение:\n%s\n", entry->d_name);
        }
    }
    closedir(dp);
    return result;
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Неверное количество аргументов\n");
        return -1;
    }
    char *dir = malloc(MAX_PATH);
    char *pat = NULL;
    if (!(strcmp(argv[1], "-r")))
    {
        realpath(argv[3], dir);
        pat = argv[2];
        do
        {
            printf("%s:\n", dir);
            dir_check(dir, pat);
        } while (is_previous_path(&dir));
    }
    else
    {
        realpath(argv[2], dir);
        pat = argv[1];
        printf("%s:\n", dir);
        dir_check(dir, pat);
    }
    free(dir);
    return 0;
}
