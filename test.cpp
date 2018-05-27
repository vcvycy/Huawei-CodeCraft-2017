#include<stdio.h>
#include<stdlib.h>
#include<vector>
#include<set>
using namespace std;
void verify(char* filename){
  FILE *f=fopen(filename,"r");
  int r;
  while (fscanf(f,"%d",&r)!=EOF){
    printf("%d\n",r);
  }
  fclose(f);
}
int main(int argc,char **argv){
    printf("%c",0x2d);
  return 0;
}
