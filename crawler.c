#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <math.h>
#define HASH_SIZE 100
#define URL_LENGTH 500
#define SEED_URL "www.chitkara.edu.in"
#define MAX_URL_PER_PAGE 50
#define INTERVAL_TIME 10
#define CHECK_MALLOC(s) if ((s) == NULL) { \
printf("memory allocation failed in %s: line%d", __FILE__, __LINE__); \
perror(":"); \
exit(-1); \
}
#define SET_MEM(start,size) memset(start,0,size)
int F = 0;
struct url{
  char link[URL_LENGTH];
  int key,depth,isVisited;
};

struct node{
   struct url urlobj;
  struct node *next;
};

struct hash{
  struct node *head,*tail;
  int count;
};

struct node *head = NULL;
void multipleextraction(char*,int,struct hash obj[]);
void testDir(char *dir)
{
  struct stat statbuf;
  if ( stat(dir, &statbuf) == -1 )
  {
    fprintf(stderr, "-----------------\n");
    fprintf(stderr, "Invalid directory\n");
    fprintf(stderr, "-----------------\n");
    exit(1);
  }

  //Both check if there's a directory and if it's writable
  if ( !S_ISDIR(statbuf.st_mode) )
  {
    fprintf(stderr, "-----------------------------------------------------\n");
    fprintf(stderr, "Invalid directory entry. Your input isn't a directory\n");
    fprintf(stderr, "-----------------------------------------------------\n");
    exit(1);
  }

  if ( (statbuf.st_mode & S_IWUSR) != S_IWUSR )
  {
    fprintf(stderr, "------------------------------------------\n");
    fprintf(stderr, "Invalid directory entry. It isn't writable\n");
    fprintf(stderr, "------------------------------------------\n");
    exit(1);
  }
}

void testUrl(char *url){
 char str[100] = "wget --spider ";
 strcat(str,url);
 if(!system(str)){//"wget --spider http://chitkara.edu.in"
    printf("URL : OK\n");
 }
 else{
    fprintf(stderr, "-------------------------------------\n");
    fprintf(stderr, "Invalid URL. Please enter correct URL\n");
    fprintf(stderr, "-------------------------------------\n");
 }
}

int stringtonumber(char *maxdepth){
  int c = 0,r = 0,i = 0;
  while(maxdepth[i] != '\0'){
     c = maxdepth[i] - 48;
     r = (r * 10) + c;
     i++;
  }
  return r;
}

void numbertostring(char ch[],int num){
  int c,n,d = 0;
  n = num;
  while(n > 0){
    d++;
    n = n / 10;
  }
  ch[d] = '\0';
  d = d - 1;
  while(num > 0){
    c = (num % 10) + 48;
    ch[d] = c;
    d--;
    num = num / 10;
  }
}

void testDepth(int depth){

  if((depth > 0)&&(depth <= 2)){
     printf("DEPTH : OK\n");
  }
  else{
     fprintf(stderr, "-------------------------------------\n");
     fprintf(stderr, "Invalid DEPTH. Please enter DEPTH between 1 and 2\n");
     fprintf(stderr, "-------------------------------------\n");
  }
}

void makefile(char baseurl[80],char dirpath[100],char tempfile[80],int dep){
  char file[80],str[200],ch[10];
  strcpy(file,"file");
  F++;
  numbertostring(ch,F);
  strcat(file,ch);
  strcat(file,".txt");
  strcat(dirpath,file);
  FILE *fp1,*fp2;
  fp1 = fopen(dirpath,"w");
  fp2 = fopen(tempfile,"r");
  if(fp1 == NULL){
    printf("dirpath doesn't exist\n");
  }
  if(fp2 == NULL){
    printf("tempfile doesn't exist\n");
  }
  strcat(baseurl,"\n");
  fputs(baseurl,fp1);
  fprintf(fp1,"%d\n",dep);

  while(!feof(fp2)){
   fgets(str,200,fp2);
   fputs(str,fp1);
  }
  fclose(fp1);
  fclose(fp2);

}

void getpage(char *url,char *dir,int dep)
{

 char dirpath[100],tempfile[80],extract[100];
 char urlbuffer[URL_LENGTH+300]={0};
 dirpath[0]='\0';
 tempfile[0]='\0';
 extract[0]='\0';
 strcat(dirpath,dir);
 strcat(dirpath,"/");
 strcat(tempfile,"tempfiles/temp.txt");
 strcat(urlbuffer, "wget -O ");
 strcat(urlbuffer,tempfile);
 strcat(urlbuffer," ");
 strcat(urlbuffer,url);
 system(urlbuffer);

 makefile(url,dirpath,tempfile,dep);
}
int dupcheck(struct node *head,char str2d[]){
   struct node *ptr = head;
   int c = 1;
   while(ptr != NULL){
     c = strcmp(ptr->urlobj.link,str2d);
      if(c == 0){
        return c;
      }
      ptr = ptr -> next;
   }
   return c;
}
struct node *linkedlist(char str2d[],struct hash obj[],int key,int dep){
  struct node *n,*ptr,*ptr2;
  int c = 1;
  c = dupcheck(head,str2d);
  if(c == 0){
    return head;
  }
  n = (struct node *)malloc(sizeof(struct node));
  strcpy(n->urlobj.link,str2d);
  n->urlobj.key = key;
  n->urlobj.depth = dep;
  if(obj[key].head == NULL){

    if(head == NULL){
    n -> next = NULL;
    head = n;
    }
    else{
    ptr = head;
    while(ptr -> next != NULL){
      ptr = ptr -> next;
    }
    n -> next = NULL;
    ptr -> next = n;
    }

    obj[key].head = n;
    obj[key].tail = n;
    obj[key].count = obj[key].count + 1;
  }
  else{

    ptr2 = obj[key].tail;
    n -> next = ptr2 -> next;
    ptr2 -> next = n;
    obj[key].count = obj[key].count + 1;
    obj[key].tail = n;
  }

  return head;
}

void hashing(char str2d[MAX_URL_PER_PAGE][URL_LENGTH],int n,char *dir,int maxdepth,int dep){
  struct hash obj[HASH_SIZE];

  int c = 0,i,sum = 0,j;
  double d;
  for(i = 0; i < HASH_SIZE; i++){
    obj[i].head = NULL;
    obj[i].tail = NULL;
    obj[i].count = 0;
  }
  for(i = 0; i < n; i++){
     j = 0;
     sum = 0;
     while(str2d[i][j] != '\0'){
       sum = sum + str2d[i][j];
       j++;
     }
     d = sum*((sqrt(5)-1)/2);
     c = HASH_SIZE*fmod(d,1);
     head = linkedlist(str2d[i],obj,c,dep);
  }


  multipleextraction(dir,maxdepth,obj);

}

void urlextract(char *dir,int maxdepth,int dep){
 char command[200] = "cat ";
 char ch[10];
 strcat(command,dir);
 char filename[50] = "/file";
 numbertostring(ch,F);
 strcat(filename,ch);
 strcat(filename,".txt");
 strcat(command,filename);
 char temp[100] = " | grep -Eo \"(http|https)://";
 char temp2[100] = "[a-zA-Z0-9./?=_-]*\" | sort -u > tempfiles/linkextract.txt";
 strcat(temp,SEED_URL);
 strcat(temp,temp2);
 strcat(command,temp);
 system(command);
 FILE *fp;
 int count = 0,i = 0,check = 0,j;
 char str[URL_LENGTH+100];
 char str2d[MAX_URL_PER_PAGE][URL_LENGTH];
 str[0] = '\0';
 str2d[0][0] = '\0';
 fp = fopen("tempfiles/linkextract.txt","r");
 if(fp == NULL){
   printf("Link file not found\n");
 }
 while(fgets(str,URL_LENGTH+100,fp) != NULL){
   if(count >= MAX_URL_PER_PAGE){
      break;
   }

   count = count + 1;
   if(strlen(str) <= URL_LENGTH){
      strcpy(str2d[i],str);
      i++;
   }
 }
 fclose(fp);

 hashing(str2d,i,dir,maxdepth,dep);
}

void multipleextraction(char *dir,int maxdepth,struct hash obj[HASH_SIZE]){
  struct node *ptr;
  //int i;
  ptr = head;
  while(ptr != NULL){
    if(ptr->urlobj.depth >= maxdepth){
       return;
    }
    if(ptr->urlobj.isVisited == 0){
    ptr->urlobj.isVisited = 1;
    getpage(ptr->urlobj.link,dir,ptr->urlobj.depth+1);
    urlextract(dir,maxdepth,ptr->urlobj.depth+1);
    }
    ptr = ptr -> next;
  }
  /*for(i = 0; i < HASH_SIZE; i++){
    if(obj[i].head == NULL){
      printf("%d   Head is null\n",i);
    }
    else{
    printf("%d  count  = %d    head =  %s      tail =  %s\n",i,obj[i].count,obj[i].head->urlobj.link,obj[i].tail->urlobj.link);
    }
  }*/
}

int main(int argc,char *argv[]){
    char url[80],dir[80];
    int dep = 0,maxdepth;
    struct node *ptr;
    char folder[50] = "cd ";
    char tempfilecreate[50] = "cd ";

    maxdepth = stringtonumber(argv[2]);
    strcpy(url,SEED_URL);
    strcpy(dir,argv[1]);
    strcat(folder,dir);
    strcat(tempfilecreate,dir);
    strcat(tempfilecreate,"; mkdir tempfiles");
    system(tempfilecreate);
    strcat(folder,"; mkdir crawlerdata");
    system(folder);
    strcat(dir,"/crawlerdata");
    testUrl(url);
    testDir(dir);
    testDepth(maxdepth);
    getpage(url,dir,dep);
    urlextract(dir,maxdepth,dep);
    ptr = head;
    int i = 1;
    while(ptr !=NULL){
      printf("%d  %s\n",i,ptr->urlobj.link);
      i++;
      ptr = ptr -> next;
    }
    return 0;
}
