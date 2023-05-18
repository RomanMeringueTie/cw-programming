#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <math.h>

#define MAX_PATH 260

unsigned int hash(char *str, size_t m)
{
    unsigned int h = 0;
    int d = 13;
    for (int i = 0; *str != '\0' && i < m; i++)
    {
        h += str[i] * pow(d, m);
    }
    return h;
}

char *rabinKarp(char *pattern, char *text, int *index)
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
            {
                *index = i;
                return text;
            }
        }
        if (i < n - m)
        {
            t = t - text[i] * pow(d, m) + text[i + m] * pow(d, m);
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

int dir_check(const char *path, char *pattern)
{
    int index = 0;
    int prevIndex = 0;
    int ret = 0;
    int astCount = check_str(pattern);
    int isPattern = 0;
    char *copy = malloc(strlen(pattern));
    strcpy(copy, pattern);
    char *result = NULL;
    struct dirent *entry;
    DIR *dp;
    dp = opendir(path);
    if (dp == NULL)
    {
        perror("opendir");
        return -1;
    }
    while ((entry = readdir(dp)))
    {
        if (!astCount)
        {
            result = rabinKarp(pattern, entry->d_name, &index);
            if (result != NULL)
            {
                printf("Найдено совпадение: %s\n", result);
                ret++;
            }
        }
        else
        {
            char *tmp = NULL;
            strcpy(copy, pattern);
            isPattern = 0;
            for (tmp = strtok(copy, "*"); tmp; tmp = strtok(NULL, "*"))
            {
                result = rabinKarp(tmp, entry->d_name, &index);
                if (result != NULL)
                {
                    isPattern++;
                    if (index < prevIndex)
                        return ret;
                    prevIndex = index;
                }
            }
            if (isPattern == astCount)
            {
                printf("Найдено совпадение: %s\n", entry->d_name);
                ret++;
            }
        }
    }
    free(copy);
    closedir(dp);
    return ret;
}

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        printf("Неверное количество аргументов\n");
        return -1;
    }
    int ret = 0;
    char *dir = malloc(MAX_PATH);
    char *pat = NULL;
    if (!(strcmp(argv[1], "-r")))
    {
        realpath(argv[3], dir);
        pat = argv[2];
        do
        {
            printf("%s:\n", dir);
            ret = dir_check(dir, pat);
            if (ret == 0)
                printf("Совпадений не найдено\n");
            else if (ret == -1)
                return -1;
        } while (is_previous_path(&dir));
    }
    else
    {
        realpath(argv[2], dir);
        pat = argv[1];
        printf("%s:\n", dir);
        if (dir_check(dir, pat) == 0)
            printf("Совпадений не найдено\n");
    }
    free(dir);
    return 0;
}
