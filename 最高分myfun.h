#include<queue>
#include<stdio.h>
#include<string.h>
#include<vector>
#include<stdlib.h>
#include<list>
#include<map>
#include<set>
#include<cmath>
#include<time.h>
#include<algorithm>
using namespace std;
#ifndef __MYFUN__
#define __MYFUN__
#define MAXNODE (1000+2)
#define INF (0x7fffffff)
#define MAX_TIME 60
char* write(char *buff,char *p){
  for (int i=0;i<strlen(buff);i++){
     *p++=buff[i];
  }
  *p=0;
  return p;
}
void read_file(char ** buff,const unsigned int spec,char **filedata){
  static int line_num=0;
  buff[0]=filedata[line_num++];
}
int read_int(char **filedata){
  static int line_num=0;
  static char *p=NULL;
  if (p==NULL)p=filedata[line_num++];
  //找到第一个数字
  while (1){
    if (*p==0)p=filedata[line_num++];
    else
    if (*p>='0'&&*p<='9')break;
    else
    p++;
  }

  int ret=0;
  while (*p>='0'&&*p<='9'){
    ret=ret*10+*p-'0';
    p++;
  }
  return ret;
}
struct PATH{                      //费用流
    int flow;
    int cost;                     //整条路径花费
    vector<int> node;
    PATH(){flow=0;node.clear();}
};
struct  SOLUTION{                 //给定服务器为servers，此结构体保存其最小费用最大流
    bool   isFesible;             //是否可行
    int    cost;                  //当前方案的花费
    int    reserved;
    set<int> servers;             //选择的服务器
    vector<PATH>path;             //所有的路径
    SOLUTION(){isFesible=false;cost=INF;}
    void init(){
        isFesible=false;
        cost=INF;
        servers.clear();
        path.clear();
    }
    void output(){return;
       if (!isFesible){printf("No Fesible\n");return ;}
       printf("servers:");
       for (set<int>::iterator it=servers.begin();it!=servers.end();it++)printf("%d ",*it);putchar('\n');
       printf("-------------PATH-------Cost=%d----------\n",path.size(),cost);
       int totCost=0;
       for (int i=0;i<path.size();i++){
         PATH *p=&(path[i]);
         printf("Flow=%02d cost=%03d | ",p->flow,p->cost);
         for (int j=0;j<p->node.size();j++)printf("%d ",p->node[j]);putchar('\n');
         totCost+=p->cost;
         }
        printf("===========TolCost %d==========\n",totCost);
        if (totCost!=cost)system("pause");
    }
};
struct EDGE{
    int from;
    int to;
    int cap;
    int flow;
    int cost;
    EDGE(int from=0,int to=0,int cap=0,int flow=0,int cost=0){
         this->from=from;
         this->to=to;
         this->cap=cap;
         this->flow=flow;
         this->cost=cost;
    }
};
struct GRAPH{
    /*以下记录原始的输入*/
    int nNet;          //nNet个网络节点
    int nEdges;        //nEdges条边
    int nCon;          //nCon个消费节点
    int serCost;       //服务器部署成本
    //边
    vector<int>   u;
    vector<int>   v;
    vector<int>   cap;
    vector<int>   cost;
    //消费节点
    int           netToCon[MAXNODE];
    vector<int>   con_id;   //消费节点对应的网络节点编号
    vector<int>   con_need; //消费节点所需的带宽
    /* 以下对图的一些属性进行扩展 如最短路*/
    //节点费用最短路（kmeans）
    int dist[MAXNODE][MAXNODE];
    int tolFlow;                                //所需的总流量
    int getDist(){
        memset(dist,0xff,sizeof(dist));
        for (int i=0;i<nEdges;i++){
            int x=u[i],y=v[i];
            dist[x][y]=dist[y][x]=cost[i];
        }
        for (int i=0;i<nNet;i++)dist[i][i]=0;
        //Floyd
        for (int k=0;k<nNet;k++)
        for (int i=0;i<nNet;i++)
        for (int j=0;j<nNet;j++)
          if (dist[i][k]!=-1 && dist[j][k]!=-1)
          if (dist[i][j]==-1 || dist[i][j]>dist[i][k]+dist[k][j])
               dist[i][j]=dist[j][i]=dist[i][k]+dist[k][j];
        return 0;
    }
   int ReadGraph(char **filedata){
     //char *buffer;
     //char **buff=&buffer;
     nNet=read_int(filedata);
     nEdges=read_int(filedata);
     nCon=read_int(filedata);
     //read_file(buff,1,filedata); sscanf(buff[0],"%d%d%d",&nNet,&nEdges,&nCon);
     //read_file(buff,1,filedata);
     //read_file(buff,1,filedata);
     //sscanf(buff[0],"%d",&serCost);
     serCost=read_int(filedata);
     //read_file(buff,1,filedata);
     //读入边
     u.clear();
     v.clear();
     cap.clear();
     cost.clear();
     for (int i=0;i<nEdges;i++){
         int u2,v2,cap2,cost2;
         //read_file(buff,1,filedata);sscanf(buff[0],"%d%d%d%d",&u2,&v2,&cap2,&cost2);
         u2=read_int(filedata);
         v2=read_int(filedata);
         cap2=read_int(filedata);
         cost2=read_int(filedata);

         u.push_back(u2);
         v.push_back(v2);
         cap.push_back(cap2);
         cost.push_back(cost2);
     }
     //read_file(buff,1,filedata);
     //读入消费节点
     con_id.clear();
     con_need.clear();
     for (int i=0;i<nCon;i++){
         int cid,nid,need;
         //read_file(buff,1,filedata);sscanf(buff[0],"%d%d%d",&cid,&nid,&need);
         cid=read_int(filedata);
         nid=read_int(filedata);
         need=read_int(filedata);
         con_id.push_back(nid);
         con_need.push_back(need);
     }
     //计算其他信息
     getDist();
     tolFlow=0;
     for (int i=0;i<nCon;i++){
       tolFlow+=con_need[i];
     }
     for (int i=0;i<nCon;i++) netToCon[con_id[i]]=i;
   }
};
//以下为最小费用流
vector<EDGE>       edges;                                //残量图
vector<int>           G[MAXNODE];
int n,m,s,t;                                                         //n节点，m边，s,t
int inq[MAXNODE];
int d[MAXNODE];
int p[MAXNODE];
int a[MAXNODE];
void addEdge(int u,int v,int cap,int cost){
         edges.push_back(EDGE(u,v,cap,0,cost));
         edges.push_back(EDGE(v,u,0,0,-cost));
         G[u].push_back(edges.size()-2);
         G[v].push_back(edges.size()-1);
         //printf("Edge : %d -> %d  cap:%d  cost:%d\n",u,v,cap,cost);
}
PATH* BellmanFord(int s,int t,int&flow,int &cost){
  for (int i=0;i<n;i++)d[i]=INF;
  memset(inq,0,sizeof(inq));
  d[s]=0;inq[s]=1;p[s]=0;a[s]=INF;
  queue<int>Q;
  Q.push(s);
  while (!Q.empty()){
       int u=Q.front();Q.pop();
       inq[u]=0;
       for (int i=0;i<G[u].size();i++){
           EDGE& e=edges[G[u][i]];
           if (e.cap>e.flow&& d[e.to]>d[u]+e.cost){
                d[e.to]=d[u]+e.cost;
                p[e.to] = G[u][i];
                a[e.to]=min(a[u],e.cap-e.flow);
                if (!inq[e.to]) {Q.push(e.to);inq[e.to]=1;}
           }
       }
  }
  if (d[t] == INF) return NULL;
  flow+=a[t];
  cost+=d[t]*a[t];
  int u=t;
  int pathNodes=0;
  while (u!=s){
     edges[p[u]].flow+=a[t];
     edges[p[u]^1].flow-=a[t];
     u=edges[p[u]].from;
     pathNodes++;
  }
  pathNodes--;
  //记录路径
  static PATH *path=new PATH();
  path->flow=a[t];
  path->cost=a[t]*d[t];
  path->node.resize(pathNodes);
  u=edges[p[t]].from;          //源点和汇点无需记录
  int pathId=pathNodes-1;      //路径要从服务器往消费节点方向记录
  while (u!=s){
     path->node[pathId--]=u;
     u=edges[p[u]].from;
  }
  return path;
}
SOLUTION * GetMCMF(GRAPH* g,set<int>& servers){
//返回一个SOLUTION指针，但是如果调用者需要用到此指针的数据，要另外复制一份数据。否则指针指向的数据会被重新覆盖。
    //建图
    n=g->nNet+2;
    s=n-2;
    t=n-1;
    edges.clear();
    for (int i=0;i<n;i++)G[i].clear();
    //链路
    for (int i=0;i<g->nEdges;i++){
         addEdge(g->u[i],g->v[i],g->cap[i],g->cost[i]);
         addEdge(g->v[i],g->u[i],g->cap[i],g->cost[i]);
    }
    //边 s-> server cap=INF cost=0
    for (set<int>::iterator it=servers.begin();it!=servers.end();it++){
        addEdge(s,*it,INF,0);
    }
    //边 consume node -> t  cap =con_need  cost=0
    for (int i=0;i<g->nCon;i++)
         addEdge(g->con_id[i],t,g->con_need[i],0);
    static SOLUTION * sol=new SOLUTION;
    sol->init();
    int flow=0,cost=0;
    PATH *path;
    while ((path=BellmanFord(s,t,flow,cost))!=NULL){
       sol->path.push_back(*path);
    }
    if (flow!=g->tolFlow) sol->isFesible=false;
    else{
        //sol->output();//debug
        sol->isFesible=true;
        sol->cost=cost+g->serCost*servers.size();
        sol->servers=servers;
    }
    return sol;
}
namespace accelerator{
  //Move
  vector<int> pathId[MAXNODE];              //每个服务器占用的path路径条数
  void getpathId(vector<PATH>& path){
     for (int i=0;i<MAXNODE;i++)pathId[i].clear();
     for (int i=0;i<path.size();i++){
       pathId[path[i].node[0]].push_back(i);
     }
  }
  int flow[MAXNODE];
  int move(GRAPH *g,int netId,vector<PATH>& TotPath){   //移动servers
      int totflow=0;
      memset(flow,0,sizeof(flow));
      for (int i=0;i<pathId[netId].size();i++){   //枚举当前节点netId连出的所有流
          int pid=pathId[netId][i];
          PATH *path=&TotPath[pid];
          totflow+=path->flow;
          for (int nodeId=1;nodeId < path->node.size();nodeId++){ //枚举某条具体的流。第一个节点为自身
            int curNode=path->node[nodeId];
            flow[curNode]+=path->flow;
          }
      }
      //流量超过一半，且dist最远
      int mxNetId=-1;
      int mxDist=-1;
      for (int i=0;i<g->nNet;i++){
          if (flow[i]*2>totflow)
            if (g->dist[netId][i]>mxDist){
            mxDist=g->dist[netId][i];
            mxNetId=i;
          }
      }
      return mxNetId;
  }
  set<int> nxtServ;                                  //可以移动到这个服务器集合
  set<int>* moveServers(GRAPH *g,SOLUTION *sol){    //servers，不可移动返回NULL
      getpathId(sol->path);
      set<int>*ser=&sol->servers;
      int changed=false;
      nxtServ.clear();
      for (set<int>::iterator it=ser->begin();it!=ser->end();it++){
          int fromId=*it;
          int toId;
          if ((toId=move(g,fromId,sol->path))!=-1){
              changed=true;
              nxtServ.insert(toId);
          }else nxtServ.insert(fromId);
      }
      if (changed){
#ifdef DEBUG
        printf("Yes\n");
#endif
        return &nxtServ;
      }
      return NULL;
  }
  //添加
  /*vector<int> addList;
  vector<int> getAddServers(GRAPH *g,SOLUTION *sol){    //
      map<int,int> flow;
      flow.clear();
      for (int pid=0;pid<sol->path.size();pid++){
         PATH *path=sol->path[pid];
         for (int i=1;i<path->size();i++){
           if (flow.find(path->node)==flow.end()) flow[path->node]=0;
           flow[path->node]+=path->flow;
         }
      }
      //for (map<int,int>::iterator)
  }*/
}
class TABU{
private:
    GRAPH*         graph;
    SOLUTION*      best;  //  当前搜索到的最优解
    SOLUTION*      curSol;//  servers 当前搜索到的Sol
    int            updateSol();
    /*tabuList*/
    int            tabuLen;
    list<int>      tabuList;
    bool           inTabuList(int server);
    int            addTabuServer(int server);
    /* Frequence */
    int            Frequency[MAXNODE];
public:
    TABU(){}
    int            init(GRAPH *g,set<int>* initSer);
    SOLUTION* Work(GRAPH *g,int epoch,int eta,int tabuListLen,set<int>*initSer);
};
int  TABU::updateSol(){
    if (best->isFesible == false ||(best->cost > curSol->cost && curSol->isFesible)){
       *best=*curSol;
    }
    return 0;
}
bool TABU::inTabuList(int server){
    list<int>::iterator it;
    for (it=tabuList.begin();it!=tabuList.end();it++){
        if (*it  == server)
           return true;
    }
    return false;
}
int TABU::addTabuServer(int server){
    tabuList.push_back(server);
    while (tabuList.size()>tabuLen)
          tabuList.pop_front();
     return 0;
}
int TABU::init(GRAPH* g,set<int>* initSer){
    graph=g;
    /* 随机化初始servers */
    set<int>servers;
    servers.clear();
    if (initSer==NULL){
      for (int i=0;i<g->nCon;i++)servers.insert(g->con_id[i]);
    }
    else
      servers=*initSer;
    //初始化费用流
    curSol=new SOLUTION();
    *curSol=*GetMCMF(graph,servers);                        //费用流
    if (curSol->isFesible==false)exit(-2);
    //printf("[init]curCost=%d\n",curSol->cost);
    best=new SOLUTION();
    updateSol();
    /*tabuList*/
    tabuList.clear();
    /* Frequence */
    memset(Frequency,0,sizeof(Frequency));
    return 0;
}
#define  Rand(p)  (RAND_MAX*(p)>rand())             //以p概率返回true
SOLUTION* TABU::Work(GRAPH *g,int epoch,int eta=0,int tabuListLen=0,set<int>*initSer=NULL){         //连续eta个epoch都没有更好的解，则退出
    time_t time_start=time(NULL);
    init(g,initSer);
    //return best;
    if (tabuListLen==0)
      tabuLen=(int)sqrt(g->nNet);
    else
      tabuLen=tabuListLen;
    if (eta==0)eta=30;
    int Epoch=epoch;
    //
    srand(time(NULL));
    SOLUTION *sol2;
    int noBetter=0;
    while (epoch--&&noBetter++<eta){
         int choose=-1;
         int cost=INF; 
         int nNei=15;
         int nAdd=15;
         vector<PATH> bestPath;
         bestPath.clear();
         set<int> servers=curSol->servers;  //注：
for (int j=0;j<30;j++){
  int i=rand()%g->nNet;
  if (servers.find(i)!=servers.end()){
    servers.erase(i); 
  }else servers.insert(i);
  sol2=GetMCMF(graph,servers);
  if (sol2->isFesible&&sol2->cost <cost){
    choose=i;
    cost=sol2->cost;
    bestPath=sol2->path;
  }
  if (servers.find(i)!=servers.end()) servers.erase(i);else servers.insert(i);
}
         /*for (int i=graph->nNet-1;i>=0;i--){
                if (inTabuList(i))continue;
                //if current Node in servers set then  DELETE it
                if (servers.find(i)!=servers.end()){
                    if (Rand(1.0*nNei/servers.size())){ //随机化，当前节点随机是否要继续计算
                        servers.erase(i);
                        sol2=GetMCMF(graph,servers);
                        if (sol2->isFesible && sol2->cost <cost){    //!!!!!!!!!!路径此处丢失!!!!!!!!!!!!
                            choose=i;
                            cost=sol2->cost;
                            bestPath=sol2->path;
                        }
                        servers.insert(i);
                    }
                }else{
                    //else insert it
                    if (Rand(1.0*nAdd/servers.size())){
                        servers.insert(i);
                        sol2=GetMCMF(graph,servers);
                        if (sol2->isFesible && sol2->cost < cost){
                             choose=i;
                             cost=sol2->cost;
                             bestPath=sol2->path;
                        }
                        servers.erase(i);
                    }
                }
         }*/
         if (choose==-1){continue;}
         curSol->path=bestPath;
         addTabuServer(choose);
         if (curSol->servers.find(choose)!=curSol->servers.end()){
            curSol->servers.erase(choose);
         }
         else{
            curSol->servers.insert(choose);
         }
        curSol->cost=cost;
         //急速
         /*set<int> *nxt;
         if ((nxt=accelerator::moveServers(g,curSol))!=NULL){
           *curSol=*GetMCMF(g,*nxt);
         }*/
        if (best->cost > curSol->cost){
             updateSol();
             noBetter=0;
          }
        time_t time_end=time(NULL);
        if (time_end-time_start>60)break;
        #ifdef DEBUG
        printf("Epoch :%02d  | cost=%d sz=%d\n",Epoch-epoch,curSol->cost,curSol->servers.size());
        #endif
    }
    best->reserved=Epoch-epoch;
    return best;
}
void initServers(GRAPH *g,set<int>&servers){
   servers.clear();
   for (int i=0;i<g->nCon;i++)servers.insert(g->con_id[i]);
   return ;
   //
   int l=1;
   int r=g->nCon;
   int a[MAXNODE];
   for (int i=0;i<g->nCon;i++){
      a[i]=i;
      int r=i==0?0:rand()%i;
      swap(a[i],a[r]);
   }
   SOLUTION *sol;
   while (l<r){  //找一个尽量小的集合满足MCMF
     int mid=(l+r)>>1;
     servers.clear();
     for (int i=0;i<mid;i++)servers.insert(g->con_id[a[i]]);
     sol=GetMCMF(g,servers);
     if (!sol->isFesible) l=mid+1;else r=mid;
   }
   int cost=INF;
   int best=l;
   int step=(int)sqrt(g->nCon);
   for (int num=l;num<=g->nCon;num+=step){
       servers.clear();
       for (int i=0;i<num;i++)servers.insert(g->con_id[a[i]]);
       sol=GetMCMF(g,servers);
       if (sol->isFesible && cost > sol->cost){ best=num;cost=sol->cost;}
   }
   servers.clear();
   for (int i=0;i<best;i++){
       servers.insert(g->con_id[a[i]]);
   }
}
#endif
