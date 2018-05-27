#include<stdlib.h>
#include<stdio.h>
#include<set>
#include<time.h>
using namespace std;
int main(){
  freopen("in300.txt","w",stdout);
  int nNode=300;
  int nServCost=2000;
  //===
  int nEdges=nNode*4;
  int nCon=nNode/2;
  printf("%d %d %d\n\n%d\n\n",nNode,nEdges,nCon,nServCost);
  //
  int avgFlow=30;
  //edge
  srand(time(0));
  set<pair<int,int> > edge;  //边重复
  for (int i=0;i<nEdges;i++){
    int u=rand()%nNode;
    int v=rand()%nNode;while(v==u) v=rand()%nNode;

    while (edge.count(make_pair(u,v))>0 || edge.count(make_pair(v,u))>0){
      u=rand()%nNode;
      v=rand()%nNode;while(v==u) v=rand()%nNode;
    }
    edge.insert(make_pair(u,v));
    int flow=rand()%avgFlow;
    int cost=rand()%(nServCost/10)+1;

    printf("%d %d %d %d\n",u,v,flow,cost);
  }
  printf("\n");
  //consumer
  set<int> conid;  //消费节点重复
  for (int i=0;i<nCon;i++){
    int nNetNode=rand()%nNode;
    while (conid.count(nNetNode))nNetNode=rand()%nNode;
    conid.insert(nNetNode);
    int nNeed=rand()%(avgFlow*2);
    printf("%d %d %d\n",i,nNetNode,nNeed);
  }
  return 0;
}
