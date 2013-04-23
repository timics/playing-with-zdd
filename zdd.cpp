#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include<stdio.h>
using namespace std;

class zdd
{
    public: //I made everything public for now
        vector <string> names; //Contains the names of all the nodes represented in the diagram
        vector <int> T; //Contains the index of the node pointed by the "then" branch, for all the nodes represented in the diagram
        vector <int> E; //Contains the index of the node pointed by the "else" branch, for all the nodes represented in the diagram

        zdd(); //Constructor: adds the True and False nodes
        void show(); //Prints for each node its name and index and its children (also by printing their name and index)
        void add_node(string name); //Adds a node in the diagram, setting its "then" and "else" correspondent elements to 0
        void add_child(int node, int child, char branch); //Adds a "then" or an "else" edge from a parent to a child
        void remove_identical_subtrees(); //Removes the nodes with the same name and the same children

};

zdd::zdd()
{
    names.push_back("False");
    T.push_back(-1);
    E.push_back(-1);

    names.push_back("True");
    T.push_back(-1);
    E.push_back(-1);
}

void zdd::show()
{
    for(int i=2;i<names.size();++i)
    {
        cout<<names[i]<<"("<<i<<") "<<names[T[i]]<<"("<<T[i]<<") "<<names[E[i]]<<"("<<E[i]<<")\n";
    }
}

void zdd::add_node(string name)
{
    names.push_back(name);
    T.push_back(-1);
    E.push_back(-1);
}

void zdd::add_child(int node, int child, char branch)
{
    if(branch=='T')
    {
        T[node]=child;
    }
    else if(branch=='E')
    {
        E[node]=child;
    }
}

void zdd::remove_identical_subtrees()
{
    for(int i=0;i<T.size()-1;++i)
        for(int j=i+1;j<T.size();++j)
            if(T[i]==T[j] && E[i]==E[j] && names[i]==names[j])
            {
                T.erase(T.begin()+i);
                E.erase(E.begin()+i);
                names.erase(names.begin()+i);
            }
    return;
}

//The following function takes an integer n representing the number of variables and a string that represents a sum of products of variables x0,x1,...xn-1,
//and returns a vector of binary strings, such as each product is represented by a string a of n 0s and 1s (the i-th character is "1" iff xi is part of the product).
vector<string> parse(string s,int n)
{
    vector<string> r;
    string a;
    for(int j=0;j<n;++j)
        a.push_back('0');

    for(int i=0;i<s.size();++i)
    {
        if(s[i]=='x')
        {
            a[s[i+1]-'0']='1';
        }

        if(s[i]=='1'&&(i==0 || s[i-1]!='x'))
            for(int j=0;j<n;++j)
                a[j]='0';

        if(s[i]=='+')
        {
            r.push_back(a);
            for(int j=0;j<n;++j)
                a[j]='0';
        }

    }
    r.push_back(a);
    return r;
}

//For the following recursive function, we need to modify a global variablem which we initialize here.
zdd ret=zdd();

//This function takes a vector of strings corresponding to the set of products we want to encode, an index that indicates the topmost variable, the parent of the subtree
//that we are encoding (-1 if we are at the first step), and the branch that lead to that subtree ('T' if "then", 'E' if "else", 'R' if root), and adds the root of the
//subtree to the global ret zdd (we also update the children of the parent of this subtree)

void encode(vector <string> set,int index, int parent, char branch)
{
    vector<string> set0; //The set corresponding to the "else" subtree
    vector<string> set1; //The set corresponding to the "then" subtree

    int consider_variable=0; //This variable will stay 0 iff the topmost variable does not appear in any of the products in the current set
    int nonempty_set=0; //This variable will stay 0 iff the first product is "1", which is represented as a string of 0

    if(set.size()!=0)
    {
        for(int i=0;i<set[0].size();++i)
            if(set[0][i]=='1')
            {
                nonempty_set=1;
                break;
            }

        while(consider_variable==0 && set[0].size()>=1)
        {
            for(int i=0;i<set.size();++i)
                if(set[i][0]=='1')
                {
                    consider_variable=1;
                    break;
                }

            if(consider_variable==0)
            {
                for(int i=0;i<set.size();++i)
                    set[i].erase(0,1);
                ++index;
            }
        }
    }

    if(set.size()==0) //We reached the "False" leaf
    {
        if(parent!=-1)
            ret.add_child(parent,0,branch);
    }

    else if(set.size()==1 && nonempty_set==0) //We reached the "True" leaf
    {
        if(parent!=-1)
            ret.add_child(parent,1,branch);
    }

    else
    {
        string s;
        s.push_back('x');
        s.push_back(index+'0');
        ret.add_node(s); //We add the node corresponding to the topmost variable

        if(parent!=-1)
            ret.add_child(parent,ret.names.size()-1,branch);

        for(int i=0;i<set.size();++i)
        {
            if(set[i][0]=='0')
            {
                set[i].erase(0,1);
                set0.push_back(set[i]);
            }
            else
            {
                set[i].erase(0,1);
                set1.push_back(set[i]);
            }
        }

        int par=ret.names.size()-1;

        encode(set1,index+1,par,'T');

        encode(set0,index+1,par,'E');


    }

    return;
}

void true_encode(string s,int n) //A function that mixes previously defined functions, so that we do not need to call them
{
    vector<string> v=parse(s,n);
    encode(v,0,-1,'R');
    ret.remove_identical_subtrees();
}


int main()
{
    int n;
    string s;

    /*ifstream fin;
    fin.open("input.in");
    fin>>n>>s;*/

    cin>>n>>s;

    true_encode(s,n);
    ret.show();
    return 0;
}
