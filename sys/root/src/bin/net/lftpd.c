/*
The contents of this file are subject to the Mozilla Public License Version 1.1 
(the "License"); you may not use this file except in compliance with the License.
You may obtain a copy of the License at http://www.mozilla.org/MPL/

Software distributed under the License is distributed on an "AS IS" basis, 
WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License for the 
specific language governing rights and limitations under the License.

The Original Code is Lepton.

The Initial Developer of the Original Code is Philippe Le Boulanger.
Portions created by Philippe Le Boulanger are Copyright (C) 2011 <lepton.phlb@gmail.com>.
All Rights Reserved.

Contributor(s): Jean-Jacques Pitrolle <lepton.jjp@gmail.com>.

Alternatively, the contents of this file may be used under the terms of the eCos GPL license 
(the  [eCos GPL] License), in which case the provisions of [eCos GPL] License are applicable 
instead of those above. If you wish to allow use of your version of this file only under the
terms of the [eCos GPL] License and not to allow others to use your version of this file under 
the MPL, indicate your decision by deleting  the provisions above and replace 
them with the notice and other provisions required by the [eCos GPL] License. 
If you do not delete the provisions above, a recipient may use your version of this file under 
either the MPL or the [eCos GPL] License."
*/


/*===========================================
Includes
=============================================*/
#include <stdlib.h>
#include "kernel/signal.h"
#include "kernel/libstd.h"
#include "kernel/devio.h"
#include "kernel/stat.h"
#include "kernel/statvfs.h"
#include "kernel/dirent.h"
#include "kernel/time.h"

#include "kernel/net/socket.h"

#include "stdio/stdio.h"

/*===========================================
Global Declaration
=============================================*/
#define CR	'\r'
#define ftp_port	2002//20001
#define data_port	0
#define ncar	80
#define maxdata	512
#define portmax	3000

/* type de transfert */
#define type_ascii	0
#define type_image	1

#define c_quit	   0
#define c_port	   1
#define c_type	   2
#define c_store	3
#define c_retr	   4
#define c_user	   5
#define c_pass	   6
#define c_syst	   7
#define c_send	   8
#define c_cwd	   9
#define c_pwd	   10
#define c_list	   11
#define c_noop	   12
#define c_opts	   13
#define c_error	-1

/* definition des commandes */
const char cmd_quit[]    = "QUIT";
const char cmd_user[]    = "USER";
const char cmd_type[]    = "TYPE";
const char cmd_pass[]    = "PASS";
const char cmd_port[]    = "PORT";
const char cmd_syst[]    = "syst";
const char cmd_store[]   = "STOR";
const char cmd_retr[]    = "RETR";
const char cmd_send[]    = "SEND";
const char cmd_cwd[]     = "CWD";
const char cmd_pwd[]     = "PWD";
const char cmd_list[]    = "LIST";
const char cmd_noop[]    = "NOOP";
const char cmd_opts[]    = "opts";

/* messages ftp */
const char banniere[]   ="    lftpd lepton ftp daemon server 2004.\r\n";
const char passwd[]     ="    Password required ";
const char user[]       ="    User logged in.\r\n";
const char portok[]     ="    Identification port ok\r\n";
const char conopen[]    ="    connexion donnees etablie\r\n";
const char complet[]    ="    transfert termine\r\n";
const char typei[]      ="    Type set to I.\r\n";
const char typea[]      ="    Type set to A.\r\n";
const char syst[]       ="    UNIX type: L8\r\n";
const char errparam[]   ="    Parametre non reconnu\r\n";
const char errftp[]     ="    Commande inconnue\r\n";
const char abortcon[]   ="    Transfert interrompu, connexion rompue.\r\n";

int fd;			/* descripteur du fichier a creer */
int rwmode=0644;	/* droits du fichier a creer */
int cmdsock;		/* descripteur du socket initial */
int datasock;		/* descripteur du socket de connexion data */
int new_cmdsock; /* descripteur du socket de connexion de commande */
int cli_port;		/* numero de port data client */
int ind_type;		/* indicateur de type de transfert */

/* structure serveur associee au socket initial */
struct sockaddr_in st_serv;
int servlen= sizeof(st_serv);

/* structure client associee a la connexion de commande */
struct sockaddr_in st_cli;
int clilen= sizeof(st_cli);

/* structure serveur associee a la connexion data */
struct sockaddr_in st_data;
int stdatalen= sizeof(st_data);

/* structure client associee a la connexion data */
struct sockaddr_in cli_data;
int clidatalen= sizeof(cli_data);

char message[80];	/* buffer de reception des commandes ftp */
char bufcmd[20],param[50];	/* buffers de stockage */
char ipaddr[16],hport[5],lport[5];	/* buffers de stockage */
int msglen;		/* longueur des messages */

/* macro de mise a zero d'une zone memoire */
//unix version
//#define clear(x) bzero(x,sizeof(x)) 

//WIN32 version
#define clear(x) memset(x,0,sizeof(x))

/* macro de comparaison de chaine */
#define cmpstr(x) (__cmpstr(bufcmd,x,strlen(x)) == 0)

char ftp_root[]="/";


/*===========================================
Implementation
=============================================*/
int __cmpstr(const char* buf1,const char* buf2,int len){
   int i;
   for(i=0;i<len;i++){
      char c1 = (buf1[i]>='A' && buf1[i]<='Z') ? buf1[i]+32 : buf1[i];
      char c2 = (buf2[i]>='A' && buf2[i]<='Z') ? buf2[i]+32 : buf2[i];
      if(c1!=c2)
         return -1;
   }

   return 0;
}


/*-------------------------------------------
| Name:send_ftpmsg
| Description:routine d'envoi de message ftp au client
| Parameters:
| Return Type:
| Comments:la chaine doit commencer par 4 espaces
| See:RFC. le code doit obeir aux specifications FTP
---------------------------------------------*/
void send_ftpmsg(int sk,int code,const char *ch){
	char wks[80];
   int len;
   int cb=0;
	
	clear(wks);
	strncpy(wks,ch,strlen(ch));	/* recopie le message */
	sprintf(wks,"%d",code);		/* ajoute le code converti en 3 car */
	wks[3]=' ';			/* plus l'espace intercalaire */
   len=strlen(wks);

   ////printf("ftp message:%s\n",wks);
   //envoi le tout sur le socket 
   while(cb<len){
      int w;
	   if((w=send(sk,wks+cb,len-cb,0))<0)
         return;
      cb+=w;
   }
   
}

/*-------------------------------------------
| Name:p_user
| Description:commande USER recue: demander mot de passe
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void p_user(void) {
	char msg[64];
	strcpy(msg,passwd);
	strcat(msg," for ");
	strcat(msg,param);
	strcat(msg,"\r\n");
	send_ftpmsg(new_cmdsock,331,msg);
}

/*-------------------------------------------
| Name:p_pass
| Description:commande PASS recue: confirmer login
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void p_pass(void) {
   send_ftpmsg(new_cmdsock,230,user);
}


/*-------------------------------------------
| Name:p_syst
| Description:commande SYST recue: confirmer type de systeme
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void p_syst(void) {
	send_ftpmsg(new_cmdsock,215,syst);
}

/*-------------------------------------------
| Name:p_type
| Description:commande TYPE recue: confirmer type de transfert
| Parameters:
| Return Type:
| Comments:tester la valeur du parametre, initialiser ind_type
|          et envoyer confirmation au client ou un code d'erreur
| See:
---------------------------------------------*/
void p_type(void) {
   if(param[0]=='A')
   {
	   ind_type=type_ascii;
	   //printf("%s",typea);
	   send_ftpmsg(new_cmdsock,211,typea);
   }
   else
   {
	   ind_type=type_image;
	   //printf("%s",typei);
	   send_ftpmsg(new_cmdsock,211,typei);
   }
}

/*-------------------------------------------
| Name:p_port
| Description:commande PORT recue: extraire l'adresse IP machine et numero de port
| Parameters:
| Return Type:
| Comments:commande PORT recue: extraire l'adresse IP machine et numero de port
|          le format est fixe: 6 valeurs separes par des virgules
|          4 pour le numero IP et 2 (MSB,LSB) pour le port
| See:
---------------------------------------------*/
void p_port(void) {
   int i,j;
   /* extraction de l'adresse IP */
   clear(ipaddr);
   i=0;
   /* pour les 4 parties du numero IP */
   for (j=0 ; j<4 ; j++) {
	   /* recopie tous les caracteres sauf "," */
	   for (; param[i] != ',' ; i++) ipaddr[i]= param[i];
	   /* met un "." au lieu de la "," pour les 3 premiers numeros */
	   if (j != 3) ipaddr[i++]= '.';
	   }
   //printf("Numero IP client: %s\n",ipaddr);

   /* extraction numero de port data client */
   clear(hport);	/* poids forts */
   j=0;
   /* saute la "," precedente et recopie jusqu'a la suivante */
   for (i++ ; param[i] != ',' ; i++) hport[j++]= param[i];

   clear(lport);	/* poids faibles */
   j=0;
   /* saute la "," precedente et recopie jusqu'a la fin de la chaine */
   for (i++ ; i<(int)strlen(param) ; i++) lport[j++]= param[i];

   /* construit le numero de port = 256*hport + lport */
   cli_port= atoi(lport)+(256*atoi(hport));

   //printf("Numero de port data client: %d\n",cli_port);

   /* init structure client pour la connexion data */
   //bzero((char *)&cli_data,clidatalen);
   memset((char *)&cli_data,0,clidatalen);

   cli_data.sin_family=AF_INET;
   cli_data.sin_port=htons(cli_port);
   cli_data.sin_addr.s_addr=inet_addr(ipaddr);
   send_ftpmsg(new_cmdsock,200,portok);
}

/*-------------------------------------------
| Name:p_store
| Description:commande STOR recue: etablir la connexion de donnees pour le fichier
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int p_store(void) {
   int fd;
   int cb=0;
   char data[maxdata];		/* buffer de reception des donnees */

   /* init structure serveur pour connexion data */
   memset((char *)&st_data,0,stdatalen); 

   st_data.sin_family=AF_INET;
   st_data.sin_port=htons(data_port);
   st_data.sin_addr.s_addr=INADDR_ANY;

   /* creation du socket pour la connexion data */

   datasock=socket(PF_INET,SOCK_STREAM,0);

   /* attachement du socket sur la connexion data cote serveur  */

   if(bind(datasock,(struct sockaddr*)&st_data,stdatalen)){
	   //printf("Bind pour le point d'accès local (data) ");
	   return -1;
   }

   /* etablissement de la connexion data avec le client specifie */
   //printf("tentative de connexion sur le port %d\n",cli_port);

   if(connect(datasock,(struct sockaddr*)&cli_data,clidatalen)){
	   //printf("Connection avec le client (data) ");
	   return -1;
   }

   //printf("connexion data effectuee sur le port %d\n",cli_data.sin_port);

	send_ftpmsg(new_cmdsock,150,conopen);

   /* lecture donnees pour transfert et stockage fichier */
   if((fd = open(param,O_CREAT|O_RDWR,0))<0){
      //printf("Erreur creation fichier");
	   send_ftpmsg(new_cmdsock,426,abortcon);
	   shutdown(datasock,0);
   }
   
   if ((fd<0) && (fd= open(param,O_WRONLY,0))<0) {
	   //printf("Erreur lecture fichier");
	   send_ftpmsg(new_cmdsock,426,abortcon);
	   shutdown(datasock,0);
	   return -1;
	}

   //printf("Fichier local %s ouvert\n",param);
   //printf("lecture du fichier distant %s\n",param);
   //printf("Attente\n");

   msglen=0;
   while(msglen==0)
	   msglen=recv(datasock,data,maxdata,0);

   //printf("Debut du transfert\n");

   while(msglen>0){
      cb+=msglen;
      //data[msglen]=0x00;
      ////printf("%s",data);
	   write(fd,data,msglen);
	   msglen=recv(datasock,data,maxdata,0);
   }

   close(fd);

   //shutdown with how=SD_SEND. 
   shutdown(datasock,0);

   //printf("\ntransfert termine: %d byte\n",cb);
   send_ftpmsg(new_cmdsock,250,complet);

   return 0;
}

/*-------------------------------------------
| Name:p_retr
| Description:commande RETR recue: etablir la connexion de donnees pour le fichier
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int p_retr(void) {
   int cb=0;
   int fd;
   char data[maxdata];		/* buffer de reception des donnees */

   /* init structure serveur pour connexion data */
   memset((char *)&st_data,0,stdatalen); 

   st_data.sin_family=AF_INET;
   st_data.sin_port=htons(data_port);
   st_data.sin_addr.s_addr=INADDR_ANY;

   /* creation du socket pour la connexion data */

   datasock=socket(PF_INET,SOCK_STREAM,0);

   /* attachement du socket sur la connexion data cote serveur  */

   if(bind(datasock,(struct sockaddr*)&st_data,stdatalen))
   {
	   //printf("Bind pour le point d'accès local (data) ");
	   return -1;
   }

   /* etablissement de la connexion data avec le client specifie */
   //printf("tentative de connexion sur le port %d\n",cli_port);

   if(connect(datasock,(struct sockaddr*)&cli_data,clidatalen))
   {
	   //printf("Connection avec le client (data) ");
	   return -1;
   }

   //printf("connexion data effectuee sur le port %d\n",cli_data.sin_port);
	 send_ftpmsg(new_cmdsock,150,conopen);

   /* lecture donnees pour transfert et stockage fichier */

   if ((fd= open(param,O_RDONLY,0))<0) {
	   //printf("Erreur lecture fichier");
	   send_ftpmsg(new_cmdsock,426,abortcon);
	   shutdown(datasock,0);
	   return -1;
	}

   //printf("Fichier local %s ouvert\n",param);
   //printf("lecture du fichier local %s\n",param);
   //printf("Attente\n");
  
   //printf("Debut du transfert\n");
   
   msglen=read(fd,data,maxdata-1);
   while(msglen>0){
	   send(datasock,data,msglen,0);
      msglen=read(fd,data,maxdata-1);
      cb+=msglen;
      data[msglen]=0x00;
      ////printf("%s",data);
   }
  
   close(fd);

    //shutdown with how=SD_SEND. 
   shutdown(datasock,0);
   
   //printf("\ntransfert termine: %d byte\n",cb);

   send_ftpmsg(new_cmdsock,250,complet);

   return 0;
}

/*-------------------------------------------
| Name:p_list
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void p_list(void) {

   DIR* dir;
   struct dirent dirent;
   struct stat _stat;
   char path[PATH_MAX];
   int l;

   int len;
   int cb=0;
   char buffer[64]={0};
   
	
   /* init structure serveur pour connexion data */
   memset((char *)&st_data,0,stdatalen); 

   st_data.sin_family=AF_INET;
   st_data.sin_port=htons(data_port);
   st_data.sin_addr.s_addr=INADDR_ANY;

   /* creation du socket pour la connexion data */

   datasock=socket(PF_INET,SOCK_STREAM,0);

   /* attachement du socket sur la connexion data cote serveur  */

   if(bind(datasock,(struct sockaddr*)&st_data,stdatalen)){
	   //printf("Bind pour le point d'acces local (data) ");
	   exit(0);
   }

   /* etablissement de la connexion data avec le client specifie */
   //printf("tentative de connexion sur le port %d\n",cli_port);

   if(connect(datasock,(struct sockaddr*)&cli_data,clidatalen)){
	   //printf("Connection avec le client (data) ");
	   exit(0);
   }

   send_ftpmsg(new_cmdsock,150,conopen);
   //printf("connexion data effectuee sur le port %d\n",cli_data.sin_port);

   
   strcpy(path,"./");
   l = strlen(path);
   
   if(!(dir=opendir(path)))
      return;

   while(readdir2(dir,&dirent)){
      char cbuf[26];

      strcpy(&path[l],dirent.d_name);
      stat(path,&_stat);

      ctime_r(&_stat.st_ctime,cbuf);

      sprintf(buffer,"%crwxrwxrwx 1 us %9d %.12s %s\r\n",
                         S_ISREG(_stat.st_mode) ? '-' : 'd',
                         _stat.st_size,cbuf+4,dirent.d_name );

      /*sprintf(buffer,"%crwxrwxrwx 1 us %9ld Dec 1 00:00 %s\r\n",
                         S_ISREG(_stat.st_mode) ? 'd' : '-',
                         _stat.st_size,dirent.d_name );*/

      len=strlen(buffer);
      cb=0;
      while(cb<len){
         int w;
         if((w=send(datasock,buffer,len-cb,0))<0)
         //if((w=write(datasock,buffer,len-cb))<0)
            break;
         cb+=w;
      }

   }
   closedir(dir);

   //shutdown with how=SD_SEND. 
   shutdown(datasock,0);
   close(datasock);
   
   //while(!recv(datasock,buffer,sizeof(buffer),0) )Sleep(10);
   //closesocket(datasock);
   //printf("connexion data ferme\n");

   send_ftpmsg(new_cmdsock,250,complet);

}

/*-------------------------------------------
| Name:p_pwd
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void p_pwd(void){
   char message[128];
   char buf[PATH_MAX];

   if(!getcwd(buf,PATH_MAX))
      return;
   
   //_getcwd(repertoire,256);
   sprintf(message,"    \"%s\" current directory\r\n",buf);
   //printf("%s",message);
   send_ftpmsg(new_cmdsock,257,message);
}

/*-------------------------------------------
| Name:p_cwd
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void p_cwd(void){
   chdir(param);
   send_ftpmsg(new_cmdsock,250,"    Commande cwd OK\r\n");
}

/*-------------------------------------------
| Name:p_ftperror
| Description:traitement des commandes erronees
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void p_ftperror(void) {
   //printf("Commande non reconnue: %s\n",message);
   send_ftpmsg(new_cmdsock,502,errparam);
}

/*-------------------------------------------
| Name:idcmd
| Description:identification des commandes
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int idcmd(void) {
   if (cmpstr(cmd_quit))   return(c_quit);
   if (cmpstr(cmd_user))   return(c_user);
   if (cmpstr(cmd_pass))   return(c_pass);
   if (cmpstr(cmd_syst))   return(c_syst);
   if (cmpstr(cmd_type))   return(c_type);
   if (cmpstr(cmd_store))  return(c_store);
   if (cmpstr(cmd_retr))   return(c_retr);
   if (cmpstr(cmd_port))   return(c_port);
   if (cmpstr(cmd_list))   return(c_list);
   if (cmpstr(cmd_pwd))    return(c_pwd);
   if (cmpstr(cmd_cwd))    return(c_cwd);
   if (cmpstr(cmd_noop))   return(c_noop);
   if (cmpstr(cmd_opts))   return(c_opts);
   return(c_error);
}

/*-------------------------------------------
| Name:ftpEngine
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int lftpd_main(int argc, char* argv[]){

   /* init structure serveur pour connexion cmd initiale */
   char ftpEngineStatus=1;


   ftpEngineStatus=1;

   //
   chdir(ftp_root);

   //
   memset((char *)&st_serv,0,servlen);

   st_serv.sin_family=AF_INET;
   st_serv.sin_addr.s_addr=INADDR_ANY;

   st_serv.sin_port= htons (ftp_port);

   /* creation du socket initial */
   //cmdsock=socket(0,0,0);
   cmdsock=socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);

   /* attachement du socket pour la connexion initiale */
   if(bind(cmdsock,(struct sockaddr*)&st_serv,servlen))
   {
	   //printf("Bind connexion initiale ");
	   exit(0);
   }

   /* mise a l'ecoute */
   listen(cmdsock,1);
   //printf("Serveur ftp a l'ecoute sur le port %d\n",_ntohs(st_serv.sin_port));

   /* attente connexion client sur nouveau socket */
listen:
   new_cmdsock=accept(cmdsock,(struct sockaddr*)&st_cli,&clilen);
   //printf("Client connecte sur le port cmd %d\n",_ntohs(st_cli.sin_port));

   /* envoi banniere ftp au client */

   send_ftpmsg(new_cmdsock,220,banniere);

   /* boucle principale: lecture des requetes, identification et execution */

   ind_type= type_ascii;	/* type ASCII par defaut */

   while(ftpEngineStatus) {

	   /* lecture de la requete */
	   clear(message);
	   //msglen= read(new_cmdsock,message,ncar);
      msglen= recv(new_cmdsock,message,ncar,0);
      if(msglen<=0)
         break;

	   //printf("commande recue: %s",message);

	   /* identification et branchement */
	   /* la commande est placee dans bufcmd */
	   /* et le parametre dans param */
	   sscanf(message,"%s %s",bufcmd,param);

	   switch (idcmd()) {
		   case c_quit: {
			   ftpEngineStatus=0;
			   send_ftpmsg(new_cmdsock,221,"    Au revoir :o)\r\n");
            //printf("ftpEngineStatus = 0\n");
			   break;
			   }
		   case c_port: {
			   p_port();
			   break;
			   }
		   case c_user: {
			   p_user();
			   break;
			   }
		   case c_type: {
			   p_type();
			   break;
			   }
		   case c_pass: {
			   p_pass();
			   break;
			   }
		   case c_syst: {
			   p_syst();
			   break;
			   }
		   case c_store: {
			   p_store();
			   break;
			   }
         case c_retr: {
			   p_retr();
			   break;
			   }
		   case c_list: {
			   p_list();
			   break;
			   }
		   case c_pwd: {
			   p_pwd();
			   break;
			   }
		   case c_cwd: {
			   p_cwd();
			   break;
			   }

         //
         case c_noop:{
            send_ftpmsg(new_cmdsock,250,"    Commande noop OK\r\n");
            break;
         }

         case c_opts:{
            send_ftpmsg(new_cmdsock,501,"    Commande opts not supported\r\n");
         break;
         }

		   case c_error: {
			   p_ftperror();
			   break;
			   }
	   }

      //
      //Sleep(1);
   }

   //printf("connexion shutdown...\n");

   shutdown(new_cmdsock,0);
   close(new_cmdsock);
   
   shutdown(cmdsock,0);
   close(cmdsock);
   
   //printf("connexion cmd ferme\n");

   //bye
   //printf("\nbye!\n");
   goto listen;

   //bye
   //printf("\nbye!\n");
   return 0;
}


/*===========================================
End of Source lftpd.c
=============================================*/
