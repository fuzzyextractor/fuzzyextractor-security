#include<bits/stdc++.h>
//#include "omp.h"
#include <mpi.h>
#include "leech.hpp"
#define DIS_MSG 6
#define RESULT_MSG 7
#define TERMINATE_MSG 8

using namespace std;
const int maxn=20000;
vector<string>name;
vector<double>vec[maxn];


const int n=512;




void manager(int argc, char * argv[], int p);
void worker(int argc, char * argv[]);







typedef long long LL;
 

const long long alpha=1<<20;

double dis(const vector<double>&vec1,const vector<double>&vec2){
    
    //double in[n],out[n];
    vector<double>in,out;
    in.resize(n);
    out.resize(n);
    for(int i=0;i<n;i++)
        in[i]=(vec1[i]-vec2[i])*alpha;
 
 
    double l=0.001,r=1;
    int T=20;
    for(int t=1;t<=T;t++){

        double mid=(l+r)/2;

        long long scale=mid*alpha;
        scale+=scale%2; 

        //out=decode(in,0,scale); 
        Leech::decode(out.data(),in.data(),scale);

        bool zero=true;
        for(int i=0;i<n;i++)
            if(fabs(out[i])>1e-6){
                zero=false;
            }
        if(zero)
            r=mid;
        else
            l=mid;        

    }

    return l;
}





string trim(string name){
    string n;
    for(int i=0;i<name.length()-9;i++)
        n+=name[i];
    return n;
}
 
 


int coll(int id,double t){
    int cnt=0;
    for(int i=0;i<(int)name.size();i++){
        if(name[i]==name[id])continue;
        if(dis(vec[i],vec[id])<t)
            cnt++;
    }
    return cnt;
}

const int THREAD_NUM=12;


void thread_dis(const vector<double>&vec1,const vector<double>&vec2,double *res){
    *res=dis(vec1,vec2);
}

pair<double,double> distri(int id, int p){
    int src, tag;
    MPI_Status status;	/* Message status */
    char send_buf[sizeof(int) + 2*sizeof(double)*n]; // i + vec[i] + vec[id]
    char recv_buf[sizeof(double) + sizeof(int)]; // i + distance
    int return_index;

    double sum=0,sig=0;
    int cnt=0;
    vector<double>d;
    d.resize(name.size());

    // start the job
    int terminated = 0;
    int processed = min(id+1,name.size()-p-10);
    for (int i = 1; i < p; i++) {
        memcpy(send_buf, &processed, sizeof(int));
        memcpy(send_buf+sizeof(int),vec[processed].data(), n*sizeof(double));
        memcpy(send_buf+sizeof(int)+sizeof(double)*n,vec[id].data(), sizeof(double)*n);
        MPI_Send(send_buf, sizeof(int) + 2*sizeof(double)*n, MPI_CHAR, i, DIS_MSG, MPI_COMM_WORLD);
        processed++;
    }
    while (terminated < p-1) {
        MPI_Recv(&recv_buf, sizeof(double) + sizeof(int), MPI_CHAR, MPI_ANY_SOURCE, RESULT_MSG, MPI_COMM_WORLD, &status);
        src = status.MPI_SOURCE;
        tag = status.MPI_TAG;
        memcpy( &return_index, recv_buf, sizeof(int));
        memcpy( &d[return_index], recv_buf+sizeof(int), sizeof(double));
        cerr<< return_index <<" "<<d[return_index]<<endl;
        //cerr << return_index << " " << flush;

        if (processed < (int)name.size()) {
            memcpy(send_buf, &processed, sizeof(int));
            memcpy(send_buf+sizeof(int), vec[processed].data(), sizeof(double)*n);
            memcpy(send_buf+sizeof(int)+sizeof(double)*n, vec[id].data(), sizeof(double)*n);
            MPI_Send(send_buf, sizeof(int) + 2*sizeof(double)*n, MPI_CHAR, src, DIS_MSG, MPI_COMM_WORLD);
            processed++;
        } else {
            //MPI_Send(send_buf, sizeof(int) + 2*sizeof(double)*n, MPI_CHAR, src, TERMINATE_MSG, MPI_COMM_WORLD);
            terminated++;
        }
    }

    // job finished
    cout<<name[id]<<" ";
    for(auto x:d){
        sum+=x;
        cout<<x<<" ";
    }
    cout<<endl;

    double mu=sum/cnt;

    for(auto x:d){
        sig+=(x-mu)*(x-mu);
    }
    sig=sqrt(sig/cnt);

    return make_pair(mu,sig);
}

int count(int id){
    int cnt=0;
    for(int i=0;i<(int)name.size();i++){
        if(name[i]==name[id])
            cnt++;
    }
    return cnt;
}

double prob(double mu,double sig,double t){
    return 0.5+erf((t-mu)/sig/sqrt(2))/2;
}

void manager(int argc, char * argv[], int p){
    ifstream fin("vec3");
    string nm;
    int c=0;
    while(fin>>nm){
        name.push_back(trim(nm));
        vec[c].resize(n);
        for(int i=0;i<n;i++)
            fin>>vec[c][i];
        c++;
    }
    int all=name.size(); 
    vector<double>di;
    for(int i=0;i<all;i++) {
        auto pr=distri(i, p);  
    } 

    char send_buf[sizeof(int) + 2*sizeof(double)*n]; // i + vec[i] + vec[id]
    memset(send_buf, 0, sizeof(send_buf));
    for(int i=0; i<p; i++)
        MPI_Send(send_buf, sizeof(int) + 2*sizeof(double)*n, MPI_CHAR, 0, TERMINATE_MSG, MPI_COMM_WORLD);

}


void worker(int argc, char * argv[]){
    char recv_buf[sizeof(int) + 2*sizeof(double)*n]; // i + vec[i] + vec[id]
    char send_buf[sizeof(double) + sizeof(int)]; // i + distance
    int tag, src;
    int terminate = 0;
    int index;
    vector<double> v1,v2;
    v1.resize(n);
    v2.resize(n);
    double distance;
    MPI_Status status;

    // main loop
    while (1) {
        MPI_Recv(recv_buf, sizeof(int) + 2*sizeof(double)*n, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        src = status.MPI_SOURCE;
        tag = status.MPI_TAG;
        switch (tag) {
            case TERMINATE_MSG:
                terminate = 1;
                break;
            case DIS_MSG:
                memcpy(&index, recv_buf, sizeof(int));
                memcpy(v1.data(), recv_buf+sizeof(int), sizeof(double)*n);
                memcpy(v2.data(), recv_buf+sizeof(int)+sizeof(double)*n, sizeof(double)*n);
                distance = dis(v1,v2);
                memcpy(send_buf, &index, sizeof(int));
                memcpy(send_buf+sizeof(int), &distance, sizeof(double));
                MPI_Send(&send_buf, sizeof(int)+sizeof(double), MPI_CHAR, src, RESULT_MSG, MPI_COMM_WORLD);
                break;
            default:
                break;
        }
        if (terminate) {
            break;
        }
    }
}




int main(int argc, char* argv[]){


    int id, p;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    if (p < 2) {
        cout << "Only 1 process, aborting." << endl;
    } else {
        if (id == 0)
            manager(argc, argv, p);
        else
            worker(argc, argv);
    }
    MPI_Finalize();
    return 0;

}
