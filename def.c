node::node(const char *name,node *father=0)
    {
        this->name=name;
        this->is_dir=true;
        this->value.sonlist=new std::list<node*>();
        this->father=father;
    }
node::node(const char *name,long n,node *father=0)
    {
        this->name=name;
        this->is_dir=false;
        this->value.n=n;
        this->father=father;
    }
bool node::operator<(node &a)
    {
        return this->name.compare(a.name)<0;
    }
bool node::operator>(node &a)
    {
        return this->name.compare(a.name)>0;
    }
bool node::operator==(node &a)
    {
        if(this->is_dir==a.is_dir)
            if(this->name.compare(a.name)==0)
            {
                if(!this->is_dir)
                {
                    if(this->value.n==a.value.n)
                        return true;
                }
                else return true;
            }
        return false;
    }
bool node::operator!=(node &a)
    {
        return !(*this==a);
    }
