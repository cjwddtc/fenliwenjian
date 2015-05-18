#include <stdio.h>

int find_match(char first_char,char second_char,unsigned char direct,char *source,char **first,char **second)
{
#define inc(a) if(direct) a++;else a--;
    if(source==0) return 3;
    while(*source !=first_char)
    {
        inc(source);
        if(*source==0){
            *first=source;
            return 1;
        }
    }
    if(first!=0) *first=source;
    int n=-1;
    while(n!=0 || *source!=second_char)
    {
        if(*source==first_char) n++;
        else if(*source==second_char) n--;
        inc(source)
        if(*source==0){
            *first=source;
            return 2;
        }
    }
    if(second!=0) *second=source;
    return 0;
}

char *copystr(char *start,char *finish,char *des){
    while(start!=finish) *des++=*start++;
    return des;
}
