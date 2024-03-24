#include "chat.h"
#include <stdio.h>

#define maxutil 100  // nº max de utilizadores 

typedef struct util_id{
  int util_id;
  char nick[10];
}lp_util,s_util;

s_util util[maxutil]; // lista dos processos de cada utilizador

lp_msg vecmsg; // lista de msg


int por_util(int idproc,byte *nick)  // 0 se foi posto -1 se não 
{
  int i;
  for (i=0;i<maxutil;i++)
    if(util[i].util_id==-1)
      {
	printf("%d(%s) LOGED IN\n",idproc,nick);
	util[i].util_id=idproc;
	strcpy(util[i].nick,nick);
	return 0;
      }
  return -1;

}

int tirar_util(int idproc) // 0 se havia -1 se não 
{
  int i;
  for (i=0;i<maxutil;i++)
    if(util[i].util_id==idproc)
      {
	printf("%d(%s) LOGED OUT\n",idproc,util[i].nick);
	util[i].util_id=-1;
      }
  if (i==maxutil)
    return -1;
  else 
    return 0;
}

char* get_nick(int id)
{
  int i;
  for (i=0;i<maxutil;i++)
    if(util[i].util_id==id)
      return util[i].nick;
  return NULL;
}

void escrever_para_util (int  index) //escrever para os utilizadores
{
  int i,shmid;  // shmid poiter de memoria virtual  
  int sem;        // semaforo
  lp_charmsg aux;   
  byte x;
  char m[MAXCHAR],m2[MAXCHAR];
  strcpy(m2,vecmsg[index].msg);
  sprintf(m,"\n<%s>%s",get_nick(vecmsg[index].id),vecmsg[index].msg);
  strcpy(vecmsg[index].msg,m);
  for(i=0;i<maxutil;i++) // para cada elemnto da lista de utiliadores 
    {
      if (util[i].util_id!=-1)  // -1 elemento da lista vasio
	{ 
	 printf(" %d(%s) WRITING \"%s\" TO %d(%s)\n",vecmsg[index].id,get_nick(vecmsg[index].id),m2,util[i].util_id,get_nick(util[i].util_id));
	 // escrver no utilizador
	 shmid=shmget(util[i].util_id,257*sizeof(s_charmsg),IPC_CREAT|0666);
	 aux=(lp_charmsg)shmat(shmid,NULL,0);
	 sem=semget(KEY,1,(0666));
	 DOWN(sem,0);
	 x=aux[256].msg[0];
	 memcpy(aux[x].msg,vecmsg[index].msg,sizeof(byte)*MAXCHAR);
	 aux[256].msg[0]++;
	 UP(sem,0);
	 shmdt(aux);
	}
    }
}

void mandar_lista (int index)
{
  char lista[MAXCHAR];
  s_msg m;
  int i,shmid,sem;
  lp_charmsg aux;
  byte x;
  strcpy(lista,"\n<lista de nicks>");
  for(i=0;i<maxutil;i++)
    {
      if(util[i].util_id!=-1)
	sprintf(lista,"%s\n%s",lista,util[i].nick);
    }
  strcpy(m.msg,lista);
  m.type=LIST;
  m.id=KEY;
  m.id_to=0;
  // escrver no utilizador
  shmid=shmget(vecmsg[index].id,257*sizeof(s_charmsg),IPC_CREAT|0666);
  aux=(lp_charmsg)shmat(shmid,NULL,0);
  sem=semget(KEY,1,(0666));
  DOWN(sem,0);
  x=aux[256].msg[0];
  memcpy(aux[x].msg,m.msg,sizeof(byte)*MAXCHAR);
  aux[256].msg[0]++;
  UP(sem,0);
  shmdt(aux);
}

int ler (int index)
{
  if (vecmsg[index].type==IN)
    if(por_util(vecmsg[index].id,vecmsg[index].msg)==0)
      mandar_lista(index);
    else
      printf("ERROR exedeu o nº max de utilizadores ...");
    
  if (vecmsg[index].type==OUT)
    tirar_util(vecmsg[index].id);
  
  if (vecmsg[index].type==CHAT)
    escrever_para_util(index);
}

main()
{
  
  int shmid;
  byte iler=0,iescrever=0;
  int i;
  int sem;
  printf ("\n(c) X-prog 2001 (Trabalho de S.O.) Chat - Cliente/Servidor\n\n");
  shmid=shmget(KEY,257*sizeof(s_msg),(IPC_CREAT|0666));
  /* a posição vecmsg[256].type tem o index para se 
     saber onde escrever AS MSG'S */
  vecmsg=(lp_msg)shmat(shmid,NULL,0);
  sem=semget(KEY,1,(IPC_CREAT|0666));
  vecmsg[256].type=0;
  INIT (sem,0);
  for(i=0;i<maxutil;i++)
    util[i].util_id=-1;
  
  for (;;)
    {
      DOWN (sem,0);
      iescrever=(byte)vecmsg[256].type;
      if (iler!=iescrever)
	{
	  ler(iler);
	  iler++;
	  printf("");
	}
      UP (sem,0);
      usleep(10000);
    }
  
  
  shmdt(vecmsg);
  return 0;
}

