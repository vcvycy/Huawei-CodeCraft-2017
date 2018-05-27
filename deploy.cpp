
#define DEBUG

#include<windows.h>

//#define NOTDIST
#define NOTMOVE
#define MAX_TIME 90
//#define OPPOSITE_EDGE  直接放弃反向弧
#include "myfun.h"
#include <stdio.h>
//你要完成的功能总入口
int write_result(char *buf,char *filename){
  FILE *f=fopen("result.txt","w");
  fwrite(buf,1,strlen(buf),f);
  fclose(f);
  if (strlen(buf)<200)for (int i=0;i<strlen(buf);i++)putchar(buf[i]);
}
#define MAX_EDGE_NUM 20100
void deploy_server(char * topo[MAX_EDGE_NUM], int line_num,char * filename)
{
   g_time=time(0);
   GRAPH *g=new GRAPH();
   int t1=time(0);
   g->ReadGraph(topo);
   //printf("Read Graph Time :%d\n",time(0)-t1);
   SOLUTION *sol=Genetic(g);
   //以下为输出
   static char out_buff[200000];
   char *pbuff=out_buff;
   char buff[50000];
   sprintf(buff,"%d\n\n",sol->path.size()); pbuff=write(buff,pbuff);
   for (int i=0;i<sol->path.size();i++){
     PATH *p=&(sol->path[i]);
     for (int j=0;j<p->node.size();j++){sprintf(buff,"%d ",p->node[j]);pbuff=write(buff,pbuff);}
     sprintf(buff,"%d ",g->netToCon[p->node[p->node.size()-1]]);pbuff=write(buff,pbuff);
     sprintf(buff,"%d\n",p->flow);pbuff=write(buff,pbuff);
   }
   write_result(out_buff,filename);
   /**/
   #ifdef DEBUG
   printf("********************************************\n"
          "最终结果:totalFlow:%d\ncost:%d\n",g->tolFlow,sol->cost);
   #endif
}
char *topo[MAX_EDGE_NUM];
int main(int argc,char**argv){
  int line_num=0;
  if (freopen("case0.txt","r",stdin)==NULL)exit(-1);
  while (1){
    topo[line_num]=new char[1000];
    if (gets(topo[line_num])==NULL)break;
    line_num++;
  }
  int ts=GetTickCount();
  printf("line_num=%d\n",line_num);
  deploy_server(topo,line_num,NULL);
  int te=GetTickCount();
  printf("time:%d ms\n",te-ts);
  return 0;
}
