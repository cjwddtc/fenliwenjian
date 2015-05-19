#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <string.h>
#include "tool.h"

jmp_buf err;

struct CLASS
{
    char *def;
    char *dec;
};

char *catch_class(char *b,struct CLASS *cls)
{
    int flag=0;
    char class_name[128];
    char *name_ptr=class_name;
    int size;
    char *a=b,*c,*d,*e;
    char *def=cls->def;
    char *dec=cls->dec;
    if(strncmp(b,"class",5)!=0)
        longjmp(err,1);
    b+=5;
    while(isspace(*b))  b++;//find the start of the class name
    while(!isspace(*b) && *b!='{')
    {
        if(*b==';')
        {
            b++;
            dec=copystr(a,b,dec);
            cls->dec=dec;
            cls->def=def;
            return b;
        }
        *name_ptr++=*b++;//save the class name
    }
    while(isspace(*b)) b++;
    if(*b==';')
    {
        b++;
        dec=copystr(a,b,dec);
        cls->dec=dec;
        cls->def=def;
        return b;
    }
    else if(*b=='{') b++;
    else longjmp(err,8);
    dec=copystr(a,b,dec);
    *name_ptr=0;//save the finish position
    a=b;
    size=name_ptr-class_name;

    while(1)
    {
        b=a;
        while(*b!='{')
        {
            if(*b=='}')
            {
                b++;
                dec=copystr(a,b,dec);
                cls->dec=dec;
                cls->def=def;
                return b;
            }
            b++;
        }
        int n=find_match('{','}',1,b,&c,&d);
        d++;
        if(n==2) longjmp(err,7);
        else if(n==1) longjmp(err,8);

        while(isspace(*--c));//pass space
        b=c;
        if(*b==')')
        {
            find_match(')','(',0,b,0,&b);
            while(isspace(*b)) b--;//find the name of the function
            e=b;
            while(*b!='*' && !isspace(*b)) b--;//find start of the name
            b++;
            if(*(b)=='`')
                if( e-b-1!=size || strncmp(b-1,class_name,size))
                    flag=1;
                else
                    flag=0;
            else if(e-b!=size || strncmp(b,class_name,size))
                flag=1;//whether function have return
            else
                flag=0;
            e=b;
            if(flag)
            {
                b--;
                while(*b=='*' || isspace(*b)) b--;//find the return type of the name
                while(!isspace(*--b)) ;//find the start of the function
                b++;
            }
            dec=copystr(a,c,dec);
            *dec++=';';
            if(flag)
            {
                def=copystr(b,e,def);
            }
            def=copystr(class_name,name_ptr,def);
            *def++=':';
            *def++=':';
            def=copystr(e,d,def);
            *def++='\n';
            a=d;
        }
        else longjmp(err,3);
    }
}

void analys(char *a,struct CLASS *cls)
{
    char *b,*c,*d;
    char *end=(char *)1;
    while(1)
    {
        if(end!=0) end=strstr(a,"class");
        int n=find_match('{','}',1,a,&c,&d);
        if(n==2) longjmp(err,5);
        else if(n==1)
        {
            cls->dec=copystr(a,c,cls->dec);
            return ;
        }
        if(end!=0 && c>end)
        {
            cls->dec=copystr(a,end,cls->dec);
            a=catch_class(end,cls);
        }
        else
        {
            b=c;
            d++;
            while(isspace(*--b));
            if(*b==')')
            {
                int n=find_match(')','(',0,b,0,&b);
                if(n) longjmp(err,6);
                while(isspace(*--b));
                while(!isspace(*b) && *b!='*') b--;
                while(isspace(*b) || *b=='*') b--;
                while(!isspace(*b))b--;
                b++;
                cls->dec=copystr(a,c,cls->dec);
                cls->def=copystr(b,d,cls->def);
                *(cls->dec)++=';';
                *(cls->def)++='\n';
                a=d;
            }
            else
            {
                cls->dec=copystr(a,d,cls->dec);
                a=d;
            }
        }
    }
}

void fenli(FILE *source,FILE *f_dec,FILE *f_def)
{
    fseek(source,0,2);
    int size=ftell(source);
    if(size==0) return ;
    struct CLASS a;
    a.dec=(char *)malloc(size);
    a.def=(char *)malloc(size);
    struct CLASS ci=a;
    char *sources=(char *)malloc(size);
    rewind(source);
    int n=fread(sources,1,size,source);
    *(sources+n)=0;
    char *p=strstr(sources,"//");
    while(p)
    {
        while(*p!='\n') *p++=' ';
        p=strstr(p,"//");
    }
    p=strstr(sources,"/*");
    while(p)
    {
        char *q=strstr(p,"*/");
        if(q==0) longjmp(err,5);
        while(p!=q) *p++=' ';
        *p++=' ';
        *p++=' ';
        p=strstr(p,"/*");
    }
    analys(sources,&a);
    fwrite(ci.dec,1,a.dec-ci.dec,f_dec);
    fwrite(ci.def,1,a.def-ci.def,f_def);
    free(ci.dec);
    free(ci.def);
    fflush(f_dec);
    fflush(f_def);
};

int main(int args,char *argv[])
{
    FILE *a,*b,*c;
    a=fopen("asd.c","r");
    b=fopen("dec.c","w");
    c=fopen("def.c","w");
    if(!a)
    {
        printf("wrong 1");
        return 0;
    }
    if(!b)
    {
        printf("wrong 2");
        return 0;
    }
    if(!c)
    {
        printf("wrong 3");
        return 0;
    }
    int n;
    if(n=setjmp(err))
    {
        printf("err:%d\n",n);
        getchar();
    }
    fenli(a,b,c);
    return 0;
}
