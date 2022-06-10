#include <iostream>
#include<cstring>
#include<cstdio>
#include"BPlustree.hpp"
struct test{
    char x[66];
    void operator =(const test &y){
        strcpy(x,y.x);
    }
    bool friend operator <(const test &n1,const test &y){
        if(strcmp(n1.x,y.x)<0)return true;
        return false;
    }
    friend ostream &operator<<( ostream &output,
                                const test &t )
    {
        output << t.x;
        return output;
    }
    test(){}
};
int main() {
//    freopen("my.in","r",stdin);
//    freopen("my.out","w",stdout);
int n;
cin >> n;
lailai::BPT<test> bpt("totalleave");
for(int i = 1; i <= n; ++i){
    string s0;
    cin >> s0;
    test t;
    if(s0[0]=='i'){
        string s;
        cin >> s;
        strcpy(t.x,s.c_str());
        ll index;
        cin >> index;
        bpt.Insert(t,index);
    }
    else if(s0[0]=='f'){
        vector<ll> v;
        string s;
        cin >> s;
        strcpy(t.x,s.c_str());
        bpt.Get(t,v);
        if(v.empty())cout << "null"<< endl;
        else {
            for(auto j = v.begin(); j!= v.end(); ++j){
                cout << *j << ' ';
            }
            cout << endl;
        }
    }
    else if(s0[0]=='d'){
        ll s1;
        string s;
        cin >> s >> s1;
        strcpy(t.x,s.c_str());
//        cout << "remove :" << s << endl;
        bpt.Remove(t,s1);
    }

}
//cout << remove("totalleave")<< endl;
}
