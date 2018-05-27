#include<stdio.h>
#include<stdlib.h>
#include<vector>
#include<set>
#define IN_FILE "in500.txt"
using namespace std;
struct EDGE{
  int u,v,cost,cap,flow;
  EDGE(int u,int v,int cap,int cost):u(u),v(v),cost(cost),cap(cap){
    flow=0;
  }
};
vector<EDGE> e;
vector<int>  g[1010];
int n,m,con,sercost;
vector<int>  con_id;
vector<int>  con_need;
set<int> red;
void addEdge(int u,int v,int cap,int cost){
  e.push_back(EDGE(u,v,cap,cost));
  g[u].push_back(e.size()-1);
  //printf("EDGE %d %d %d %d\n",u,v,cap,cost);
}
void read_graph(char* filename){
  FILE* f=fopen(filename,"r");
  if (f==NULL)exit(-1);
  fscanf(f,"%d%d%d%d",&n,&m,&con,&sercost);
  for (int i=0;i<m;i++){
    int u,v,cap,cost;
    fscanf(f,"%d%d%d%d",&u,&v,&cap,&cost);
    addEdge(u,v,cap,cost);
    addEdge(v,u,cap,cost);
  }
  for (int i=0;i<con;i++){
    int u,v,p;
    fscanf(f,"%d%d%d",&p,&u,&v);
    con_id.push_back(u);
    con_need.push_back(v);
    red.insert(u);
  }
  fclose(f);
}
int reduce(int u,int v,int flow){  //从edge中删去一条 u->v 流量为flow的大小
  for (int i=0;i<g[u].size();i++){
    EDGE &edge=e[g[u][i]];
    if (edge.v != v)continue;
    if (edge.v ==v &&edge.cap<flow){
        printf("u:%d v:%d edge.cap=%d flow=%d\n",u,v,edge.cap,flow);
        continue;

    }
    edge.cap-=flow;
    return edge.cost*flow;
  }
  printf("No!%d->%d : %d\n",u,v,flow);
  //exit(-3);//no edge u->v
}
int read(FILE*f){
  int ret;
  int k=fscanf(f,"%d",&ret);
  if (k==EOF){
    printf("No Enough Interger\n");
    exit(-1);
  }
  return ret;
}
vector<int> read_line(FILE* f){
  char s[1000];
  if (fgets(s,1000,f)!=s){
    printf("No Enough Interger\n");
    exit(-1);
  }
  vector<int>ret;
  ret.clear();
  char *p=s;
  while (*p!=0){
    if (*p>='0'&&*p<='9'){
      int num=0;
      while (*p>='0'&&*p<='9') {num=num*10+*p-'0';p++;}
      ret.push_back(num);
    }else
    p++;
  }
 // printf("s=%s ret.size=%d\n",s,ret.size());
  if (ret.size()==0)return read_line(f);
  else return ret;
}
void verify(char* filename){
    set<int>servers;
  int totalcost=0;
  int tolFlow=0;
  FILE *f=fopen(filename,"r");
  if (f==NULL)exit(-4);
  vector<int>path;
  path=read_line(f);
  int pathNum=path[0];
  //printf("path %d\n",pathNum);
  while (pathNum--){
     path=read_line(f);
     servers.insert(path[0]);
     for (int i=1;i<path.size()-2;i++) totalcost+=reduce(path[i-1],path[i],path[path.size()-1]);
     con_need[path[path.size()-2]]-=path[path.size()-1];
     tolFlow+=path[path.size()-1];
     //printf("消费节点%d 加流浪%d 还需要%d\n",path[path.size()-2],path[path.size()-1],con_need[path[path.size()-2]]);
  }
  printf("TotalFlow=%d TotalCost=%d\n",tolFlow,totalcost+servers.size()*sercost);
  #define min(a,b) (a<b?a:b)
  for (int i=0;i<min(con,con_need.size());i++)
  if (con_need[i]!=0){
      printf("消费节点%d   还需要%d\n",i,con_need[i]);
      exit(0);
  }
  printf("Yes!");
  fclose(f);
}
int main(int argc,char **argv){
  if (argc>1)
  read_graph(argv[1]);
  else
  read_graph(IN_FILE);//argv[1]);
  verify("result.txt");
  return 0;
}
