/*
The contents of this file are subject to the Mozilla Public License Version 1.1 
(the "License"); you may not use this file except in compliance with the License.
You may obtain a copy of the License at http://www.mozilla.org/MPL/

Software distributed under the License is distributed on an "AS IS" basis, 
WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License for the 
specific language governing rights and limitations under the License.

The Original Code is ______________________________________.

The Initial Developer of the Original Code is ________________________.
Portions created by ______________________ are Copyright (C) ______ _______________________.
All Rights Reserved.

Contributor(s): ______________________________________.

Alternatively, the contents of this file may be used under the terms of the eCos GPL license 
(the  [eCos GPL] License), in which case the provisions of [eCos GPL] License are applicable 
instead of those above. If you wish to allow use of your version of this file only under the
terms of the [eCos GPL] License and not to allow others to use your version of this file under 
the MPL, indicate your decision by deleting  the provisions above and replace 
them with the notice and other provisions required by the [eCos GPL] License. 
If you do not delete the provisions above, a recipient may use your version of this file under 
either the MPL or the [eCos GPL] License."
*/


/*============================================
| Includes    
==============================================*/
#define WIN32_LEAN_AND_MEAN
/* get the windows definitions of the following 4 functions out of the way */
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>


#include <commdlg.h>
#include <string.h>
#include <stdio.h>
#include <conio.h>
#include <io.h>
#include <memory.h>

#include "kernel/core/ucore/embOSW32_100/segger_intr.h"


#include "WpdPack_3_1/WpdPack/Include/packet32.h"
#include "WpdPack_3_1/WpdPack/Include/ntddndis.h"

/*============================================
| Global Declaration 
==============================================*/

LPADAPTER  lpAdapter;
LPPACKET   lpPacket;
char buffer[256000];  // buffer to hold the data coming from the driver
unsigned char *cur_packet;
int cur_length;
unsigned char lwip_ethaddr[6];

DWORD PacketTaskId;
HANDLE hPacketTask;

void update_adapter_thread(void);

typedef struct{
   char * p_packet;
   int packet_len;
}packet_mem_t;


int packet_rd=0;
int packet_wr=0;
#define MEM_PACKET_MAX 32
packet_mem_t packet_mem[MEM_PACKET_MAX]={0};

volatile int adapter_interrupt = 0;
/*============================================
| Implementation 
==============================================*/

/*--------------------------------------------
| Name:        init_adapter
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int init_adapter(int adapter_num)
{
  #define Max_Num_Adapter 10

  char AdapterList[Max_Num_Adapter][1024];

	int i;
	DWORD dwVersion;
	DWORD dwWindowsMajorVersion;

	//unicode strings (winnt)
	char		AdapterName[512]; // string that contains a list of the network adapters
	char		*temp,*temp1;

	//ascii strings (win95)
	char		AdapterNamea[512]; // string that contains a list of the network adapters
	char		*tempa,*temp1a;

	int			AdapterNum=0;
	ULONG		AdapterLength;

  PPACKET_OID_DATA ppacket_oid_data;
	
	// obtain the name of the adapters installed on this machine
	AdapterLength=512;

  memset(AdapterList,0,sizeof(AdapterList));

  i=0;

	// the data returned by PacketGetAdapterNames is different in Win95 and in WinNT.
	// We have to check the os on which we are running
	dwVersion=GetVersion();
	dwWindowsMajorVersion =  (DWORD)(LOBYTE(LOWORD(dwVersion)));
	if (!(dwVersion >= 0x80000000 && dwWindowsMajorVersion >= 4))
	{  // Windows NT
		PacketGetAdapterNames((char *)AdapterName,&AdapterLength);
		temp=AdapterName;
		temp1=AdapterName;
		while ((*temp!='\0')||(*(temp-1)!='\0'))
		{
			if (*temp=='\0') 
			{
				memcpy(AdapterList[i],temp1,(temp-temp1)*2);
				temp1=temp+1;
				i++;
		}
	
		temp++;
		}
	  
		AdapterNum=i;
	}else	{
      //windows 95
		PacketGetAdapterNames(AdapterNamea,&AdapterLength);
		tempa=AdapterNamea;
		temp1a=AdapterNamea;

		while ((*tempa!='\0')||(*(tempa-1)!='\0'))
		{
			if (*tempa=='\0') 
			{
				memcpy(AdapterList[i],temp1a,tempa-temp1a);
				temp1a=tempa+1;
				i++;
			}
			tempa++;
		}
		  
		AdapterNum=i;
	}

   if (AdapterNum<=0)
    return -1;
   if (AdapterNum<=0)
    return -1;
   if (adapter_num < 0)
     return -1;
   if (adapter_num >= AdapterNum)
     return -1;

   ppacket_oid_data=malloc(sizeof(PACKET_OID_DATA)+6);
   lpAdapter=PacketOpenAdapter(AdapterList[/*adapter_num*//*1*/1]);

   if (!lpAdapter || (lpAdapter->hFile == INVALID_HANDLE_VALUE))
     return -1;

   ppacket_oid_data->Oid=OID_802_3_PERMANENT_ADDRESS;
   ppacket_oid_data->Length=6;

   if (!PacketRequest(lpAdapter,FALSE,ppacket_oid_data))
	   return -1;

   memcpy(&lwip_ethaddr,ppacket_oid_data->Data,6);
   free(ppacket_oid_data);
   printf("MAC: %2X%2X%2X%2X%2X%2X\n", lwip_ethaddr[0], lwip_ethaddr[1], lwip_ethaddr[2], lwip_ethaddr[3], lwip_ethaddr[4], lwip_ethaddr[5]);
   PacketSetBuff(lpAdapter,512000);
   PacketSetReadTimeout(lpAdapter,1);
   PacketSetHwFilter(lpAdapter,NDIS_PACKET_TYPE_ALL_LOCAL|NDIS_PACKET_TYPE_PROMISCUOUS);
   if ((lpPacket = PacketAllocatePacket())==NULL){
	   return (-1);
   }

   //
   for(i=0;i<MEM_PACKET_MAX;i++)
      packet_mem[i].p_packet   = (char*)malloc(2048);
   //
   PacketInitPacket(lpPacket,(char*)buffer,256000);

   hPacketTask = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)update_adapter_thread, NULL, 0, &PacketTaskId ); 

   return 0;
}

/*--------------------------------------------
| Name:        shutdown_adapter
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
void shutdown_adapter(void)
{
	PacketFreePacket(lpPacket);
	PacketCloseAdapter(lpAdapter);
}

/*--------------------------------------------
| Name:        packet_send
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int packet_send(void *buffer, int len)
{
	LPPACKET lpPacket;

   if(!lpAdapter)
      return -1;

	if ((lpPacket = PacketAllocatePacket())==NULL)
 		return -1;
	PacketInitPacket(lpPacket,buffer,len);
	if (!PacketSendPacket(lpAdapter,lpPacket,TRUE))
		return -1;
	PacketFreePacket(lpPacket);

	return 0;
}

extern void process_input(void);

/*--------------------------------------------
| Name:        process_packets
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
extern int __win32_eth_input_r;
extern int __win32_eth_input_w;

static void process_packets(LPPACKET lpPacket)
{
	ULONG	ulLines, ulBytesReceived;
	char	*base;
	char	*buf;
	u_int off=0;
	u_int tlen,tlen1;
	struct bpf_hdr *hdr;

	ulBytesReceived = lpPacket->ulBytesReceived;

	buf = lpPacket->Buffer;

	off=0;

   while (off<ulBytesReceived){	
      //if (kbhit())return;
      hdr=(struct bpf_hdr *)(buf+off);
      tlen1=hdr->bh_datalen;
      cur_length=tlen1;
      tlen=hdr->bh_caplen;
      off+=hdr->bh_hdrlen;

      ulLines = (tlen + 15) / 16;
      if (ulLines > 5) ulLines=5;

      base =(char*)(buf+off);
      cur_packet=base;
      off=Packet_WORDALIGN(off+tlen);

      //
      if(!adapter_interrupt)
         continue;

      //save packet
      packet_mem[packet_wr].packet_len = cur_length;
      //packet_mem[packet_wr].p_packet   = (char*)malloc(cur_length);
      if(!packet_mem[packet_wr].p_packet){
         printf("error: packet_wr=%d !!!\r\n",packet_wr);
         continue;
      }

      memcpy(packet_mem[packet_wr].p_packet,cur_packet,cur_length);

      if((++packet_wr)==MEM_PACKET_MAX)
         packet_wr=0;

      //fire interrupt to cpu
      //printf("packet_wr=%d _input_w=%d _input_r=%d\r\n",packet_wr,__win32_eth_input_w,__win32_eth_input_r);
      emuFireInterrupt(120);
      //process_input();//remove use signal event
	}
}

/*--------------------------------------------
| Name:        update_adapter_thread
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
void update_adapter_thread(void){
   for(;;){
      if (PacketReceivePacket(lpAdapter,lpPacket,TRUE)==TRUE)
         process_packets(lpPacket);
      cur_length=0;
      cur_packet=NULL;
   }
}

/*--------------------------------------------
| Name:        win32_eth_start
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int win32_eth_start(void){
   init_adapter(0);
   return 0;
}

/*--------------------------------------------
| Name:        win32_eth_stop
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int win32_eth_stop(void){
   shutdown_adapter();
   return 0;
}

/*--------------------------------------------
| Name:        win32_eth_enable_interrupt
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int win32_eth_enable_interrupt(void){
   adapter_interrupt =1;
   return 0;
}

/*--------------------------------------------
| Name:        win32_eth_disable_interrupt
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int win32_eth_disable_interrupt(void){
   adapter_interrupt =0;
   return 0;
}

/*--------------------------------------------
| Name:        win32_eth_getpkt
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int win32_eth_getpkt(char* buf, int len){

   int packet_len;
   //save packet
   if((packet_len = packet_mem[packet_rd].packet_len)>len)
      return -1;

   if(packet_mem[packet_rd].packet_len<=0)
      return -1;
   if(!packet_mem[packet_rd].p_packet)
      return -1;

   memcpy(buf,packet_mem[packet_rd].p_packet,packet_len);

   //free(packet_mem[packet_rd].p_packet);
   //packet_mem[packet_rd].p_packet=(char*)0;
   packet_mem[packet_rd].packet_len=0;

   if((++packet_rd)==MEM_PACKET_MAX)
      packet_rd=0;

   return packet_len;
}


/*--------------------------------------------
| Name:        win32_eth_getpkt
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int win32_eth_putpkt(const char* buf, int len){
   return (!packet_send((void*)buf,len)?len:-1);
}

/*============================================
| End of Source  : win32_core_pkt.c
==============================================*/
