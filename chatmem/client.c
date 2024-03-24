#include "chat.h"
 
lp_charmsg vecmsg;

char nick[10]; 
int pid;

byte lastread=0;

void logout ()
{
  int shmid,sem;
  byte pos;
  s_msg msg;
  lp_msg vec_m;

  strcpy (msg.msg,nick);
  msg.type=OUT;
  msg.id=pid;
  msg.id_to=0;
  
  shmid=shmget(KEY,257*sizeof(s_msg),(0666));
  vec_m=(lp_msg)shmat(shmid,NULL,0);
  sem=semget(KEY,1,(0666));
  
  DOWN (sem,0);
  pos=(byte)vec_m[256].type;
  vec_m[256].type++;
  vec_m[pos]=msg;
  UP (sem,0);

 
  printf("By by \n");
  exit (1);
} 

void login()
{
  int shmid,sem;
  byte pos;
  s_msg msg;
  lp_msg vec_m;

  strcpy (msg.msg,nick);
  msg.type=IN;
  msg.id=pid;
  msg.id_to=0;
  
  shmid=shmget(KEY,257*sizeof(s_msg),(0666));
  vec_m=(lp_msg)shmat(shmid,NULL,0);
  sem=semget(KEY,1,(0666));
  
  DOWN (sem,0);
  pos=(byte)vec_m[256].type;
  vec_m[256].type++;
  vec_m[pos]=msg;
  UP (sem,0);
}

envia (char* linha)
{
  int shmid,sem;
  byte pos;
  s_msg msg;
  lp_msg vec_m;

  strcpy (msg.msg,linha);
  msg.type=CHAT;
  msg.id=pid;
  msg.id_to=0;
  
  shmid=shmget(KEY,257*sizeof(s_msg),(0666));
  vec_m=(lp_msg)shmat(shmid,NULL,0);
  sem=semget(KEY,1,(0666));
  
  DOWN (sem,0);
  pos=(byte)vec_m[256].type;
  vec_m[256].type++;
  vec_m[pos]=msg;
  UP (sem,0);
}

void handler(int s)
{
   /* Verificar mensagens vindas do servidor */
  //printf("l:rui \n");
  
  if (lastread!=vecmsg[256].msg[0])
    {
      printf("%s\n",vecmsg[lastread++].msg);
      
    }
  alarm(1);   /* N = No de segundos */
}


main ()
{
  //  char *linha;
  char prompt[60];
  char *linha;
  int semin;
  int shmind;
  linha=calloc(MAXCHAR,sizeof(char)); 
  printf ("\n(c) X-prog 2001 (Trabalho de S.O.) Chat - Cliente/Servidor\n\n");
  printf ("nick ? :");
  scanf ("%s",nick);
  pid=getpid();  
  
  shmind=shmget(pid,257*sizeof(s_charmsg),(IPC_CREAT|0666));
  /* a posição vecmsg[256][0] tem o index para se 
     saber onde escrever AS MSG'S */
  vecmsg=(lp_charmsg)shmat(shmind,NULL,0);
  semin=semget(KEY,1,(IPC_CREAT|0666));
  vecmsg[256].msg[0]=0;
  INIT (semin,0);
  
  login();
  signal (SIGTSTP,logout); 
  signal (SIGINT,logout); 
   for (;;) {
     /* Ignora linhas vazias */
     sprintf(prompt,"<%s:>",nick); 

     DOWN(semin,0);
     signal(SIGALRM, handler);
     UP(semin,0);
     
     raise(SIGALRM);
     
     while (strlen(linha = readline(prompt)) == 0)
       {
         free(linha);         /* Obrigatótio */
       }
     add_history(linha);
     
     envia (linha);
     
     
     free(linha);            /* Obrigatório */
   } 
}
