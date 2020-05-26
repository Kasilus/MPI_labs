#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char* concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
    // in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

int main() {
    int a = 4;
    int b = 3;
    int c = a/b;
    printf("%d\n", c);
}
