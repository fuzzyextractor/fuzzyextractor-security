#include <bits/stdc++.h> 
#include "leech.hpp"

using namespace std;
const int maxn=20000;
vector<string>name;
vector<double>vec[maxn];


const int n=512;



typedef long long LL;


const long long alpha=1<<20; 


unsigned long long h(long long *out){
    unsigned long long x=0,y=0,z=0;
    for(int i=0;i<n;i++){
        x=x*233+out[i];
        y=y^out[i];
        z=z+out[i];
    }
    return x^y^z;
}

string trim(string s){
    int T=9;
    while(T--)
        s.pop_back();
    return s;
}

int all=0;


void decode_E8(double *out,double *in,long long scale){
    const int n=512;
    const int num=512/8;
    int in_[n],out_[n];
    memset(in_,0,sizeof(in_));

    for(int i=0;i<n;i++){
        in_[i]=in[i];
    }
    for(int i=0;i<num;i++){
        decode_4E8(out_+i*8,in_+i*8,scale);
    }
    for(int i=0;i<n;i++){
        out[i]=out_[i];
    }
}


void decode_Z(double *out,double *in,long long scale){
    const int n=512; 
    int in_[n],out_[n];
    memset(in_,0,sizeof(in_));

    for(int i=0;i<n;i++){
        in_[i]=in[i];
    }
    for(int i=0;i<n;i++){
        out_[i]=round(in_[i],scale);
        //decode_4E8(out_+i*8,in_+i*8,scale);
    }
    for(int i=0;i<n;i++){
        out[i]=out_[i];
    }
}


double max_prob(double t){
    map<unsigned long long,int>M;

    for(int j=0;j<all;j++){
        double _in[512],_out[512];
        long long out[512];

        for(int i=0;i<n;i++)
            _in[i]=round(vec[j][i]*alpha);
        long long scale=t*alpha; 
        decode(_out,_in,scale);
        for(int i=0;i<n;i++)
            out[i]=_out[i];

        //for(int i=0;i<n;i++)
        //   printf("%.0f%c",1.0*out[i]/scale," \n"[i+1==n]);
        M[h(out)]++;
        //cout<<all<<" "<<j<<" "<<M.size()<<endl;
    }
    int mx=0;
    for(auto it=M.begin();it!=M.end();it++){
        if(mx<it->second){
            mx=it->second;
        }
    }

    /*for(auto x:list)
        cout<<name[x]<<endl;
    cout<<hh<<endl;
    cout<<all<<" "<<mx<<" "<<M.size()<<endl;*/
    if(mx==1)
        return -1;

    double p=1.0*mx/all;
    //cout<<mx<<" "<<M.size()<<endl;
    return p;
}

int main(int argc, char* argv[]){
    ifstream fin("vec_insight_lfw");
	vector<string>name;
	string nm;
    set<string>D;
	while(fin>>nm){

        nm=trim(nm);
        if(D.count(nm)){
            double x;
            for(int i=0;i<512;i++)
                fin>>x;
            continue;
        }
        D.insert(nm);

		name.push_back(nm);
        vec[all].resize(512);
		for(int i=0;i<512;i++){
			fin>>vec[all][i];
        }
        all++;
	}
    fin.close();


    double t_mn=0.066;
    double t_mx=0.090;

    double t=t_mn;


    int T=29;
    for(int i=0;i<=T;i++){
        double t=t_mn+(t_mx-t_mn)/T*i;
        cout<<max_prob(t)<<","<<flush;
    }
    

    return 0;
}
