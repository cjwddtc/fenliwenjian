#ifndef TREE_H_INCLUDED
#define TREE_H_INCLUDED

class node;
                       
union asdf
{
    long n;
    std::list<node*> *sonlist;
};

           
class node
{
public:
    bool is_dir;
    union asdf value;
    std::string name;
    node *father;
    node(const char *name,node *father=0;
    node(const char *name,long n,node *father=0;
    bool operator<(node &a;
    bool operator>(node &a;
    bool operator==(node &a;
    bool operator!=(node &a;
};
std::string getpath(node *root);
std::string getxdpath(node *root);
void catchToTree(node *root);
void writeToFile(node *root,FILE *fp);
void readFromFile(node *root,FILE *fp);
void delall(node *root);
void print(node *root,int n=0);
#endif                    
