#include<stdio.h>
int main(){
  int a,b;
  char *s=NULL;
  int k=sscanf(s,"%d%d",&a,&b);
  printf("k=%d\n",k);
  return 0;
}
