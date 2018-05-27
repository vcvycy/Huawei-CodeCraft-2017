/* by chenjianfeng in xiamen university 201703*/
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
int g_time,g_time2;
char* write(char *buff,char *p){
  /*for (int i=0;i<strlen(buff);i++){
     *p++=buff[i];
  }*/
  while (*buff!=0){
    *p=*buff;
    p++;
    buff++;
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
    vector<int> eid;              //边在edges中的id  包含源点和汇点。故size比node大1。 edges[eid[i]].to=node[i] and edges[eid[sz-1].to]=汇点
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
    vector<int>   mynodes;//0..nNet-1这些节点中，属于nodes的节点
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
    void clear(){
      mynodes.clear();
      u.clear();
      v.clear();
      cap.clear();
      cost.clear();
      con_id.clear();
      con_need.clear();
    }
    int getDist(){
        memset(dist,0xff,sizeof(dist));
        for (int i=0;i<nEdges;i++){
            int x=u[i],y=v[i];
            dist[x][y]=dist[y][x]=cost[i];
        }
        for (int i=0;i<nNet;i++)dist[i][i]=0;
        //Floyd
        //int ts=time(0);
        int sz=mynodes.size();
        //new
        int arr[MAXNODE];
        for (int i=0;i<sz;i++)arr[i]=mynodes[i];
        int *pe=arr+sz;
        for (int *k=arr;k!=pe;k++)
        for (int *i=arr;i!=pe;i++)
        for (int *j=i+1;j!=pe;j++){
          if (dist[*i][*k]!=-1 && dist[*j][*k]!=-1)
          if (dist[*i][*j]==-1 || dist[*i][*j]>dist[*i][*k]+dist[*k][*j])
               dist[*i][*j]=dist[*j][*i]=dist[*i][*k]+dist[*k][*j];
        }
        return 0;
    }
   int ReadGraph(char **filedata){
     nNet=read_int(filedata);
     nEdges=read_int(filedata);
     nCon=read_int(filedata);
     serCost=read_int(filedata);
     //读入边
     u.clear();
     v.clear();
     cap.clear();
     cost.clear();
     for (int i=0;i<nEdges;i++){
         int u2,v2,cap2,cost2;
          u2=read_int(filedata);
         v2=read_int(filedata);
         cap2=read_int(filedata);
         cost2=read_int(filedata);

         u.push_back(u2);
         v.push_back(v2);
         cap.push_back(cap2);
         cost.push_back(cost2);
     }
     //读入消费节点
     con_id.clear();
     con_need.clear();
     for (int i=0;i<nCon;i++){
         int cid,nid,need;
         cid=read_int(filedata);
         nid=read_int(filedata);
         need=read_int(filedata);
         con_id.push_back(nid);
         con_need.push_back(need);
     }
     //计算其他信息
     tolFlow=0;
     for (int i=0;i<nCon;i++){
       tolFlow+=con_need[i];
     }
     memset(netToCon,0xff,sizeof(netToCon));
     for (int i=0;i<nCon;i++) netToCon[con_id[i]]=i;
     mynodes.clear();
     for (int i=0;i<nNet;i++) mynodes.push_back(i);
     getDist();
   }
};
//每次执行玩solver分块后，子图的graph和solution都记录在以下中
SOLUTION g_subsol[30];
GRAPH g_subg[30];
int g_subsol_sz;
/*************************************************最小费用流*************************************/
//以下为最小费用流
vector<EDGE>       edges;                                //残量图
vector<int>           G[MAXNODE];
int n,m,s,t;                                                         //n节点，m边，s,t
int inq[MAXNODE];
int d[MAXNODE];
int p[MAXNODE];
int a[MAXNODE];
bool isPathValid(PATH& p){
  for (int i=0;i<p.eid.size()-1;i++){
    EDGE e=edges[p.eid[i]];
    if (e.to!=p.node[i]){
      printf("path not valid!\n");
      exit(-5);
    }
  }
}
void addEdge(int u,int v,int cap,int cost){
         edges.push_back(EDGE(u,v,cap,0,cost));
         G[u].push_back(edges.size()-1);
         #ifdef OPPOSITE_EDGE
         edges.push_back(EDGE(v,u,0,0,-cost));
         G[v].push_back(edges.size()-1);
         #endif
         //printf("Edge : %d -> %d  cap:%d  cost:%d\n",u,v,cap,cost);
}
PATH path_for_getmcmf;
PATH* BellmanFord(int s,int t){
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
  int u=t;
  int pathNodes=0;
  while (u!=s){
     edges[p[u]].flow+=a[t];
     /*#ifdef OPPOSITE_EDGE
     edges[p[u]^1].flow-=a[t];
     #endif*/
     u=edges[p[u]].from;
     pathNodes++;
  }
  pathNodes--;
  //记录路径
  PATH *path=&path_for_getmcmf;
  path->flow=a[t];
  path->cost=a[t]*d[t];
  path->node.resize(pathNodes);
  path->eid.resize(pathNodes+1);
  u=edges[p[t]].from;          //源点和汇点无需记录
  path->eid[pathNodes]=p[t];
  int pathId=pathNodes-1;      //路径要从服务器往消费节点方向记录
  while (u!=s){
     path->node[pathId]=u;
     path->eid[pathId--]=p[u];
     u=edges[p[u]].from;
  }
  #ifdef DEBUG
  isPathValid(*path);
  #endif
  return path;
}
SOLUTION sol_for_getmcmf;
SOLUTION * GetMCMF(GRAPH* g,set<int>& ser){
//返回一个SOLUTION指针，但是如果调用者需要用到此指针的数据，要另外复制一份数据。否则指针指向的数据会被重新覆盖。
    //建图
    set<int> servers=ser;
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

    //边 consume node -> t  cap =con_need  cost=0
    for (int i=0;i<g->nCon;i++)
         addEdge(g->con_id[i],t,g->con_need[i],0);
    //边 s-> server cap=INF cost=0
    /*for (set<int>::iterator it=servers.begin();it!=servers.end();it++){
        addEdge(s,*it,INF,0);
    }*/
    for (int i=0;i<g->nNet;i++)
      if (servers.find(i)==servers.end())
        addEdge(s,i,0,0);
      else
        addEdge(s,i,INF,0);

    SOLUTION * sol=&sol_for_getmcmf;
    sol->init();
    int flow=0,cost=0;
    PATH *path;
    while ((path=BellmanFord(s,t))!=NULL){
       sol->path.push_back(*path);
       flow+=path->flow;
       cost+=path->cost;
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
void insertPath2(PATH& p){  //路径添加到edges残量图中
  for (int i=0;i<p.eid.size();i++){
    int id=p.eid[i];
    edges[id].flow+=p.flow;
  }
}
SOLUTION * GetMCMF3(GRAPH* g,set<int>& ser,bool mapreuse,SOLUTION* sol1=NULL,SOLUTION* sol2=NULL){
//返回一个SOLUTION指针，但是如果调用者需要用到此指针的数据，要另外复制一份数据。否则指针指向的数据会被重新覆盖。
    //建图
    set<int> servers=ser;
if (!mapreuse){
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

    //边 consume node -> t  cap =con_need  cost=0
    for (int i=0;i<g->nCon;i++)
         addEdge(g->con_id[i],t,g->con_need[i],0);
    //边 s-> server cap=INF cost=0
    /*for (set<int>::iterator it=servers.begin();it!=servers.end();it++){
        addEdge(s,*it,INF,0);
    }*/
    for (int i=0;i<g->nNet;i++)
      if (servers.find(i)==servers.end())
        addEdge(s,i,0,0);
        else
        addEdge(s,i,INF,0);
}else{
    for (int i=0;i<edges.size();i++)
    edges[i].flow=0;
    for (int i=edges.size()-1;i>=0;i--)
      if (edges[i].from ==s){
          if (servers.find(edges[i].to)!=servers.end())
          edges[i].cap=INF;
          else
          edges[i].cap=0;
      }else break;
}
    SOLUTION * sol=&sol_for_getmcmf;
    sol->init();
    int flow=0,cost=0;
    //利用旧的路径加速
if (sol1!=NULL){
    if (sol1->servers.size()<sol2->servers.size()) swap(sol1,sol2);
    int inS2[MAXNODE],sz2=0;
    for (set<int>::iterator it=sol2->servers.begin();it!=sol2->servers.end();it++){
      if (sol1->servers.find(*it)==sol1->servers.end())
      inS2[sz2++]=*it;
    }
    #ifdef DEBUG
    //int edgeCnt=0,edgeCnt2=0;
    #endif
    for (int i=0;i<sol1->path.size();i++){
       PATH* p=&sol1->path[i];
       int targetNode=p->node[p->node.size()-1];
       int flowDist=p->cost/p->flow;
       int min_dist=INF;
       for(int j=0;j<sz2;j++) min_dist=min(min_dist,g->dist[targetNode][inS2[j]]);
       if (min_dist >=flowDist){
            insertPath2(*p);
            sol->path.push_back(*p);
            flow+=p->flow;
            cost+=p->cost;
            #ifdef DEBUG
            //edgeCnt++;
            #endif
       }
    }
}


    PATH *path;
    while ((path=BellmanFord(s,t))!=NULL){
       sol->path.push_back(*path);
       flow+=path->flow;
       cost+=path->cost;
       #ifdef DEBUG
       //edgeCnt2++;
       #endif
    }
    #ifdef DEBUG
    //printf("edgeCnt=%d  %d\n",edgeCnt,edgeCnt2);
    #endif
    if (flow!=g->tolFlow) sol->isFesible=false;
    else{
        //sol->output();//debug
        sol->isFesible=true;
        sol->cost=cost+g->serCost*servers.size();
        sol->servers=servers;
    }
    return sol;
}

SOLUTION * GetMCMF2(GRAPH* g,set<int>& ser,bool mapreuse,SOLUTION *curSol){
//返回一个SOLUTION指针，但是如果调用者需要用到此指针的数据，要另外复制一份数据。否则指针指向的数据会被重新覆盖。
    //建图
    set<int> servers=ser;
    //地图没变，不用重新建
if (mapreuse==false){
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

    //边 consume node -> t  cap =con_need  cost=0
    for (int i=0;i<g->nCon;i++)
         addEdge(g->con_id[i],t,g->con_need[i],0);
     //边 s-> server cap=INF cost=0
    /*for (set<int>::iterator it=servers.begin();it!=servers.end();it++){
        addEdge(s,*it,INF,0);
    }*/
    for (int i=0;i<g->nNet;i++)
      if (servers.find(i)==servers.end())
        addEdge(s,i,0,0);
        else
        addEdge(s,i,INF,0);
}else {
    for (int i=0;i<edges.size();i++)
    edges[i].flow=0;
    for (int i=edges.size()-1;i>=0;i--)
      if (edges[i].from ==s){
          if (servers.find(edges[i].to)!=servers.end())
          edges[i].cap=INF;
          else
          edges[i].cap=0;
      }else break;
}

    SOLUTION * sol=&sol_for_getmcmf;
    sol->init();
    int flow=0,cost=0;
    //把旧的路径加入图中
    if (curSol->servers.size() < servers.size()){ //说明是加点
        int addNode;
           for (set<int>::iterator it=servers.begin();it!=servers.end();it++)
          if (curSol->servers.count(*it)==0){
             addNode=*it;
             break;
          }
        for (int i=0;i<curSol->path.size();i++){
          PATH *p=&curSol->path[i];
          int targetNode=p->node[p->node.size()-1];  //目标节点
          int flowDist=p->cost/p->flow;
          if (g->dist[addNode][targetNode] >=flowDist){ //路径加入当前解
              insertPath2(*p);
              sol->path.push_back(*p);
              flow+=p->flow;
              cost+=p->cost;
          }
        }
    }
    else{                                          //说明是减少节点
        int delNode;
        for (set<int>::iterator it=curSol->servers.begin();it!=curSol->servers.end();it++)
          if (servers.count(*it)==0){
             delNode=*it;
             break;
          }
        for (int i=0;i<curSol->path.size();i++){
          PATH*p=&curSol->path[i];
          int startNode=p->node[0];
          if (startNode!=delNode){
              insertPath2(*p);
              sol->path.push_back(*p);
              flow+=p->flow;
              cost+=p->cost;
          }
        }
    }
    PATH *path;
    while ((path=BellmanFord(s,t))!=NULL){
       sol->path.push_back(*path);
       flow+=path->flow;
       cost+=path->cost;
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
/***********************优化服务器*******************************/
/***************以下操作均在edges残量图中操作******************/
  vector<PATH> serPath[MAXNODE];

void removePath(PATH& p){  //路径从edges残量图中删除
  for (int i=0;i<p.eid.size();i++){
    int id=p.eid[i];
    edges[id].flow-=p.flow;
    #ifdef OPPOSITE_EDGE
    edges[id^1].flow+=p.flow;
    #endif
  }
}
void insertPath(PATH& p){  //路径添加到edges残量图中
  for (int i=0;i<p.eid.size();i++){
    int id=p.eid[i];
    edges[id].flow+=p.flow;
    #ifdef DEBUG
    if (edges[id].cap<edges[id].flow) {
      printf("路径插入导致cap溢出！eid=%d  from %d to %d cap=%d flow=%d\n",id,edges[i].from,edges[i].to,edges[id].cap,edges[id].flow);
      exit(-4);
    }
    #endif
    #ifdef OPPOSITE_EDGE
    edges[id^1].flow-=p.flow;
    #endif
  }
}
//s->sid这条边cap设置为cap

void set_s_sid_cap(int sid,int cap){
  for (int i=edges.size()-1;i>=0;i--){
    if (edges[i].from==s && edges[i].to==sid){  //s->sid这条边的flow应该为0.其他边由于有其他服务器存在，不一定为0.
        edges[i].cap=cap;
        return;
    }
  }
}
//removeSer只操作残量图
void removeSer(int sid,vector<PATH>&p,int &ser_flow,int& ser_cost){     //edges残量图中删除服务器，并返回此服务器未删除的流量
  // 源点->sid 这条边cap设为0
  ser_flow=0;
  ser_cost=0;
  for (int i=0;i<p.size();i++){
      ser_flow+=p[i].flow;
      ser_cost+=p[i].cost;
      removePath(p[i]);
  }
  set_s_sid_cap(sid,0);
}

void recoverSer(int sid,vector<PATH>&newPath){
  for (int i=0;i<newPath.size();i++)      removePath(newPath[i]);     //删除新加入的路径
  // 源点->sid的这条边，cap设置为INF
  /*for (int i=edges.size()-1;i>=0;i--){
    if (edges[i].from==s && edges[i].to==sid){  //s->sid这条边的flow应该为0.其他边由于有其他服务器存在，不一定为0.
        #ifdef DEBUG
        if (edges[i].flow!=0){// || edges[i^1].flow!=0){
           printf("removePath Unsuccess!\n");
           exit(-3);
        }
        #endif
        edges[i].cap=INF;
        break;
    }
  }*/
  set_s_sid_cap(sid,INF);
  //插入旧路径
  for (int i=0;i<serPath[sid].size();i++) insertPath(serPath[sid][i]);
}


set<int> tabu_ser;                                                      //被删过，无法再次处理的节点
int arrSer[MAXNODE];
int chooseAser(set<int>&curSer){              //选择一个服务器并返回(尽量选择服务器可替代性强的=路径少？)
  //服务器位置随机，方便随机选择服务器
  int sz=0;
  for (set<int>::iterator it=curSer.begin();it!=curSer.end();it++){
     if (tabu_ser.find(*it)==tabu_ser.end())
     arrSer[sz++]=*it;
  }
  if (sz==0)return -1;
  int k=max(2,sz/20);                                                   //从5%的节点中选择
  int sid,pathNum=20000;
  while (k--){
    int s=arrSer[rand()%sz];
    if (serPath[s].size()<pathNum){
      sid=s;
      pathNum=serPath[s].size();
    }
  }
  tabu_ser.insert(sid);
  return sid;
}
void buildResidentGraph(GRAPH* g,set<int>&servers){
     for (int i=0;i<edges.size();i++)
    edges[i].flow=0;
    for (int i=edges.size()-1;i>=0;i--)
      if (edges[i].from ==s){
          if (servers.find(edges[i].to)!=servers.end())
          edges[i].cap=INF;
          else
          edges[i].cap=0;
      }else break;
    /*n=g->nNet+2;
    s=n-2;
    t=n-1;
    edges.clear();
    for (int i=0;i<n;i++)G[i].clear();
    //链路
    for (int i=0;i<g->nEdges;i++){
         addEdge(g->u[i],g->v[i],g->cap[i],g->cost[i]);
         addEdge(g->v[i],g->u[i],g->cap[i],g->cost[i]);
         //printf("build addedge:%d %d %d %d\n",g->u[i],g->v[i],g->cap[i],g->cost[i]);
    }
    //边 consume node -> t  cap =con_need  cost=0
    for (int i=0;i<g->nCon;i++)
         addEdge(g->con_id[i],t,g->con_need[i],0);
    //边 s-> server cap=INF cost=0
    //s->普通节点连(0,0)的边。方便变成服务器后，修改下cap即可
    for (int i=0;i<g->nNet;i++)
      if (servers.find(i)==servers.end())
        addEdge(s,i,0,0);
      else
        addEdge(s,i,INF,0);
     */
    //将serPath加入图中
   for (int i=0;i<g->nNet;i++)
    for (int j=0;j<serPath[i].size();j++){
       insertPath(serPath[i][j]);
    }
}
//减少服务器节点。原则：修改残量图，修改serPath，sol中除了path不更新，其他(cost,servers)都要更新
void reduceServers(GRAPH* g,SOLUTION *sol,int diff,int useTime){
  #ifdef DEBUG
  //printf("Diff=%d sz=%d before reduce :%d\n",diff,sol->servers.size(),sol->cost);
  #endif
  int ts=time(0);
  vector<PATH> newPath;
  int costBeforeRemove=sol->cost;                                                 //当前总费用
  int flag=sol->servers.size();
  tabu_ser.clear();
  while (1){
    //选择一个节点
    int sid=chooseAser(sol->servers);
    if (sid==-1){break;}
    int ser_flow,ser_cost;                                                     //此服务器产生的流量和费用
    removeSer(sid,serPath[sid],ser_flow,ser_cost);                             //服务器
    //printf("servers [%d] need flow %d cost %d  ",sid,ser_flow,ser_cost);
    int costAfterRemove=costBeforeRemove-ser_cost-g->serCost;                  //当前的cost
    PATH *path;

    newPath.clear();
    //在残量图中继续计算
    while ((path=BellmanFord(s,t))!=NULL){
       costAfterRemove+=path->cost;
       ser_flow-=path->flow;
       newPath.push_back(*path);
       //printf("newPath:");for (int i=0;i<path->node.size();i++)printf("%d ",path->node[i]);
       //printf(" flow %d\n",path->flow);
    }
    //printf("flow_need=%d before cost=%d after cost=%d\n",ser_flow,costBeforeRemove,costAfterRemove);
    if (ser_flow!=0 || costAfterRemove>costBeforeRemove-diff){
        recoverSer(sid,newPath);
        //printf("can't delete %d\n",sid);
    } else{
        //添加新路径到serPath
        for (int i=0;i<newPath.size();i++) serPath[newPath[i].node[0]].push_back(newPath[i]);
        sol->servers.erase(sid);
        costBeforeRemove=costAfterRemove;
        serPath[sid].clear();
       // printf("delete %d success\n",sid);
    }
    if (diff>0 && --flag<=sol->servers.size()*0.05)break;
    if (time(NULL)-ts>=useTime)break;
    //break;
  }
  sol->cost=costBeforeRemove;
  #ifdef DEBUG
  //printf("Diff=%d sz=%d after reduce :%d\n",diff,sol->servers.size(),sol->cost);
  #endif
}
bool moveServer(GRAPH* g,SOLUTION* sol,int useTime){
  #ifdef NOTMOVE
    return false;
  #endif
  #ifdef DEBUG
    printf("before move cost:%d\n",sol->cost);
  #endif
  int ts=time(0);
  vector<PATH> newPath;                                                           //旧节点删除、新节点添加后，调用bellmanford产生的新路径
  int costBeforeRemove=sol->cost;                                                 //当前总费用
  //保存当前服务器进入数组中，方便随机选择
  int sz=0;
  for (set<int>::iterator it=sol->servers.begin();it!=sol->servers.end();it++) arrSer[sz++]=*it;
  int flag=0;
  while (1){
    //选择一个节点，和此节点相邻的非服务器节点
    int oldNodeId=rand()%sz;
    int oldNode=arrSer[oldNodeId];
    int pathId=rand()%serPath[oldNode].size();
    if (serPath[oldNode][pathId].node.size()<2)continue;
    int newNode=serPath[oldNode][pathId].node[1];//edges[G[oldNode][rand()%G[oldNode].size()]].to;
    int ser_flow,ser_cost;                                                     //此服务器产生的流量和费用
    //删除旧节点oldNode
    removeSer(oldNode,serPath[oldNode],ser_flow,ser_cost);                             //服务器
    //添加新节点
    set_s_sid_cap(newNode,INF);
    int costAfterRemove=costBeforeRemove-ser_cost;                  //当前的cost
    PATH *path;
    newPath.clear();
    //在残量图中继续计算
    int newSz=0;
    while ((path=BellmanFord(s,t))!=NULL){
       costAfterRemove+=path->cost;
       ser_flow-=path->flow;
       newPath.push_back(*path);
       if (path->node[0]==newNode)newSz++;
    }
    if (ser_flow!=0 || costAfterRemove>costBeforeRemove){
        recoverSer(oldNode,newPath);
        set_s_sid_cap(newNode,0);
        flag++;
    } else{

    #ifdef DEBUG
     if (ser_flow==0) printf("MOVE %d->%d[sz:%d]  cost %d->%d\n",oldNode,newNode,newSz,costBeforeRemove,costAfterRemove);
    #endif
        //添加新路径到serPath
        for (int i=0;i<newPath.size();i++) serPath[newPath[i].node[0]].push_back(newPath[i]);
        serPath[oldNode].clear();                                                                //边保存
        sol->servers.erase(oldNode);                                                             //节点保存
        sol->servers.insert(newNode);
        costBeforeRemove=costAfterRemove;
        //arrSER
        arrSer[oldNodeId]=newNode;

     sol->cost=costBeforeRemove;
     return true;
    }
    if (flag> g->nEdges) break;
    if (time(NULL)-ts>=useTime)break;
    //break;
  }
  return false;
  #ifdef DEBUG
    printf("after move cost:%d\n",sol->cost);
  #endif
}
//减少服务器节点。原则：修改残量图，修改serPath，sol中除了path不更新，其他(cost,servers)都要更新
void reduceServers2(GRAPH* g,SOLUTION *sol,int diff,int useTime,set<int>&nochange){
  #ifdef DEBUG
  //printf("Diff=%d sz=%d before reduce :%d\n",diff,sol->servers.size(),sol->cost);
  #endif
  int ts=time(0);
  vector<PATH> newPath;
  int costBeforeRemove=sol->cost;                                                 //当前总费用
  int flag=sol->servers.size();
  tabu_ser.clear();
  set<int> ser;
  for(set<int>::iterator it=sol->servers.begin();it!=sol->servers.end();it++){
    if (nochange.find(*it)==nochange.end())
      ser.insert(*it);
  }
  set<int>::iterator it=ser.begin();
  while (1){
    //选择一个节点
    //int sid=chooseAser(ser);
    //if (sid==-1){break;}
    if (ser.size()==0)break;
    int sid=chooseAser(ser);//*ser.begin();
    ser.erase(sid);
    //if (rand()<100)break;                                                      //

    int ser_flow,ser_cost;                                                     //此服务器产生的流量和费用
    removeSer(sid,serPath[sid],ser_flow,ser_cost);                             //服务器
    //printf("servers [%d] need flow %d cost %d  ",sid,ser_flow,ser_cost);
    int costAfterRemove=costBeforeRemove-ser_cost-g->serCost;                  //当前的cost
    PATH *path;

    newPath.clear();
    //在残量图中继续计算
    while ((path=BellmanFord(s,t))!=NULL){
       costAfterRemove+=path->cost;
       ser_flow-=path->flow;
       newPath.push_back(*path);
       //printf("newPath:");for (int i=0;i<path->node.size();i++)printf("%d ",path->node[i]);
       //printf(" flow %d\n",path->flow);
    }
    //printf("flow_need=%d before cost=%d after cost=%d\n",ser_flow,costBeforeRemove,costAfterRemove);
    if (ser_flow!=0 || costAfterRemove>costBeforeRemove-diff){
        recoverSer(sid,newPath);
        //printf("can't delete %d\n",sid);
    } else{
        //添加新路径到serPath
        for (int i=0;i<newPath.size();i++) serPath[newPath[i].node[0]].push_back(newPath[i]);
        sol->servers.erase(sid);
        costBeforeRemove=costAfterRemove;
        serPath[sid].clear();
       // printf("delete %d success\n",sid);
    }
    if (diff>0 && --flag<=sol->servers.size()*0.05)break;
    if (time(NULL)-ts>=useTime)break;
    //break;
  }
  sol->cost=costBeforeRemove;
  #ifdef DEBUG
  //printf("Diff=%d sz=%d after reduce :%d\n",diff,sol->servers.size(),sol->cost);
  #endif
}
void updateServers(GRAPH* g,SOLUTION* sol,int useTime,int diff){                  //缩减服务器数量，servers必须保证可行
  //每一个服务器包含的路径，用于减去这部分路径
  #ifdef DEBUG
  //printf("-----before shrink SerSz=%d Cost=%d--------\n",sol->servers.size(),sol->cost);
  #endif
  int ts=time(NULL);
  for (int i=0;i<MAXNODE;i++)serPath[i].clear();                         //每个服务器的路径集合
  for (int i=0;i<sol->path.size();i++){
      serPath[sol->path[i].node[0]].push_back(sol->path[i]);
  }

  //建图
  buildResidentGraph(g,sol->servers);
  while (diff){
    if (diff<20)diff=0;else diff/=4;
    reduceServers(g,sol,diff,useTime-(time(0)-ts));
    //moveServer(g,sol,3);
    //printf("diff=%d\n",diff);
  }
  //保存sol
  sol->path.clear();
  for (int i=0;i<g->nNet;i++)
    for (int j=0;j<serPath[i].size();j++)
    sol->path.push_back(serPath[i][j]);
  sol->isFesible=true;
  #ifdef DEBUG
  //printf("-----after shrink SerSz=%d Cost=%d--------\n",sol->servers.size(),sol->cost);
  #endif
}
//updateServers3 使用g_subsol进行加速
void updateServers3(GRAPH* g,SOLUTION* sol,int useTime,int diff){                  //缩减服务器数量，servers必须保证可行
  //每一个服务器包含的路径，用于减去这部分路径
  #ifdef DEBUG
  //printf("-----before shrink SerSz=%d Cost=%d--------\n",sol->servers.size(),sol->cost);
  #endif
  int ts=time(NULL);
  for (int i=0;i<MAXNODE;i++)serPath[i].clear();                         //每个服务器的路径集合
  for (int i=0;i<sol->path.size();i++){
      serPath[sol->path[i].node[0]].push_back(sol->path[i]);
  }
  //不做改变的节点(每一个子图中70%节点不变。即距离最远的70%
  /*set<int> noChange;noChange.clear();
  set<int> inOtherG[MAXNODE];inOtherG=sol->servers;
  set<int>::iterator it;
  for (int gid=0;gid<g_subsol_sz;gid++){
      for (it=g_subg[gid].servers.begin();it!=g_subg[gid].servers.end();it++) inOtherG.erase(*it);
      int nochange_sz=g_subg.mynodes.size()*7/10;  //70%
      while (nochange_sz--){
          int mx_dist=-1,mx_node; //距离其他服务器
          for (it=g_subg[gid]

      }
      for (it=g_subg[gid].servers.begin();it!=g_subg[gid].servers.end();it++) inOtherG.insert(*it);
  }*/
  //建图
  buildResidentGraph(g,sol->servers);
  while (diff){
    if (diff<20)diff=0;else diff/=4;
    reduceServers(g,sol,diff,useTime-(time(0)-ts));
    //moveServer(g,sol,3);
    //printf("diff=%d\n",diff);
  }
  //保存sol
  sol->path.clear();
  for (int i=0;i<g->nNet;i++)
    for (int j=0;j<serPath[i].size();j++)
    sol->path.push_back(serPath[i][j]);
  sol->isFesible=true;
  #ifdef DEBUG
  //printf("-----after shrink SerSz=%d Cost=%d--------\n",sol->servers.size(),sol->cost);
  #endif
}
void updateServers2(GRAPH* g,SOLUTION* sol,int useTime,int diff,set<int>& noChange){                  //缩减服务器数量，servers必须保证可行
  //updateServers(g,sol,useTime,diff);return ;
  //每一个服务器包含的路径，用于减去这部分路径
  //减少一部分 noChange servers。20%的大小。距离需要改变的距离越近，越要转为needChange
  set<int> needChange;needChange.clear();
  for (set<int>::iterator it=sol->servers.begin();it!=sol->servers.end();it++)
    if (noChange.count(*it)==0)
      needChange.insert(*it);
  int del_sz=noChange.size()/10;
  while (del_sz--){
      int min_dist=INF,min_node=-1;
      for (set<int>::iterator it=noChange.begin();it!=noChange.end();it++){
         int dist=INF;//节点*it距离needChange中最短的距离
         for(set<int>::iterator it2=needChange.begin();it2!=needChange.end();it2++)
           dist=min(dist,g->dist[*it][*it2]);
         if (min_dist>dist){
            min_dist=dist;
            min_node=*it;
         }
      }
      if (min_node!=-1)noChange.erase(min_node);
  }
  /********/
  int ts=time(NULL);
  for (int i=0;i<MAXNODE;i++)serPath[i].clear();                         //每个服务器的路径集合
  for (int i=0;i<sol->path.size();i++){
      serPath[sol->path[i].node[0]].push_back(sol->path[i]);
  }

  //建图
  buildResidentGraph(g,sol->servers);
  while (diff){
    if (diff<20)diff=0;else diff/=4;
    reduceServers2(g,sol,diff,useTime-(time(0)-ts),noChange);
    //reduceServers(g,sol,diff,useTime-(time(0)-ts));
    //moveServer(g,sol,3);
    //printf("diff=%d\n",diff);
  }
  //保存sol
  sol->path.clear();
  for (int i=0;i<g->nNet;i++)
    for (int j=0;j<serPath[i].size();j++)
    sol->path.push_back(serPath[i][j]);
  sol->isFesible=true;
  #ifdef DEBUG
  //printf("-----after shrink SerSz=%d Cost=%d--------\n",sol->servers.size(),sol->cost);
  #endif
}
/****************************************************************************/
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
int getRealPath(vector<PATH> &path,set<int>&servers,GRAPH *g);
class TABU{
private:
    SOLUTION*      best;  //  当前搜索到的最优解
    SOLUTION*      curSol;//  servers 当前搜索到的Sol
    int            updateSol();
public:
    TABU(){curSol=new SOLUTION();best=new SOLUTION();}
    int            init(GRAPH *g,set<int>* initSer);
    SOLUTION* Work(GRAPH *g,int epoch,int eta,set<int>*initSer,int time_limit);
    SOLUTION* Work_plus(GRAPH *g,int epoch,int eta,set<int>*initSer,int time_limit);
    SOLUTION* mergeSubGraph(GRAPH *g,GRAPH*subg[2],int epoch,int eta,set<int>*initSer,int time_limit,int c0,int c1);
};
int  TABU::updateSol(){
    if (best->isFesible == false ||(best->cost > curSol->cost && curSol->isFesible)){
       *best=*curSol;
    }
    return 0;
}
int TABU::init(GRAPH* g,set<int>* initSer){
    //graph=g;
    /* 随机化初始servers */
    set<int>servers;
    servers.clear();
    if (initSer==NULL){
      for (int i=0;i<g->nCon;i++)servers.insert(g->con_id[i]);
    }
    else
      servers=*initSer;
    //初始化费用流
    curSol->init();

    *curSol=*GetMCMF(g,servers);                        //费用流

    best->init();
    updateSol();
    return 0;
}
//int getNodes
#define  Rand(p)  (RAND_MAX*(p)>rand())             //以p概率返回true
SOLUTION* TABU::Work(GRAPH *g,int epoch,int eta,set<int>*initSer,int time_limit){         //连续eta个epoch都没有更好的解，则退出
    time_limit=max(time_limit,1);
    time_t time_start=time(NULL);
    init(g,initSer);
    int Epoch=epoch;

    //
    SOLUTION *sol2;
    int noBetter=0;
    while (epoch--&&noBetter++<eta){
         int choose=-1;
         int cost=INF;
         set<int> servers=curSol->servers;
         vector<PATH> bestPath;
         bestPath.clear();
         for (int j=0;j<15;j++){
           int i=rand()%g->mynodes.size();
           i=g->mynodes[i];
           if (servers.find(i)!=servers.end()){
             servers.erase(i);
           }else {
             servers.insert(i);
           }
           sol2=GetMCMF2(g,servers,true,curSol);
           //sol2=GetMCMF(g,servers);
           if (sol2->isFesible&&sol2->cost <cost){
             choose=i;
             cost=sol2->cost;
             bestPath=sol2->path;
           }
           if (servers.find(i)!=servers.end()) servers.erase(i);else servers.insert(i);
         }
         if (choose==-1){continue;}
         if (curSol->servers.find(choose)!=curSol->servers.end()){
            curSol->servers.erase(choose);
         }
         else{
            curSol->servers.insert(choose);
         }
        curSol->cost=cost;
        curSol->path=bestPath;
        if (best->cost > curSol->cost){
             updateSol();
             noBetter=0;
          }
        time_t time_end=time(NULL);
        if (time_end-time_start>time_limit)break;
        #ifdef DEBUG
        //printf("Epoch :%02d  | cost=%d sz=%d\n",Epoch-epoch,curSol->cost,curSol->servers.size());
        #endif
    }
    best->reserved=Epoch-epoch;
    return best;
}
//work 重复利用残量图加速
SOLUTION* TABU::Work_plus(GRAPH *g,int epoch,int eta,set<int>*initSer,int time_limit){         //连续eta个epoch都没有更好的解，则退出

    time_limit=max(time_limit,1);
    time_t time_start=time(NULL);

    init(g,initSer);
    int Epoch=epoch;
    for (int i=0;i<MAXNODE;i++)serPath[i].clear();                         //每个服务器的路径集合
    for (int i=0;i<curSol->path.size();i++){
       serPath[curSol->path[i].node[0]].push_back(curSol->path[i]);
    }
    //init执行后(getmcmf也执行了)，此时残量图还保存在edges中
     vector<PATH> newPath;                                                           //旧节点删除、新节点添加后，调用bellmanford产生的新路径
     while (Epoch--){
        int choose=-1;
        int min_cost=INF;
        for (int i=0;i<20;i++){  //随机选择一个节点
           int nodeId=g->mynodes[rand()%g->mynodes.size()];
           newPath.clear();
           if (curSol->servers.find(nodeId)!=curSol->servers.end()){                 //添加
               set_s_sid_cap(nodeId,INF);
           }else{                                                                    //删除
               int ser_flow,ser_cost;
               removeSer(nodeId,serPath[nodeId],ser_flow,ser_cost);
               int costAfterRemove=curSol->cost-g->serCost-ser_cost;
               PATH* path;
               while ((path=BellmanFord(s,t))!=NULL){
                   costAfterRemove+=path->cost;
                   ser_flow-=path->flow;
                   newPath.push_back(*path);
               }
               if (ser_flow!=0){
                  if (costAfterRemove>min_cost){
                     min_cost=costAfterRemove;
                     choose=nodeId;
                  }
               }
               recoverSer(nodeId,newPath);
           }
        }
     }

    return best;
}
SOLUTION* TABU::mergeSubGraph(GRAPH *g,GRAPH* subg[2],int epoch,int eta,set<int>*initSer,int time_limit,int c0,int c1){         //连续eta个epoch都没有更好的解，则退出
    time_limit=max(time_limit,1);
    time_t time_start=time(NULL);
    init(g,initSer);
    int Epoch=epoch;
    //固定不变的路径
    //printf("GetRealPath!\n");
    //getRealPath(curSol->path,curSol->servers,g);
    //vector<PATH> fixed;
    //两部分节点染色
    bool color[MAXNODE];
    for (int i=0;i<subg[0]->mynodes.size();i++) color[subg[0]->mynodes[i]]=0;
    for (int i=0;i<subg[1]->mynodes.size();i++) color[subg[1]->mynodes[i]]=1;
    /*
    bool activeNode[MAXNODE];//距离c0/c1中心越远，越不活跃
    memset(activeNode,0,sizeof(activeNode));
    for (int i=0;i<g->nEdges;i++){
      int u=g->u[i];
      int v=g->v[i];
      if (color[u]!=color[v]) activeNode[u]=activeNode[v]=1;
    }
    int cnt=0;
    for (int i=0;i<MAXNODE;i++)cnt+=activeNode[i];
    printf("ACTIVE NODES %d %d:%.3lf\n",cnt,g->mynodes.size(),1.0*cnt/g->mynodes.size());
    //
    */
    SOLUTION *sol2;
    int noBetter=0;
    while (epoch--&&noBetter++<eta){
         int choose=-1;
         int cost=INF;

         vector<PATH> bestPath;
         bestPath.clear();
         set<int> servers=curSol->servers;

         for (int j=0;j<30;j++){
           int i=rand()%g->mynodes.size();
           i=g->mynodes[i];
           //if (g->mynodes.size()>300&&!activeNode[i])continue;
           if (servers.find(i)!=servers.end()){
             servers.erase(i);
           }else {
             if (g->mynodes.size()>300)continue;
             servers.insert(i);
           }
           sol2=GetMCMF(g,servers);
           if (sol2->isFesible&&sol2->cost <cost){
             choose=i;
             cost=sol2->cost;
             bestPath=sol2->path;
             if (g->mynodes.size()>600&&sol2->cost<best->cost) j=44;
           }
           if (servers.find(i)!=servers.end()) servers.erase(i);else servers.insert(i);
         }
         if (choose==-1){continue;}
         curSol->path=bestPath;
         if (curSol->servers.find(choose)!=curSol->servers.end()){
            curSol->servers.erase(choose);
         }
         else{
            curSol->servers.insert(choose);
         }
        curSol->cost=cost;
         //急速

         set<int> *nxt;

         if ((nxt=accelerator::moveServers(g,curSol))!=NULL){
           *curSol=*GetMCMF(g,*nxt);
           if (curSol->isFesible==false) exit(-9);
         }
        if (best->cost > curSol->cost){
             updateSol();
             noBetter=0;
          }
        time_t time_end=time(NULL);
        if (time_end-time_start>time_limit)break;
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
}
int kmeans(GRAPH *g,int color[],int AVG_NODE_NUM){  //返回分类个数(dist=-1则不联通)

   int k=max(1,g->nNet/AVG_NODE_NUM);//0~k-1染色
   k=min(25,k);
   //初始中心点
   int cent[30];
   for (int i=0;i<k;i++){
       cent[i]=g->nNet/k*i;
       cent[i]+=rand()%g->nNet/k;
   }
   //k-means算法
   bool hasChange=true;           //中心点不变后结束kmeans
   int cnt=0;
   while (hasChange&&cnt++<10){
     //for (int i=0;i<k;i++)printf("%d ",cent[i]);putchar('\n');system("pause");
     hasChange=false;
     //节点染色
     for (int u=0;u<g->nNet;u++){
      int minDist=INF;
      for (int i=0;i<k;i++)
       if (g->dist[u][cent[i]]!=-1 && minDist>g->dist[u][cent[i]]){
         minDist=g->dist[u][cent[i]];
         color[u]=i;
       }
     }
     vector<int> cluster[30];
     for (int i=0;i<k;i++)cluster[i].clear();
     for (int u=0;u<g->nNet;u++)
      if (color[u]!=-1)
      cluster[color[u]].push_back(u);
     //cluster中心重新计算
     for (int col=0;col<k;col++){
       int newCenter=0;
       int minDistSum=INF;

       for (int c=0;c<cluster[col].size();c++){
         int u=cluster[col][c];                        //u为col颜色新中心点
         int DistSum=0;
         for (int i=0;i<cluster[col].size();i++) {
             int v=cluster[col][i];
             DistSum+=g->dist[v][u];
         }
         if (DistSum<minDistSum){
            minDistSum=DistSum;
            newCenter=u;
         }
       }
       //更新中心点
       if (cent[col]!=newCenter){
         hasChange=true;
         cent[col]=newCenter;
       }
     }
   }
   int flag=0;
   for (int i=0;i<g->nNet;i++){
     if (color[i]==-1){
       flag=1;
       color[i]=k;
     }
   }
   k+=flag;
   return k;
}
void buildSubGraph(GRAPH *g,int color[],int cluster_num,GRAPH* subg[]){

   for (int col=0;col<cluster_num;col++){
       //当前cluster节点存入Nodes
       //建图
       GRAPH *g2=g_subg+col;g2->clear();//new GRAPH();
       subg[col]=g2;

       g2->nNet=g->nNet;
       g2->serCost=g->serCost;
       g2->nEdges=0;
       g2->nCon=0;
       g2->tolFlow=0;
       g2->con_id.clear();
       g2->con_need.clear();
   }
   for (int i=0;i<g->nEdges;i++){  //边数和边
    int u=g->u[i];
    int v=g->v[i];
    int cap=g->cap[i];
    int cost=g->cost[i];
    if (color[u]==color[v]){
      int c=color[u];
      subg[c]->u.push_back(u);
      subg[c]->v.push_back(v);
      subg[c]->cap.push_back(cap);
      subg[c]->cost.push_back(cost);
      subg[c]->nEdges++;
    }
   }
    for (int i=0;i<g->con_id.size();i++){
      int u=g->con_id[i];
      int need=g->con_need[i];
      int c=color[u];
      subg[c]->con_id.push_back(u);
      subg[c]->con_need.push_back(need);
      subg[c]->tolFlow+=need;
      subg[c]->nCon++;
    }
    //mynodes
    for (int i=0;i<g->nNet;i++){
      int c=color[i];
      subg[c]->mynodes.push_back(i);
    }
    for (int i=0;i<cluster_num;i++)
      subg[i]->getDist();
}
SOLUTION *solver(GRAPH *g,int Time){
   //kmeans节点染色
   int color[MAXNODE];
   int t1=time(NULL);
   memset(color,0xff,sizeof(color));
   int cluster_num=kmeans(g,color,30+rand()%50);
   //建子图
   GRAPH *subg[30];
   buildSubGraph(g,color,cluster_num,subg);
   #ifdef DEBUG
   /*for (int i=0;i<cluster_num;i++){
     printf("cluster[%d]: edges=%d Nodes=%d\n",i,subg[i]->nEdges,subg[i]->nNet);
   }
   for (int i=0;i<g->nNet;i++)if (color[i]<0||color[i]>cluster_num-1) printf("ERR node %d,col=%d\n",i,color[i]);
   */
   #endif
   TABU *tabu=new TABU();
   //计算子图的SOLUTION
   set<int> servers;
   servers.clear();
   int t2=time(0);
   g_subsol_sz=cluster_num;
   for (int i=0;i<cluster_num;i++){
     int useTime=(Time-(t2-t1))/3*subg[i]->mynodes.size()/g->nNet;
     if (useTime<1)useTime=1;

     SOLUTION *subSol=tabu->Work(subg[i],200,20,NULL,useTime);
     g_subsol[i]=*subSol;                                      //保存用于加速
     for (set<int>::iterator it=subSol->servers.begin();it!=subSol->servers.end();it++){
        //printf("cluster[%d]: %d ",i,*it);
        #ifdef DEBUG
        if (servers.find(*it)!=servers.end()) {printf("unknown error! Node %d exist! curColor=%d color[%d]=%d\n",*it,i,*it,color[*it]);exit(-5);}
        #endif
        servers.insert(*it);

     }
     //putchar('\n');
   }
   //for (int i=0;i<cluster_num;i++) delete subg[i];
   //int t4=time(NULL);printf("work time:%d\n",t4-t1);
   SOLUTION *sol=GetMCMF(g,servers);//)tabu->Work(g,400,40,&servers,MAX_TIME-(t3-t1)-5);
   //int t5=time(0);printf("GetMCMF time:%d\n",t5-t1);
   return sol;
}
/*****************solver加强版************************************/
//二分节点，再合并
int color[MAXNODE];
TABU *GTabu=new TABU();
void colorGraph(GRAPH *g,int& c0,int& c1){ //g中节点01染色
   //找到两个中心
   int mx_dist=-1;
   for (int a1=0;a1<g->mynodes.size();a1++)
   for (int b1=0;b1<g->mynodes.size();b1++){
     int u=g->mynodes[a1];
     int v=g->mynodes[b1];
     if (g->dist[u][v]==-1){c0=u;c1=v;mx_dist=-1;a1=10000;break;}

     if (mx_dist < g->dist[u][v]){
        mx_dist=g->dist[u][v];
        c0=u;
        c1=v;
     }
   }
   //染色
   for (int a1=0;a1<g->mynodes.size();a1++){
     int u=g->mynodes[a1];
     if (g->dist[u][c1]==-1 || g->dist[u][c1]>g->dist[u][c0]) color[u]=0;
     else
     color[u]=1;
   }

}
//bool inCurGraph[MAXNODE];
void getSubGraph(GRAPH *g,GRAPH* subg[],bool updateDist){
   for (int col=0;col<2;col++){
       //当前cluster节点存入Nodes
       //建图
       GRAPH *g2=new GRAPH();
       subg[col]=g2;

       g2->nNet=g->nNet;
       g2->serCost=g->serCost;
       g2->nEdges=0;
       g2->nCon=0;
       g2->tolFlow=0;
       g2->con_id.clear();
       g2->con_need.clear();
   }
   //memset(inCurGraph,false,sizeof(inCurGraph));
   //for (int a1=0;a1<g->mynodes.size();a1++) inCurGraph[g->mynodes[a1]]=true;

   for (int i=0;i<g->nEdges;i++){  //边数和边
    int u=g->u[i];
    int v=g->v[i];
    int cap=g->cap[i];
    int cost=g->cost[i];
    if (color[u]!=color[v])continue;

    int c=color[u];
    subg[c]->u.push_back(u);
    subg[c]->v.push_back(v);
    subg[c]->cap.push_back(cap);
    subg[c]->cost.push_back(cost);
    subg[c]->nEdges++;

   }

    for (int i=0;i<g->con_id.size();i++){
      int u=g->con_id[i];
      int need=g->con_need[i];
      int c=color[u];
      subg[c]->con_id.push_back(u);
      subg[c]->con_need.push_back(need);
      subg[c]->tolFlow+=need;
      subg[c]->nCon++;
    }
    //mynodes
    for (int a1=0;a1<g->mynodes.size();a1++){
      int i=g->mynodes[a1];
      int c=color[i];
      subg[c]->mynodes.push_back(i);
    }
    if (updateDist){
      subg[0]->getDist();
      subg[1]->getDist();
    }
}
SOLUTION *solver2(GRAPH *g,int useTime=MAX_TIME){             //递归染色解决
   //点数<=50则直接算
   if (g->mynodes.size()<=150){
       SOLUTION *sol=GTabu->Work(g,300,20,NULL,useTime);
       return sol;
   }
   int t1=time(NULL);
   //g二分，且0/1染色，保存在color中
   int c0,c1;
   colorGraph(g,c0,c1);
   //建子图
   GRAPH *subg[2];
   bool updateDist=(g->dist[c0][c1]!=-1);
   getSubGraph(g,subg,updateDist);
   printf("Edge %d:%d  %.3lf\n",subg[0]->nEdges+subg[1]->nEdges,g->nEdges,1.0-1.0*(subg[0]->nEdges+subg[1]->nEdges)/g->nEdges);
   //递归子图，合并servers
   set<int>servers;
   SOLUTION *sol=solver2(subg[0],useTime/4);
   servers=sol->servers;

   sol=solver2(subg[1],useTime/2);

   for(set<int>::iterator it=sol->servers.begin();it!=sol->servers.end();it++){
     servers.insert(*it);
   }


   //释放空间
   int t2=time(NULL);
   useTime-=t2-t1;
   sol=GTabu->mergeSubGraph(g,subg,300,20,&servers,useTime,c0,c1);
   delete subg[0];
   delete subg[1];
   return sol;
}

/***********************************************************************/
//MCMF算法得到的路径不准确，要更新
int sta[MAXNODE];
int edgeId[MAXNODE];      //STA中相邻两个点边的id
int realCost;
int realFlow;
int ac[MAXNODE][MAXNODE];//两点直连的cost
int savePath(GRAPH*g,vector<PATH>&path,int nodeNum,int flow,int consumer_id){
   //保存path
   PATH p;
   p.node.clear();
   for (int i=1;i<=nodeNum;i++)p.node.push_back(sta[i]);
   p.flow=flow;
   g->con_need[consumer_id]-=flow;
   //减少途中的cap
   int costPerFlow=0;
   for (int i=1;i<nodeNum;i++){
     int eid=edgeId[i];
     edges[eid].cap-=flow;
     costPerFlow+=ac[edges[eid].from][edges[eid].to];//edges[eid].cost;
   }
   realCost+=costPerFlow*flow;
   realFlow+=flow;
   path.push_back(p);
   return 0;
}
bool pathDFS(GRAPH *g,vector<PATH>&path,int top,int flow){
   int u=sta[top];
   if (g->netToCon[u]!=-1){
      int consumer_id=g->netToCon[u];
      int need=min(flow,g->con_need[consumer_id]);
      if (need>0){ //构成一条路径了
        savePath(g,path,top,need,consumer_id);
        return true;
      }
   }
   for (int i=0;i<G[u].size();i++){
     EDGE e=edges[G[u][i]];
     int v=e.to;
     if (e.cap==0)continue;
     sta[top+1]=v;
     edgeId[top]=G[u][i];
     flow=min(flow,e.cap);
     return pathDFS(g,path,top+1,flow);
   }
   return false;
}
int af[MAXNODE][MAXNODE];  //两点直流的流量
int getRealPath(vector<PATH> &path,set<int>&servers,GRAPH *g){  //返回总cost;
  realCost=servers.size()*g->serCost;
  realFlow=0;
  for (int i=0;i<g->nEdges;i++){
    int u=g->u[i];
    int v=g->v[i];
    int cost=g->cost[i];
    ac[u][v]=ac[v][u]=cost;
  }
  //得到a
  memset(af,0,sizeof(af));
  for (int i=0;i<path.size();i++){
    PATH *p=&path[i];
    for (int j=1;j<p->node.size();j++){
       int u=p->node[j-1];
       int v=p->node[j];
       af[u][v]+=p->flow;
    }
  }
  //u->v与v->u流量中和
  for (int u=0;u<g->nNet;u++)
  for (int v=u+1;v<g->nNet;v++){
     if (af[u][v]>=af[v][u]){
       af[u][v]-=af[v][u];
       af[v][u]=0;
     }else{
       af[v][u]-=af[u][v];
       af[u][v]=0;
     }
  }
  //通过a建图
  edges.clear();
  for (int i=0;i<g->nNet;i++)G[i].clear();
  for (int i=0;i<g->nNet;i++)
  for (int j=0;j<g->nNet;j++)
    if (af[i][j]>0)
    addEdge(i,j,af[i][j],0);

  //构造新的path
  path.clear();
  for (set<int>::iterator it=servers.begin();it!=servers.end();it++){
     while (1){
       sta[1]=*it;
       if (pathDFS(g,path,1,INF)==false)break;
     }
  }
  #ifdef DEBUG
  //printf("realFlow=%d\n",realFlow);
  #endif
  return realCost;
}
SOLUTION* generate(GRAPH* g){//产生一个可行解
   #ifdef DEBUG
   int t1=GetTickCount();
   #endif
   SOLUTION *sol=solver(g,20);  //执行完solver后，edges仍是图g
   /*set<int> ser;
   do{
      ser.clear();
      for (int i=0;i<g->nNet;i++) ser.insert(rand()%g->nNet);
      sol=GetMCMF(g,ser);
      if (sol->isFesible==false){printf("not fesibel\n");}else break;
   } while (1);*/
   #ifdef DEBUG
   int t2=GetTickCount();
   #endif
   updateServers3(g,sol,MAX_TIME/8,g->serCost/16);
    #ifdef DEBUG
   int t3=GetTickCount();
   #endif
   sol=GetMCMF(g,sol->servers);
   #ifdef DEBUG
   int t4=GetTickCount();
   printf("Generate :%dms %dms %dms\n",t2-t1,t3-t2,t4-t3);
   #endif
   return sol;
}
set<int> nochange;
SOLUTION* cross(GRAPH*g,SOLUTION* sol1,SOLUTION* sol2){
   //int t1=GetTickCount();
   SOLUTION* sol;
   set<int> ppp=sol1->servers;
   for (set<int>::iterator it=sol2->servers.begin();it!=sol2->servers.end();it++){
      ppp.insert(*it);
   }
   //int t2=GetTickCount();
   sol=GetMCMF3(g,ppp,true,sol1,sol2);
   //int t3=GetTickCount();

   if (g_time2-g_time>=10){
       nochange.clear();
       for (set<int>::iterator it=sol1->servers.begin();it!=sol1->servers.end();it++)
       if (sol2->servers.count(*it))
       nochange.insert(*it);

       updateServers2(g,sol,MAX_TIME/8,g->serCost,nochange);
   }else
       updateServers(g,sol,MAX_TIME/8,g->serCost);
   //int t4=GetTickCount();
   //printf("CROSS %dms %dms %dms \n",t2-t1,t3-t2,t4-t3);
   //sol=GetMCMF3(g,sol->servers,true);

   #ifdef DEBUG
   /*
   printf("[CROSS] COST:%d ser=%d  +  cost:%d sz %d = cost:%d sz:%d\n",
          sol1->cost,sol1->servers.size(),
          sol2->cost,sol2->servers.size(),
          sol->cost,sol->servers.size());*/
   #endif
   return sol;
}
void getSerPath(SOLUTION*sol){
   for (int i=0;i<MAXNODE;i++)serPath[i].clear();
   for (int i=0;i<sol->path.size();i++)
     serPath[sol->path[i].node[0]].push_back(sol->path[i]);
}
#define max(a,b)  (a<b?b:a);
void compare_servers(set<int> &s1,set<int>&s2){
   printf("[compaire]: s1 sz=%d s2 sz=%d ",s1.size(),s2.size());
   int same=0;
   for (set<int>::iterator it=s1.begin();it!=s1.end();it++)
   if (s2.count(*it))same++;
   printf(" same:%d\n",same);
}
void mutex(GRAPH* g,SOLUTION* sol){  //变异
   set<int> s1=sol->servers;
   printf("MUTEX:  Ser SZ=%d\n",sol->servers.size());
   bool isSer[MAXNODE];
   for (int i=0;i<MAXNODE;i++) isSer[i]=false;
   for (set<int>::iterator it=sol->servers.begin();it!=sol->servers.end();it++)
      isSer[*it]=true;
   //中间节点出现次数最多的
   //20%的服务器,加入其相邻节点
   int del_nodes=max(1,sol->servers.size()/3);
   while (del_nodes--){
      int del_id;
      do{
          del_id=rand()%g->nNet;
      }while (isSer[del_id]==false);
      bool isCon=false;
      for (int i=0;i<serPath[del_id].size();i++)
      if (serPath[del_id][i].node.size()>2)
        isSer[serPath[del_id][i].node[1]]=true;
        else isCon=true;
     // if (!isCon)isSer[del_id]=false;
   }
   //流量最多的中间节点
   int flow[MAXNODE];
   memset(flow,0,sizeof(flow));
   for (int i=0;i<sol->path.size();i++){
     PATH *p=&(sol->path[i]);
     for (int j=1;j<p->node.size();j++)
       flow[p->node[j]]+=p->flow;
   }
   int add=(g->nNet-sol->servers.size())/3;
   while (add--){
     int mx_flow=0,mx_id=0;
     for (int i=0;i<g->nNet;i++){
        if (mx_flow<flow[i]) mx_flow=flow[i],mx_id=i;
     }
     isSer[mx_id]=true;
     isSer[rand()%g->nNet]=true;
   }
   //随机选择
   for (int i=0;i<sol->servers.size()/5;i++)
   isSer[rand()%g->nNet]=true;
   sol->servers.clear();
   for (int i=0;i<MAXNODE;i++)
     if (isSer[i])
     sol->servers.insert(i);
   printf("    服务器扩大到=%d\n",sol->servers.size());
   *sol=*GetMCMF(g,sol->servers);
   updateServers(g,sol,MAX_TIME/8,g->serCost);
}
#define SZ 10
SOLUTION *group[SZ];
SOLUTION *nextGroup[SZ*SZ-SZ];
SOLUTION* Genetic(GRAPH* g){
   for (int i=0;i<SZ;i++) group[i]=new SOLUTION();
   for (int i=0;i<SZ*SZ-SZ;i++)nextGroup[i]=new SOLUTION();

   //两次 然后合并
   int t1=time(0);
   int sz=0;
   for (int i=0;i<SZ;i++){
      *group[sz++]=*generate(g);
      #ifdef DEBUG
      printf("Generate %d cost=%d\n",i,group[i]->cost);
      #endif
      if (time(0)-t1>=40) break;
      if (sz>=5 && time(0)-t1>20)break;
   }
   #ifdef DEBUG
   printf("============USE TIME %d===============\n",time(0)-g_time);
   #endif
   int epoch=10;
   int sz2=sz;
   while (epoch--){
      if (sz>sz2)sz=sz2;
      if (MAX_TIME-(time(0)-g_time)<=12)break;
      int t2=time(0);
      if (sz==1)break;
      //保证group[0]是当前最好的，最好的会一直遗传下去
      for (int i=1;i<sz;i++)
       if (group[0]->cost > group[i]->cost)
       swap(group[0],group[i]);

      int nextIdx=0;
      for (int sum=1;sum<=2*sz-3;sum++)
        for (int i=0;i<sz;i++){
          int j=sum-i;
          if (j<=i)break;
          if (j>=sz)continue;
          if (group[i]->cost==group[j]->cost)continue;
          *nextGroup[nextIdx]=*cross(g,group[i],group[j]);
          //printf("nxt:%d\n",nextGroup[nextIdx]->cost);
          nextIdx++;
          int timeLeft=MAX_TIME-5-(time(0)-g_time);
          if (timeLeft<8) {sum=2*sz;break;}
          if (nextIdx>=sz2 && time(0)-t2>=20){sum=2*sz;break;}            //耗时多时，当前epoch产生足够的下一代，就直接退出

        }
      //printf("Cross Time:%d generate:%d\n",time(0)-t2,nextIdx);
      //nextGroup排序(并剔除重复cost)
      int nextSz=nextIdx;
      for (int i=nextSz-1;i>=0;i--)
      for (int j=0;j<i;j++)
      if (nextGroup[j]->cost == nextGroup[i]->cost)
        nextGroup[i]->cost=INF;

      for (int i=0;i<nextSz;i++)
        for (int j=i+1;j<nextSz;j++){
            if (nextGroup[j]->cost < nextGroup[i]->cost)
            swap(nextGroup[j],nextGroup[i]);
        }
      sz=1;
      while(nextGroup[sz]->cost !=INF && sz<SZ)sz++;

      for (int i=1;i<sz;i++) {
        *group[i]=*nextGroup[i-1];
      }
      if (group[0]->cost == group[1]->cost) {
        for (int i=1;i<sz-1;i++)swap(group[i],group[i+1]);
        sz--;
      }
      #ifdef DEBUG
      printf("Epoch :%d [TIME]%d\n",epoch,time(0)-t2);for (int i=0;i<sz;i++)printf("%d ",group[i]->cost);putchar('\n');
      #endif
   }
   SOLUTION* sol=group[0];
   for (int i=0;i<SZ;i++)if (sol->cost>group[i]->cost)
   sol=group[i];
   group[1]=GetMCMF(g,sol->servers);//由于反向弧被无视，重新计算mcmf可能导致更差的结果
   if (group[1]->cost < sol->cost) sol=group[1];
   return sol;
}
#endif
