#include <stdio.h>
	#include <sys/types.h>
	#include <unistd.h>
	#include <stdlib.h>
	#include <errno.h>
	#include <string.h>
	#include <ctype.h>
	#include <grp.h>
	
	#define NUMGIDS 1024
	
	int main(int argc,char* argv[])
	{
	  char* Res;
	  char Buffy[4096];
	  uid_t tuid=-1,uid;
	  gid_t gid;
	  int i;
	  FILE* File;
	  char* command=NULL;
	  char* shell;
	  char* HOME;
	  gid_t gid_list[NUMGIDS];
	  ssize_t NumGids=0;
	
	  if (argc>1)
	  {
	    if (strcmp(argv[1],"--help")==0)
	      { fprintf(stdout,"There is no help!\n"); exit(0);}
	    if (strcmp(argv[1],"--version")==0) {fprintf(stdout,"0.6\n"); exit(0); }
	  }
	
	  if ((argc==4) && (strcmp(argv[2],"-c")==0)) command=argv[3]; else
	  if (argc!=2)
	  {
	    fprintf(stdout,"USAGE: su username|uid [-c command]\n");
	    return 1;
	  };
	
	  i=0;
	  while(isdigit(argv[1][i])) ++i;
	  if (argv[1][i]==0) tuid=atol(argv[1]);
	
	  File=fopen("/etc/passwd","rb");
	  if (File==NULL) {perror("/etc/passwd"); return 1;};
	 
	  while(1)
	  {
	    errno=0;
	    Res=fgets(Buffy,1024,File);
	    if (Res==NULL) {
	      if (errno!=0) perror("/etc/passwd");
	        else fprintf(stderr,"su: User not found!\n");
	      return 1;
	    };
	   
	    Res=strtok(Buffy,":");
	    if (Res==NULL) continue;
	    strtok(NULL,":");
	    uid=atol(strtok(NULL,":"));
	    gid=atol(strtok(NULL,":"));
	    strtok(NULL,":");
	    HOME=strtok(NULL,":");
	    shell=strtok(NULL,":");
	    if (tuid==uid) {argv[1]=strdup(Buffy); break;}
	    if (strcmp(argv[1],Buffy)==0) break;
	  };
	  HOME=strdup(HOME);
	  shell=strdup(shell);
	 
	  File=fopen("/etc/group","rb");
	  if (File==NULL) {perror("/etc/group"); return 1;};
	 
	  while(1)
	  {
	ContinueReadingEtcGroup:
	    errno=0;
	    Res=fgets(Buffy,1024,File);
	    if (Res==NULL) {
	      if (errno!=0) {perror("/etc/group"); return 1;} else break;
	    };
	   
	    Res=strtok(Buffy,":,\n");
	    if (Res==NULL) continue;
	    strtok(NULL,":,\n");
	    gid_list[NumGids]=atol(strtok(NULL,":,\n"));
	    Res=strtok(NULL,":,\n");
	
	    while(Res!=NULL)
	    {
	      if (strcmp(Res,argv[1])==0)
	      {
	        ++NumGids;
	        if (NumGids>=NUMGIDS) goto SetNewIdentity;
	                         else goto ContinueReadingEtcGroup;
	      };
	      Res=strtok(NULL,":,\n");
	    };
	  };
	
	SetNewIdentity:
	  if (command==NULL) command=shell;
	  setenv("HOME",HOME,1);
	  setgroups(NumGids,gid_list);
	  setgid(gid);
	  setuid(uid);
	  errno=0;
	  i=system(command);
	  if (((i<0) || (i==127)) && (errno!=0)) {perror("/bin/sh"); return 1;};
	  return i;
	};
