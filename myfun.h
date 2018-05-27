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
  //�ҵ���һ������
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
struct PATH{                      //������
    int flow;
    int cost;                     //����·������
    vector<int> node;
    vector<int> eid;              //����edges�е�id  ����Դ��ͻ�㡣��size��node��1�� edges[eid[i]].to=node[i] and edges[eid[sz-1].to]=���
    PATH(){flow=0;node.clear();}
};
struct  SOLUTION{                 //����������Ϊservers���˽ṹ�屣������С���������
    bool   isFesible;             //�Ƿ����
    int    cost;                  //��ǰ�����Ļ���
    int    reserved;
    set<int> servers;             //ѡ��ķ�����
    vector<PATH>path;             //���е�·��
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
    /*���¼�¼ԭʼ������*/
    int nNet;          //nNet������ڵ�
    int nEdges;        //nEdges����
    int nCon;          //nCon�����ѽڵ�
    int serCost;       //����������ɱ�
    vector<int>   mynodes;//0..nNet-1��Щ�ڵ��У�����nodes�Ľڵ�
    //��
    vector<int>   u;
    vector<int>   v;
    vector<int>   cap;
    vector<int>   cost;
    //���ѽڵ�
    int           netToCon[MAXNODE];
    vector<int>   con_id;   //���ѽڵ��Ӧ������ڵ���
    vector<int>   con_need; //���ѽڵ�����Ĵ���
    /* ���¶�ͼ��һЩ���Խ�����չ �����·*/
    //�ڵ�������·��kmeans��
    int dist[MAXNODE][MAXNODE];
    int tolFlow;                                //�����������
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
     //�����
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
     //�������ѽڵ�
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
     //����������Ϣ
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
//ÿ��ִ����solver�ֿ����ͼ��graph��solution����¼��������
SOLUTION g_subsol[30];
GRAPH g_subg[30];
int g_subsol_sz;
/*************************************************��С������*************************************/
//����Ϊ��С������
vector<EDGE>       edges;                                //����ͼ
vector<int>           G[MAXNODE];
int n,m,s,t;                                                         //n�ڵ㣬m�ߣ�s,t
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
  //��¼·��
  PATH *path=&path_for_getmcmf;
  path->flow=a[t];
  path->cost=a[t]*d[t];
  path->node.resize(pathNodes);
  path->eid.resize(pathNodes+1);
  u=edges[p[t]].from;          //Դ��ͻ�������¼
  path->eid[pathNodes]=p[t];
  int pathId=pathNodes-1;      //·��Ҫ�ӷ����������ѽڵ㷽���¼
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
//����һ��SOLUTIONָ�룬���������������Ҫ�õ���ָ������ݣ�Ҫ���⸴��һ�����ݡ�����ָ��ָ������ݻᱻ���¸��ǡ�
    //��ͼ
    set<int> servers=ser;
    n=g->nNet+2;
    s=n-2;
    t=n-1;
    edges.clear();
    for (int i=0;i<n;i++)G[i].clear();
    //��·
    for (int i=0;i<g->nEdges;i++){
         addEdge(g->u[i],g->v[i],g->cap[i],g->cost[i]);
         addEdge(g->v[i],g->u[i],g->cap[i],g->cost[i]);
    }

    //�� consume node -> t  cap =con_need  cost=0
    for (int i=0;i<g->nCon;i++)
         addEdge(g->con_id[i],t,g->con_need[i],0);
    //�� s-> server cap=INF cost=0
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
void insertPath2(PATH& p){  //·����ӵ�edges����ͼ��
  for (int i=0;i<p.eid.size();i++){
    int id=p.eid[i];
    edges[id].flow+=p.flow;
  }
}
SOLUTION * GetMCMF3(GRAPH* g,set<int>& ser,bool mapreuse,SOLUTION* sol1=NULL,SOLUTION* sol2=NULL){
//����һ��SOLUTIONָ�룬���������������Ҫ�õ���ָ������ݣ�Ҫ���⸴��һ�����ݡ�����ָ��ָ������ݻᱻ���¸��ǡ�
    //��ͼ
    set<int> servers=ser;
if (!mapreuse){
    n=g->nNet+2;
    s=n-2;
    t=n-1;
    edges.clear();
    for (int i=0;i<n;i++)G[i].clear();
    //��·
    for (int i=0;i<g->nEdges;i++){
         addEdge(g->u[i],g->v[i],g->cap[i],g->cost[i]);
         addEdge(g->v[i],g->u[i],g->cap[i],g->cost[i]);
    }

    //�� consume node -> t  cap =con_need  cost=0
    for (int i=0;i<g->nCon;i++)
         addEdge(g->con_id[i],t,g->con_need[i],0);
    //�� s-> server cap=INF cost=0
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
    //���þɵ�·������
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
//����һ��SOLUTIONָ�룬���������������Ҫ�õ���ָ������ݣ�Ҫ���⸴��һ�����ݡ�����ָ��ָ������ݻᱻ���¸��ǡ�
    //��ͼ
    set<int> servers=ser;
    //��ͼû�䣬�������½�
if (mapreuse==false){
    n=g->nNet+2;
    s=n-2;
    t=n-1;
    edges.clear();
    for (int i=0;i<n;i++)G[i].clear();
    //��·
    for (int i=0;i<g->nEdges;i++){
         addEdge(g->u[i],g->v[i],g->cap[i],g->cost[i]);
         addEdge(g->v[i],g->u[i],g->cap[i],g->cost[i]);
    }

    //�� consume node -> t  cap =con_need  cost=0
    for (int i=0;i<g->nCon;i++)
         addEdge(g->con_id[i],t,g->con_need[i],0);
     //�� s-> server cap=INF cost=0
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
    //�Ѿɵ�·������ͼ��
    if (curSol->servers.size() < servers.size()){ //˵���Ǽӵ�
        int addNode;
           for (set<int>::iterator it=servers.begin();it!=servers.end();it++)
          if (curSol->servers.count(*it)==0){
             addNode=*it;
             break;
          }
        for (int i=0;i<curSol->path.size();i++){
          PATH *p=&curSol->path[i];
          int targetNode=p->node[p->node.size()-1];  //Ŀ��ڵ�
          int flowDist=p->cost/p->flow;
          if (g->dist[addNode][targetNode] >=flowDist){ //·�����뵱ǰ��
              insertPath2(*p);
              sol->path.push_back(*p);
              flow+=p->flow;
              cost+=p->cost;
          }
        }
    }
    else{                                          //˵���Ǽ��ٽڵ�
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
/***********************�Ż�������*******************************/
/***************���²�������edges����ͼ�в���******************/
  vector<PATH> serPath[MAXNODE];

void removePath(PATH& p){  //·����edges����ͼ��ɾ��
  for (int i=0;i<p.eid.size();i++){
    int id=p.eid[i];
    edges[id].flow-=p.flow;
    #ifdef OPPOSITE_EDGE
    edges[id^1].flow+=p.flow;
    #endif
  }
}
void insertPath(PATH& p){  //·����ӵ�edges����ͼ��
  for (int i=0;i<p.eid.size();i++){
    int id=p.eid[i];
    edges[id].flow+=p.flow;
    #ifdef DEBUG
    if (edges[id].cap<edges[id].flow) {
      printf("·�����뵼��cap�����eid=%d  from %d to %d cap=%d flow=%d\n",id,edges[i].from,edges[i].to,edges[id].cap,edges[id].flow);
      exit(-4);
    }
    #endif
    #ifdef OPPOSITE_EDGE
    edges[id^1].flow-=p.flow;
    #endif
  }
}
//s->sid������cap����Ϊcap

void set_s_sid_cap(int sid,int cap){
  for (int i=edges.size()-1;i>=0;i--){
    if (edges[i].from==s && edges[i].to==sid){  //s->sid�����ߵ�flowӦ��Ϊ0.�������������������������ڣ���һ��Ϊ0.
        edges[i].cap=cap;
        return;
    }
  }
}
//removeSerֻ��������ͼ
void removeSer(int sid,vector<PATH>&p,int &ser_flow,int& ser_cost){     //edges����ͼ��ɾ���������������ش˷�����δɾ��������
  // Դ��->sid ������cap��Ϊ0
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
  for (int i=0;i<newPath.size();i++)      removePath(newPath[i]);     //ɾ���¼����·��
  // Դ��->sid�������ߣ�cap����ΪINF
  /*for (int i=edges.size()-1;i>=0;i--){
    if (edges[i].from==s && edges[i].to==sid){  //s->sid�����ߵ�flowӦ��Ϊ0.�������������������������ڣ���һ��Ϊ0.
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
  //�����·��
  for (int i=0;i<serPath[sid].size();i++) insertPath(serPath[sid][i]);
}


set<int> tabu_ser;                                                      //��ɾ�����޷��ٴδ���Ľڵ�
int arrSer[MAXNODE];
int chooseAser(set<int>&curSer){              //ѡ��һ��������������(����ѡ��������������ǿ��=·���٣�)
  //������λ��������������ѡ�������
  int sz=0;
  for (set<int>::iterator it=curSer.begin();it!=curSer.end();it++){
     if (tabu_ser.find(*it)==tabu_ser.end())
     arrSer[sz++]=*it;
  }
  if (sz==0)return -1;
  int k=max(2,sz/20);                                                   //��5%�Ľڵ���ѡ��
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
    //��·
    for (int i=0;i<g->nEdges;i++){
         addEdge(g->u[i],g->v[i],g->cap[i],g->cost[i]);
         addEdge(g->v[i],g->u[i],g->cap[i],g->cost[i]);
         //printf("build addedge:%d %d %d %d\n",g->u[i],g->v[i],g->cap[i],g->cost[i]);
    }
    //�� consume node -> t  cap =con_need  cost=0
    for (int i=0;i<g->nCon;i++)
         addEdge(g->con_id[i],t,g->con_need[i],0);
    //�� s-> server cap=INF cost=0
    //s->��ͨ�ڵ���(0,0)�ıߡ������ɷ��������޸���cap����
    for (int i=0;i<g->nNet;i++)
      if (servers.find(i)==servers.end())
        addEdge(s,i,0,0);
      else
        addEdge(s,i,INF,0);
     */
    //��serPath����ͼ��
   for (int i=0;i<g->nNet;i++)
    for (int j=0;j<serPath[i].size();j++){
       insertPath(serPath[i][j]);
    }
}
//���ٷ������ڵ㡣ԭ���޸Ĳ���ͼ���޸�serPath��sol�г���path�����£�����(cost,servers)��Ҫ����
void reduceServers(GRAPH* g,SOLUTION *sol,int diff,int useTime){
  #ifdef DEBUG
  //printf("Diff=%d sz=%d before reduce :%d\n",diff,sol->servers.size(),sol->cost);
  #endif
  int ts=time(0);
  vector<PATH> newPath;
  int costBeforeRemove=sol->cost;                                                 //��ǰ�ܷ���
  int flag=sol->servers.size();
  tabu_ser.clear();
  while (1){
    //ѡ��һ���ڵ�
    int sid=chooseAser(sol->servers);
    if (sid==-1){break;}
    int ser_flow,ser_cost;                                                     //�˷����������������ͷ���
    removeSer(sid,serPath[sid],ser_flow,ser_cost);                             //������
    //printf("servers [%d] need flow %d cost %d  ",sid,ser_flow,ser_cost);
    int costAfterRemove=costBeforeRemove-ser_cost-g->serCost;                  //��ǰ��cost
    PATH *path;

    newPath.clear();
    //�ڲ���ͼ�м�������
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
        //�����·����serPath
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
  vector<PATH> newPath;                                                           //�ɽڵ�ɾ�����½ڵ���Ӻ󣬵���bellmanford��������·��
  int costBeforeRemove=sol->cost;                                                 //��ǰ�ܷ���
  //���浱ǰ���������������У��������ѡ��
  int sz=0;
  for (set<int>::iterator it=sol->servers.begin();it!=sol->servers.end();it++) arrSer[sz++]=*it;
  int flag=0;
  while (1){
    //ѡ��һ���ڵ㣬�ʹ˽ڵ����ڵķǷ������ڵ�
    int oldNodeId=rand()%sz;
    int oldNode=arrSer[oldNodeId];
    int pathId=rand()%serPath[oldNode].size();
    if (serPath[oldNode][pathId].node.size()<2)continue;
    int newNode=serPath[oldNode][pathId].node[1];//edges[G[oldNode][rand()%G[oldNode].size()]].to;
    int ser_flow,ser_cost;                                                     //�˷����������������ͷ���
    //ɾ���ɽڵ�oldNode
    removeSer(oldNode,serPath[oldNode],ser_flow,ser_cost);                             //������
    //����½ڵ�
    set_s_sid_cap(newNode,INF);
    int costAfterRemove=costBeforeRemove-ser_cost;                  //��ǰ��cost
    PATH *path;
    newPath.clear();
    //�ڲ���ͼ�м�������
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
        //�����·����serPath
        for (int i=0;i<newPath.size();i++) serPath[newPath[i].node[0]].push_back(newPath[i]);
        serPath[oldNode].clear();                                                                //�߱���
        sol->servers.erase(oldNode);                                                             //�ڵ㱣��
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
//���ٷ������ڵ㡣ԭ���޸Ĳ���ͼ���޸�serPath��sol�г���path�����£�����(cost,servers)��Ҫ����
void reduceServers2(GRAPH* g,SOLUTION *sol,int diff,int useTime,set<int>&nochange){
  #ifdef DEBUG
  //printf("Diff=%d sz=%d before reduce :%d\n",diff,sol->servers.size(),sol->cost);
  #endif
  int ts=time(0);
  vector<PATH> newPath;
  int costBeforeRemove=sol->cost;                                                 //��ǰ�ܷ���
  int flag=sol->servers.size();
  tabu_ser.clear();
  set<int> ser;
  for(set<int>::iterator it=sol->servers.begin();it!=sol->servers.end();it++){
    if (nochange.find(*it)==nochange.end())
      ser.insert(*it);
  }
  set<int>::iterator it=ser.begin();
  while (1){
    //ѡ��һ���ڵ�
    //int sid=chooseAser(ser);
    //if (sid==-1){break;}
    if (ser.size()==0)break;
    int sid=chooseAser(ser);//*ser.begin();
    ser.erase(sid);
    //if (rand()<100)break;                                                      //

    int ser_flow,ser_cost;                                                     //�˷����������������ͷ���
    removeSer(sid,serPath[sid],ser_flow,ser_cost);                             //������
    //printf("servers [%d] need flow %d cost %d  ",sid,ser_flow,ser_cost);
    int costAfterRemove=costBeforeRemove-ser_cost-g->serCost;                  //��ǰ��cost
    PATH *path;

    newPath.clear();
    //�ڲ���ͼ�м�������
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
        //�����·����serPath
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
void updateServers(GRAPH* g,SOLUTION* sol,int useTime,int diff){                  //����������������servers���뱣֤����
  //ÿһ��������������·�������ڼ�ȥ�ⲿ��·��
  #ifdef DEBUG
  //printf("-----before shrink SerSz=%d Cost=%d--------\n",sol->servers.size(),sol->cost);
  #endif
  int ts=time(NULL);
  for (int i=0;i<MAXNODE;i++)serPath[i].clear();                         //ÿ����������·������
  for (int i=0;i<sol->path.size();i++){
      serPath[sol->path[i].node[0]].push_back(sol->path[i]);
  }

  //��ͼ
  buildResidentGraph(g,sol->servers);
  while (diff){
    if (diff<20)diff=0;else diff/=4;
    reduceServers(g,sol,diff,useTime-(time(0)-ts));
    //moveServer(g,sol,3);
    //printf("diff=%d\n",diff);
  }
  //����sol
  sol->path.clear();
  for (int i=0;i<g->nNet;i++)
    for (int j=0;j<serPath[i].size();j++)
    sol->path.push_back(serPath[i][j]);
  sol->isFesible=true;
  #ifdef DEBUG
  //printf("-----after shrink SerSz=%d Cost=%d--------\n",sol->servers.size(),sol->cost);
  #endif
}
//updateServers3 ʹ��g_subsol���м���
void updateServers3(GRAPH* g,SOLUTION* sol,int useTime,int diff){                  //����������������servers���뱣֤����
  //ÿһ��������������·�������ڼ�ȥ�ⲿ��·��
  #ifdef DEBUG
  //printf("-----before shrink SerSz=%d Cost=%d--------\n",sol->servers.size(),sol->cost);
  #endif
  int ts=time(NULL);
  for (int i=0;i<MAXNODE;i++)serPath[i].clear();                         //ÿ����������·������
  for (int i=0;i<sol->path.size();i++){
      serPath[sol->path[i].node[0]].push_back(sol->path[i]);
  }
  //�����ı�Ľڵ�(ÿһ����ͼ��70%�ڵ㲻�䡣��������Զ��70%
  /*set<int> noChange;noChange.clear();
  set<int> inOtherG[MAXNODE];inOtherG=sol->servers;
  set<int>::iterator it;
  for (int gid=0;gid<g_subsol_sz;gid++){
      for (it=g_subg[gid].servers.begin();it!=g_subg[gid].servers.end();it++) inOtherG.erase(*it);
      int nochange_sz=g_subg.mynodes.size()*7/10;  //70%
      while (nochange_sz--){
          int mx_dist=-1,mx_node; //��������������
          for (it=g_subg[gid]

      }
      for (it=g_subg[gid].servers.begin();it!=g_subg[gid].servers.end();it++) inOtherG.insert(*it);
  }*/
  //��ͼ
  buildResidentGraph(g,sol->servers);
  while (diff){
    if (diff<20)diff=0;else diff/=4;
    reduceServers(g,sol,diff,useTime-(time(0)-ts));
    //moveServer(g,sol,3);
    //printf("diff=%d\n",diff);
  }
  //����sol
  sol->path.clear();
  for (int i=0;i<g->nNet;i++)
    for (int j=0;j<serPath[i].size();j++)
    sol->path.push_back(serPath[i][j]);
  sol->isFesible=true;
  #ifdef DEBUG
  //printf("-----after shrink SerSz=%d Cost=%d--------\n",sol->servers.size(),sol->cost);
  #endif
}
void updateServers2(GRAPH* g,SOLUTION* sol,int useTime,int diff,set<int>& noChange){                  //����������������servers���뱣֤����
  //updateServers(g,sol,useTime,diff);return ;
  //ÿһ��������������·�������ڼ�ȥ�ⲿ��·��
  //����һ���� noChange servers��20%�Ĵ�С��������Ҫ�ı�ľ���Խ����ԽҪתΪneedChange
  set<int> needChange;needChange.clear();
  for (set<int>::iterator it=sol->servers.begin();it!=sol->servers.end();it++)
    if (noChange.count(*it)==0)
      needChange.insert(*it);
  int del_sz=noChange.size()/10;
  while (del_sz--){
      int min_dist=INF,min_node=-1;
      for (set<int>::iterator it=noChange.begin();it!=noChange.end();it++){
         int dist=INF;//�ڵ�*it����needChange����̵ľ���
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
  for (int i=0;i<MAXNODE;i++)serPath[i].clear();                         //ÿ����������·������
  for (int i=0;i<sol->path.size();i++){
      serPath[sol->path[i].node[0]].push_back(sol->path[i]);
  }

  //��ͼ
  buildResidentGraph(g,sol->servers);
  while (diff){
    if (diff<20)diff=0;else diff/=4;
    reduceServers2(g,sol,diff,useTime-(time(0)-ts),noChange);
    //reduceServers(g,sol,diff,useTime-(time(0)-ts));
    //moveServer(g,sol,3);
    //printf("diff=%d\n",diff);
  }
  //����sol
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
  vector<int> pathId[MAXNODE];              //ÿ��������ռ�õ�path·������
  void getpathId(vector<PATH>& path){
     for (int i=0;i<MAXNODE;i++)pathId[i].clear();
     for (int i=0;i<path.size();i++){
       pathId[path[i].node[0]].push_back(i);
     }
  }
  int flow[MAXNODE];
  int move(GRAPH *g,int netId,vector<PATH>& TotPath){   //�ƶ�servers
      int totflow=0;
      memset(flow,0,sizeof(flow));
      for (int i=0;i<pathId[netId].size();i++){   //ö�ٵ�ǰ�ڵ�netId������������
          int pid=pathId[netId][i];
          PATH *path=&TotPath[pid];
          totflow+=path->flow;
          for (int nodeId=1;nodeId < path->node.size();nodeId++){ //ö��ĳ�������������һ���ڵ�Ϊ����
            int curNode=path->node[nodeId];
            flow[curNode]+=path->flow;
          }
      }
      //��������һ�룬��dist��Զ
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
  set<int> nxtServ;                                  //�����ƶ����������������
  set<int>* moveServers(GRAPH *g,SOLUTION *sol){    //servers�������ƶ�����NULL
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
  //���
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
    SOLUTION*      best;  //  ��ǰ�����������Ž�
    SOLUTION*      curSol;//  servers ��ǰ��������Sol
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
    /* �������ʼservers */
    set<int>servers;
    servers.clear();
    if (initSer==NULL){
      for (int i=0;i<g->nCon;i++)servers.insert(g->con_id[i]);
    }
    else
      servers=*initSer;
    //��ʼ��������
    curSol->init();

    *curSol=*GetMCMF(g,servers);                        //������

    best->init();
    updateSol();
    return 0;
}
//int getNodes
#define  Rand(p)  (RAND_MAX*(p)>rand())             //��p���ʷ���true
SOLUTION* TABU::Work(GRAPH *g,int epoch,int eta,set<int>*initSer,int time_limit){         //����eta��epoch��û�и��õĽ⣬���˳�
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
//work �ظ����ò���ͼ����
SOLUTION* TABU::Work_plus(GRAPH *g,int epoch,int eta,set<int>*initSer,int time_limit){         //����eta��epoch��û�и��õĽ⣬���˳�

    time_limit=max(time_limit,1);
    time_t time_start=time(NULL);

    init(g,initSer);
    int Epoch=epoch;
    for (int i=0;i<MAXNODE;i++)serPath[i].clear();                         //ÿ����������·������
    for (int i=0;i<curSol->path.size();i++){
       serPath[curSol->path[i].node[0]].push_back(curSol->path[i]);
    }
    //initִ�к�(getmcmfҲִ����)����ʱ����ͼ��������edges��
     vector<PATH> newPath;                                                           //�ɽڵ�ɾ�����½ڵ���Ӻ󣬵���bellmanford��������·��
     while (Epoch--){
        int choose=-1;
        int min_cost=INF;
        for (int i=0;i<20;i++){  //���ѡ��һ���ڵ�
           int nodeId=g->mynodes[rand()%g->mynodes.size()];
           newPath.clear();
           if (curSol->servers.find(nodeId)!=curSol->servers.end()){                 //���
               set_s_sid_cap(nodeId,INF);
           }else{                                                                    //ɾ��
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
SOLUTION* TABU::mergeSubGraph(GRAPH *g,GRAPH* subg[2],int epoch,int eta,set<int>*initSer,int time_limit,int c0,int c1){         //����eta��epoch��û�и��õĽ⣬���˳�
    time_limit=max(time_limit,1);
    time_t time_start=time(NULL);
    init(g,initSer);
    int Epoch=epoch;
    //�̶������·��
    //printf("GetRealPath!\n");
    //getRealPath(curSol->path,curSol->servers,g);
    //vector<PATH> fixed;
    //�����ֽڵ�Ⱦɫ
    bool color[MAXNODE];
    for (int i=0;i<subg[0]->mynodes.size();i++) color[subg[0]->mynodes[i]]=0;
    for (int i=0;i<subg[1]->mynodes.size();i++) color[subg[1]->mynodes[i]]=1;
    /*
    bool activeNode[MAXNODE];//����c0/c1����ԽԶ��Խ����Ծ
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
         //����

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
int kmeans(GRAPH *g,int color[],int AVG_NODE_NUM){  //���ط������(dist=-1����ͨ)

   int k=max(1,g->nNet/AVG_NODE_NUM);//0~k-1Ⱦɫ
   k=min(25,k);
   //��ʼ���ĵ�
   int cent[30];
   for (int i=0;i<k;i++){
       cent[i]=g->nNet/k*i;
       cent[i]+=rand()%g->nNet/k;
   }
   //k-means�㷨
   bool hasChange=true;           //���ĵ㲻������kmeans
   int cnt=0;
   while (hasChange&&cnt++<10){
     //for (int i=0;i<k;i++)printf("%d ",cent[i]);putchar('\n');system("pause");
     hasChange=false;
     //�ڵ�Ⱦɫ
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
     //cluster�������¼���
     for (int col=0;col<k;col++){
       int newCenter=0;
       int minDistSum=INF;

       for (int c=0;c<cluster[col].size();c++){
         int u=cluster[col][c];                        //uΪcol��ɫ�����ĵ�
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
       //�������ĵ�
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
       //��ǰcluster�ڵ����Nodes
       //��ͼ
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
   for (int i=0;i<g->nEdges;i++){  //�����ͱ�
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
   //kmeans�ڵ�Ⱦɫ
   int color[MAXNODE];
   int t1=time(NULL);
   memset(color,0xff,sizeof(color));
   int cluster_num=kmeans(g,color,30+rand()%50);
   //����ͼ
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
   //������ͼ��SOLUTION
   set<int> servers;
   servers.clear();
   int t2=time(0);
   g_subsol_sz=cluster_num;
   for (int i=0;i<cluster_num;i++){
     int useTime=(Time-(t2-t1))/3*subg[i]->mynodes.size()/g->nNet;
     if (useTime<1)useTime=1;

     SOLUTION *subSol=tabu->Work(subg[i],200,20,NULL,useTime);
     g_subsol[i]=*subSol;                                      //�������ڼ���
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
/*****************solver��ǿ��************************************/
//���ֽڵ㣬�ٺϲ�
int color[MAXNODE];
TABU *GTabu=new TABU();
void colorGraph(GRAPH *g,int& c0,int& c1){ //g�нڵ�01Ⱦɫ
   //�ҵ���������
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
   //Ⱦɫ
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
       //��ǰcluster�ڵ����Nodes
       //��ͼ
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

   for (int i=0;i<g->nEdges;i++){  //�����ͱ�
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
SOLUTION *solver2(GRAPH *g,int useTime=MAX_TIME){             //�ݹ�Ⱦɫ���
   //����<=50��ֱ����
   if (g->mynodes.size()<=150){
       SOLUTION *sol=GTabu->Work(g,300,20,NULL,useTime);
       return sol;
   }
   int t1=time(NULL);
   //g���֣���0/1Ⱦɫ��������color��
   int c0,c1;
   colorGraph(g,c0,c1);
   //����ͼ
   GRAPH *subg[2];
   bool updateDist=(g->dist[c0][c1]!=-1);
   getSubGraph(g,subg,updateDist);
   printf("Edge %d:%d  %.3lf\n",subg[0]->nEdges+subg[1]->nEdges,g->nEdges,1.0-1.0*(subg[0]->nEdges+subg[1]->nEdges)/g->nEdges);
   //�ݹ���ͼ���ϲ�servers
   set<int>servers;
   SOLUTION *sol=solver2(subg[0],useTime/4);
   servers=sol->servers;

   sol=solver2(subg[1],useTime/2);

   for(set<int>::iterator it=sol->servers.begin();it!=sol->servers.end();it++){
     servers.insert(*it);
   }


   //�ͷſռ�
   int t2=time(NULL);
   useTime-=t2-t1;
   sol=GTabu->mergeSubGraph(g,subg,300,20,&servers,useTime,c0,c1);
   delete subg[0];
   delete subg[1];
   return sol;
}

/***********************************************************************/
//MCMF�㷨�õ���·����׼ȷ��Ҫ����
int sta[MAXNODE];
int edgeId[MAXNODE];      //STA������������ߵ�id
int realCost;
int realFlow;
int ac[MAXNODE][MAXNODE];//����ֱ����cost
int savePath(GRAPH*g,vector<PATH>&path,int nodeNum,int flow,int consumer_id){
   //����path
   PATH p;
   p.node.clear();
   for (int i=1;i<=nodeNum;i++)p.node.push_back(sta[i]);
   p.flow=flow;
   g->con_need[consumer_id]-=flow;
   //����;�е�cap
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
      if (need>0){ //����һ��·����
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
int af[MAXNODE][MAXNODE];  //����ֱ��������
int getRealPath(vector<PATH> &path,set<int>&servers,GRAPH *g){  //������cost;
  realCost=servers.size()*g->serCost;
  realFlow=0;
  for (int i=0;i<g->nEdges;i++){
    int u=g->u[i];
    int v=g->v[i];
    int cost=g->cost[i];
    ac[u][v]=ac[v][u]=cost;
  }
  //�õ�a
  memset(af,0,sizeof(af));
  for (int i=0;i<path.size();i++){
    PATH *p=&path[i];
    for (int j=1;j<p->node.size();j++){
       int u=p->node[j-1];
       int v=p->node[j];
       af[u][v]+=p->flow;
    }
  }
  //u->v��v->u�����к�
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
  //ͨ��a��ͼ
  edges.clear();
  for (int i=0;i<g->nNet;i++)G[i].clear();
  for (int i=0;i<g->nNet;i++)
  for (int j=0;j<g->nNet;j++)
    if (af[i][j]>0)
    addEdge(i,j,af[i][j],0);

  //�����µ�path
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
SOLUTION* generate(GRAPH* g){//����һ�����н�
   #ifdef DEBUG
   int t1=GetTickCount();
   #endif
   SOLUTION *sol=solver(g,20);  //ִ����solver��edges����ͼg
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
void mutex(GRAPH* g,SOLUTION* sol){  //����
   set<int> s1=sol->servers;
   printf("MUTEX:  Ser SZ=%d\n",sol->servers.size());
   bool isSer[MAXNODE];
   for (int i=0;i<MAXNODE;i++) isSer[i]=false;
   for (set<int>::iterator it=sol->servers.begin();it!=sol->servers.end();it++)
      isSer[*it]=true;
   //�м�ڵ���ִ�������
   //20%�ķ�����,���������ڽڵ�
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
   //���������м�ڵ�
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
   //���ѡ��
   for (int i=0;i<sol->servers.size()/5;i++)
   isSer[rand()%g->nNet]=true;
   sol->servers.clear();
   for (int i=0;i<MAXNODE;i++)
     if (isSer[i])
     sol->servers.insert(i);
   printf("    ����������=%d\n",sol->servers.size());
   *sol=*GetMCMF(g,sol->servers);
   updateServers(g,sol,MAX_TIME/8,g->serCost);
}
#define SZ 10
SOLUTION *group[SZ];
SOLUTION *nextGroup[SZ*SZ-SZ];
SOLUTION* Genetic(GRAPH* g){
   for (int i=0;i<SZ;i++) group[i]=new SOLUTION();
   for (int i=0;i<SZ*SZ-SZ;i++)nextGroup[i]=new SOLUTION();

   //���� Ȼ��ϲ�
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
      //��֤group[0]�ǵ�ǰ��õģ���õĻ�һֱ�Ŵ���ȥ
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
          if (nextIdx>=sz2 && time(0)-t2>=20){sum=2*sz;break;}            //��ʱ��ʱ����ǰepoch�����㹻����һ������ֱ���˳�

        }
      //printf("Cross Time:%d generate:%d\n",time(0)-t2,nextIdx);
      //nextGroup����(���޳��ظ�cost)
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
   group[1]=GetMCMF(g,sol->servers);//���ڷ��򻡱����ӣ����¼���mcmf���ܵ��¸���Ľ��
   if (group[1]->cost < sol->cost) sol=group[1];
   return sol;
}
#endif
