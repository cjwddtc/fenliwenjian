#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <string.h>

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
    char *a=b,*c;
    char *def=cls->def;
    char *dec=cls->dec;
    if(strncmp(b,"class",5)!=0)
        longjmp(err,1);
    b+=5;
    while(isspace(*b))  b++;//find the start of the class name
    while(!isspace(*b) && *b!='{') *name_ptr++=*b++;//save the class name
    *name_ptr=0;//save the finish position
    while(*b!='{')
    {
        if(*b==';') {
            char *dec=cls->dec;
            while(a!=b) *dec++=*a++;
            *dec++=';';
            return ++b;
        }
        b++;
    }
    char *d=++b;//find start of class body and save
    while(a!=b) *dec++=*a++;//save above this to dec
    while(1)
    {
        c=b;
        while(*b!='{')
        {
            if(*b=='}')
            {
                b++;
                while(c!=b) *dec++=*c++;
                cls->dec=dec;
                cls->def=def;
                return b;
            }
            b++;
        }
        while(isspace(*--b));//pass space
        c=b+1;//find start of define and save
        if(*b==')')
        {
            char *e;
            while(*b--!='(');//find the (of the function
            while(isspace(*b)) b--;//find the name of the function
            e=b;
            while(*b!='*' && !isspace(*b)) b--;//find start of the name
            b++;
            int size=name_ptr-class_name;
            if(*(b)=='`')
                if( e-b!=size || strncmp(b-1,class_name,size))
                    flag=1;
                else
                    flag=0;
            else if(e-b+1!=size || strncmp(b,class_name,size))
                flag=1;//whether function have return
            else
                flag=0;
            if(flag)
            {
                e=b;
                b--;
                while(*b=='*' || isspace(*b)) b--;//find the return type of the name
                while(!isspace(*--b)) ;//find the start of the function
                b++;
            }
            //find the finish of the function
            while(d!=c)  *dec++=*d++;//save the above this to dec
            *dec++=';';
            int n=-1;
            while(n!=0 || *c!='}')
            {
                if(*c=='{') n++;
                if(*c=='}') n--;
                c++;
                if(*c==0)
                {
                    longjmp(err,2);
                }
            }
            if(flag)
                while(b!=e) *def++=*b++;//write the return type to the def
            char *start_name=class_name;
            while(start_name!=name_ptr) *def++=*start_name++;//write the class name
            *def++=':';
            *def++=':';//write ::
            c++;
            while(b!=c) *def++=*b++;//write function body
            *def++='\n';
            b=c;
            d=b;
        }
        else longjmp(err,3);
    }
}

void analys(char *a,struct CLASS *cls)
{
    char *b;
    char *end=(char *)1;
    while(1)
    {
        b=a;
        if(end!=0) end=strstr(b,"class");
        while(*b!='{')
        {
            if(*b==0)
            {
                char *dec=cls->dec;
                while(a!=b) *dec++=*a++;
                cls->dec=dec;
                return;
            }
            b++;
        }
        if(end!=0 && b>end)
        {
            char *dec=cls->dec;
            while(a!=end) *dec++=*a++;
            cls->dec=dec;
            a=catch_class(end,cls);
        }
        else
        {
            char *d=b;
            while(isspace(*--b));
            if(*b==')')
            {
                char *c=b+1;
                while(*--b=='(');
                while(isspace(*--b));
                while(!isspace(*b) && *b!='*') b--;
                while(isspace(*b) || *b=='*') b--;
                while(!isspace(*b))b--;
                b++;
                char *dec=cls->dec;
                char *def=cls->def;
                int n=-1;
                while(n==0 && *d=='}')
                {
                    if(*d=='}') n--;
                    if(*d=='{') n++;
                    d++;
                    if(*d==0) longjmp(err,4);
                }
                d++;
                while(a!=c) *dec++=*a++;
                while(b!=d) *def++=*b++;
                *def++='\n';
                cls->dec=dec;
                cls->def=def;
                a=d;
            }
            else
            {
                int n=-1;
                while(n==0 && *d=='}')
                {
                    if(*d=='}') n--;
                    if(*d=='{') n++;
                    d++;
                    if(*d==0) longjmp(err,4);
                }
                d++;
                char *dec=cls->dec;
                while(a!=d) *dec++=*a++;
            }
        }
    }
}

void fenli(FILE *source,FILE *f_dec,FILE *f_def)
{
    fseek(source,0,2);
    int size=ftell(source);
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
    fflush(f_dec);
    fflush(f_def);
};

int main(int args,char *argv[])
{
    FILE *a,*b,*c;
    a=fopen(argv[1],"r");
    b=fopen(argv[2],"w");
    c=fopen(argv[3],"w");
    fenli(a,b,c);
    return 0;
}
