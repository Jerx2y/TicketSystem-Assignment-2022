#include <iostream>
#include<cstring>
#include<cstdio>
#include"BPlustree.hpp"
struct test{
    char x[65];
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
    const ll l =92233720368547758;
//    freopen("1.in","r",stdin);
//    freopen("my.ans","w",stdout);
int n;
cin >> n;
lailai::BPT<int,int> bpt("totalleave");
for(int i = 1; i <= n; ++i){
    string s0;
    cin >> s0;
    if(s0[0]=='i'){
//        string x;
int x;
        cin >> x;
//        cout << x << "**" << endl;
//        test t;
//        strcpy(t.x,x.c_str());
        ll index;
//        cout << t <<"UU"<< endl;
        cin >> index;
        cout << "insert key " << x << endl;
        if(x==49){

        }
        bpt.Insert(x,index);
    }
    else if(s0[0]=='f'){
        vector<ll> v;
        int s;
        cin >> s;
//        test t;
//        strcpy(t.x,s.c_str());
        bpt.Get(s,v);
        if(v.empty())cout << "null"<< endl;
        else {
            for(auto j = v.begin(); j!= v.end(); ++j){
                cout << *j << ' ';
            }
            cout << endl;
        }
    }
    else if(s0[0]=='d'){
        int s;
        cin >> s;
//        test t;
//        strcpy(t.x,s.c_str());
        bpt.Remove(s);
    }

}
cout << remove("totalleave")<< endl;
}
