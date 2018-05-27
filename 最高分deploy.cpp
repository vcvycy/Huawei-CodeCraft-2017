//#define DEBUG
#include "deploy.h"
#include "myfun.h"
#include <stdio.h>

//你要完成的功能总入口 

void deploy_server(char * topo[MAX_EDGE_NUM], int line_num,char * filename)
{

	// 需要输出的内容
  srand(time(NULL));
   GRAPH *g=new GRAPH();
   g->ReadGraph(topo);
   TABU *tabu=new TABU();
   time_t tstart=time(NULL);
   set<int> servers;
   initServers(g,servers);

   SOLUTION *sol=tabu->Work(g,200,30,0,&servers);
//printf("ans=%d\n",sol->cost);

  static char out_buff[2001000];
   char *pbuff=out_buff;
   char buff[1000];
   sprintf(buff,"%d\n\n",sol->path.size()); pbuff=write(buff,pbuff);
   int totCost=sol->servers.size()*g->serCost;
   for (int i=0;i<sol->path.size();i++){
     PATH *p=&(sol->path[i]);
     for (int j=0;j<p->node.size();j++){sprintf(buff,"%d ",p->node[j]);pbuff=write(buff,pbuff);}
     sprintf(buff,"%d ",g->netToCon[p->node[p->node.size()-1]]);pbuff=write(buff,pbuff);
     sprintf(buff,"%d\n",p->flow);pbuff=write(buff,pbuff);
     totCost+=p->cost;
   }
   write_result(out_buff,filename);
   /**/
   #ifdef DEBUG	
   printf("Cost=%d\n",totCost);
   #endif
}
