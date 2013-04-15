/*--------------------------------------------
| Copyright(C) 2005 CHAUVIN-ARNOUX
|---------------------------------------------
| Project:         
| Project Manager: 
| Source:          dev_ppp_uip.h
| Path:        C:\tauon\sys\root\src\kernel\dev\arch\all\ppp\dev_ppp_uip
| Authors:     
| Plateform:   
| Created:     
| Revision/Date: $Revision:$  $Date:$ 
| Description: 
|---------------------------------------------
| Historic:    
|---------------------------------------------
| Authors     | Date     | Comments  
| $Log:$
|---------------------------------------------*/


/*============================================
| Compiler Directive   
==============================================*/
#ifndef _DEV_PPP_UIP_H
#define _DEV_PPP_UIP_H


/*============================================
| Includes 
==============================================*/


/*============================================
| Declaration  
==============================================*/


#define PPPUP     (0x00000001)
#define PPPDWN    (0x00000002)
#define PPPSTAT   (0x00000003)
#define PPPECHO   (0x00000004)

#define PPP_UP       (1)
#define PPP_DOWN     (0)
#define PPP_SHUTDOWN (-1)

#define PPP_STATE_NULL                          (0x0000)
#define PPP_STATE_SEND_LCP_REQ_IN_PROGRESS      (0x0001)
#define PPP_STATE_SEND_IPCP_REQ_IN_PROGRESS     (0x0002) 
#define PPP_STATE_SEND_IPV6CP_REQ_IN_PROGRESS   (0x0002)  
#define PPP_STATE_SEND_LCP_ECHO_REQ_IN_PROGRESS (0X0010)

typedef struct iface_ppp_stat_st{
   int is_up;
   unsigned long state;
   int lcp_retry;
   int ipcp_retry;
   int lcp_echo_retry;
}iface_ppp_stat_t;



#endif