/*============================================
| Includes    
==============================================*/
#include "kernel/core/types.h"
#include "kernel/core/interrupt.h"
#include "kernel/core/kernelconf.h"
#include "kernel/core/kernel.h"
#include "kernel/core/system.h"
#include "kernel/core/process.h"
#include "kernel/core/stat.h"
#include "kernel/core/fcntl.h"
#include "kernel/core/ioctl.h"
#include "kernel/core/cpu.h"
#include "kernel/core/malloc.h"

#include "kernel/fs/vfs/vfsdev.h"

#include "lib/libc/termios/termios.h"

#include "ppp.h"
#include "ahdlc.h"
#include "ipcp.h"
#include "ipv6cp.h"
#include "lcp.h"
#include "dev_ppp_uip.h"


/*============================================
| Global Declaration 
==============================================*/
const char dev_ppp_uip_name[]="net/ppp\0";

int dev_ppp_uip_load(void);
int dev_ppp_uip_open(desc_t desc, int o_flag);
int dev_ppp_uip_close(desc_t desc);
int dev_ppp_uip_isset_read(desc_t desc);
int dev_ppp_uip_isset_write(desc_t desc);
int dev_ppp_uip_read(desc_t desc, char* buf,int size);
int dev_ppp_uip_write(desc_t desc, const char* buf,int size);
int dev_ppp_uip_seek(desc_t desc,int offset,int origin);
int dev_ppp_uip_ioctl(desc_t desc,int request,va_list ap);

dev_map_t dev_ppp_uip_map={
   dev_ppp_uip_name,
   S_IFCHR,
   dev_ppp_uip_load,
   dev_ppp_uip_open,
   dev_ppp_uip_close,
   dev_ppp_uip_isset_read,
   dev_ppp_uip_isset_write,
   dev_ppp_uip_read,
   dev_ppp_uip_write,
   dev_ppp_uip_seek,
   dev_ppp_uip_ioctl //ioctl
};


#define PPP_UIP_STACK_SIZE    2048
#define PPP_UIP_PRIORITY      100

#define LCP_RETRY_MAX      1
#define IPCP_RETRY_MAX     2
#define LCP_ECHO_RETRY_MAX 4

volatile static desc_t desc_rd = -1;   //O_RDONLY
volatile static desc_t desc_wr = -1;   //O_WRONLY

struct uip_fw_netif pppif={0};

static u16_t ppp_rx_frame_count_prev=0;
extern u16_t ppp_rx_frame_count;

iface_ppp_stat_t g_iface_ppp_stat;

extern u16_t ppp_rx_frame_count;
extern u8_t ppp_tx_buffer[];

/*============================================
| Implementation 
==============================================*/

/*--------------------------------------------
| Name:        ppp_arch_putchar
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
void ppp_arch_putchar(u8_t c){
   uchar8_t _kernel_int;
   int r=-1;
   desc_t desc_link;
   //
   if(desc_wr<0)
      return;
   //
   desc_link = ofile_lst[desc_wr].desc_nxt[1];
   if(desc_link<0)
      return;

//   ofile_lst[desc_link].pfsop->fdev.fdev_write(desc_link,&c,1);
//   //
//   do{
//      _kernel_int = __wait_io_int(ofile_lst[desc_link].owner_pthread_ptr_write);
//      /*if (!__K_IS_IOINTR(_kernel_int)) 
//         continue;*/
//   }while(ofile_lst[desc_link].pfsop->fdev.fdev_isset_write(desc_link));
   while(ofile_lst[desc_link].pfsop->fdev.fdev_isset_write(desc_link)){
       __wait_io_int(ofile_lst[desc_link].owner_pthread_ptr_write);
   }
   //
   ofile_lst[desc_link].pfsop->fdev.fdev_write(desc_link,&c,1);
}


/*--------------------------------------------
| Name:        dev_ppp_uip_core
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
static void dev_ppp_uip_core(u8_t c){

   uip_len = 0;

   if(!(ppp_flags & PPP_RX_READY)) {
      return;
   }

   if(uip_len == 0) {
      ahdlc_rx(c);
      if(ppp_rx_frame_count_prev==ppp_rx_frame_count)
         return;
   }

   //
   ppp_rx_frame_count_prev=ppp_rx_frame_count;

   /* if an ip frame or PPP control protocol frame */
   if(uip_len>0)
     return; 

   /* lcp remote send a TERM_REQ.  connection is down */
   if(lcp_state&LCP_TERM_PEER){
      ipcp_state = 0;
      ipv6cp_state = 0;
      lcp_state    = 0;
      //
      g_iface_ppp_stat.state = PPP_STATE_NULL;
      g_iface_ppp_stat.is_up = PPP_DOWN;
      return;
   }
   //
   if(g_iface_ppp_stat.is_up == PPP_UP)
      return;

   /* call the lcp task to bring up the LCP layer */
   if( !(lcp_state & LCP_TX_UP) && (g_iface_ppp_stat.state!=PPP_STATE_SEND_LCP_REQ_IN_PROGRESS)){
      lcp_task( &uip_buf[ UIP_LLH_LEN ]);
      //
      g_iface_ppp_stat.state=PPP_STATE_SEND_LCP_REQ_IN_PROGRESS;
   }


   /* If LCP is up, neg next layer */
   if((lcp_state & LCP_TX_UP) && (lcp_state & LCP_RX_UP)) {
      /* If LCP wants PAP, try to authenticate, else bring up IP(V6)CP */
      if((lcp_state & LCP_RX_AUTH) && (!(pap_state & PAP_TX_UP))) {
         pap_task( &uip_buf[ UIP_LLH_LEN ]);  
      }else{
         #if UIP_CONF_IPV6
            if((g_iface_ppp_stat.state!=PPP_STATE_SEND_IPV6CP_REQ_IN_PROGRESS)){
               ipv6cp_task( &uip_buf[ UIP_LLH_LEN ]);
               //
               g_iface_ppp_stat.state=PPP_STATE_SEND_IPV6CP_REQ_IN_PROGRESS;
            }
            //
            if((ipv6cp_state & IPV6CP_TX_UP) && (ipv6cp_state & IPV6CP_RX_UP)) {
               ipv6cp_state = 0;
               lcp_state    = 0;
               g_iface_ppp_stat.state = PPP_STATE_NULL;
               g_iface_ppp_stat.is_up = PPP_UP;
               g_iface_ppp_stat.lcp_retry = 0;
            }else{
               g_iface_ppp_stat.is_up = PPP_DOWN;
            }
         #else
            if((g_iface_ppp_stat.state!=PPP_STATE_SEND_IPCP_REQ_IN_PROGRESS)){
               ipcp_task( &uip_buf[ UIP_LLH_LEN ]);
               //
               g_iface_ppp_stat.state=PPP_STATE_SEND_IPCP_REQ_IN_PROGRESS;
            }
            //
            if((ipcp_state & IPCP_TX_UP) && (ipcp_state & IPCP_RX_UP)) {
               ipcp_state = 0;
               lcp_state    = 0;
               g_iface_ppp_stat.state = PPP_STATE_NULL;
               g_iface_ppp_stat.is_up = PPP_UP;
               g_iface_ppp_stat.lcp_retry = 0;
               g_iface_ppp_stat.ipcp_retry = 0;
            }else{
               g_iface_ppp_stat.is_up = PPP_DOWN;
            }
         #endif
      }
   }
}

/*-------------------------------------------
| Name:dev_ppp_uip_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int dev_ppp_uip_load(void){

   //
   ppp_init();
   //
   g_iface_ppp_stat.state = PPP_STATE_NULL;
   g_iface_ppp_stat.is_up = PPP_DOWN;
   g_iface_ppp_stat.lcp_retry=0;
   g_iface_ppp_stat.ipcp_retry = 0;
   g_iface_ppp_stat.lcp_echo_retry=0;
   
   return 0;
}

/*-------------------------------------------
| Name:dev_ppp_uip_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int dev_ppp_uip_open(desc_t desc, int o_flag){

   if(!ofile_lst[desc].nb_reader && !ofile_lst[desc].nb_writer){

   }

   //
   if(o_flag & O_RDONLY){
      if(desc_rd>=0) //already open: exclusive resource.
         return -1;
      desc_rd = desc;
   }

   if(o_flag & O_WRONLY){
      if(desc_wr>=0) //already open: exclusive resource.
         return -1;
      desc_wr = desc;
   }

   return 0;
}

/*-------------------------------------------
| Name:dev_ppp_uip_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int dev_ppp_uip_close(desc_t desc){
   if(ofile_lst[desc].oflag & O_RDONLY){
      if(!ofile_lst[desc].nb_reader){
      }
   }

   if(ofile_lst[desc].oflag & O_WRONLY){
      if(!ofile_lst[desc].nb_writer){
      }
   }

   if(!ofile_lst[desc].nb_writer
      && !ofile_lst[desc].nb_reader){
      
   }
   return 0;
}

/*-------------------------------------------
| Name:dev_ppp_uip_isset_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int dev_ppp_uip_isset_read(desc_t desc){
   desc_t desc_link = ofile_lst[desc].desc_nxt[0];
   if(desc_link<0)
      return -1;
   if(!ofile_lst[desc_link].pfsop->fdev.fdev_isset_read)
      return -1;
   if(uip_len)
      return 0;

   return ofile_lst[desc_link].pfsop->fdev.fdev_isset_read(desc_link);
}

/*-------------------------------------------
| Name:dev_ppp_uip_isset_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int dev_ppp_uip_isset_write(desc_t desc){
   return 0;//synchronous write

}
/*-------------------------------------------
| Name:dev_ppp_uip_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int dev_ppp_uip_read(desc_t desc, char* buf,int size){

   unsigned char c;
   unsigned char* p_uip_buf = (unsigned char*)&uip_buf[ UIP_LLH_LEN ];

   desc_t desc_link = ofile_lst[desc].desc_nxt[0];
   if(desc_link<0)
      return -1;
   //
   while(ofile_lst[desc_link].pfsop->fdev.fdev_read(desc_link,&c,1)>0){
      dev_ppp_uip_core(c);
      if(uip_len>0)
         break;
   }

   if(!uip_len)
      return 0;
   //
   if(uip_len<size)
      size =uip_len;
   //
   if(size>4){
      memcpy(buf,p_uip_buf,size);
   }else{
      int i;
      for(i=0;i<4;i++)
         *buf++=*p_uip_buf++;
   }
   //
   return size;
}

/*-------------------------------------------
| Name:dev_ppp_uip_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int dev_ppp_uip_write(desc_t desc, const char* buf,int size){
  /* If IPCP came up then our link should be up. */

   #if UIP_CONF_IPV6
     /* If IPV6CP came up then our link should be up. */
     if(g_iface_ppp_stat.is_up == PPP_DOWN)
       return -1;
     
     ahdlc_tx(IPV6, &uip_buf[ UIP_LLH_LEN ],uip_appdata,
          ((uip_len-UIP_TCPIP_HLEN)>0?UIP_TCPIP_HLEN:size), ((uip_len-UIP_TCPIP_HLEN)>0?uip_len-UIP_TCPIP_HLEN:0));//phlb modif grouik code but it works.
   
  #else
      if(g_iface_ppp_stat.is_up == PPP_DOWN)
         return -1;
      
      ahdlc_tx(IPV4, &uip_buf[ UIP_LLH_LEN ],uip_appdata,
		   UIP_TCPIP_HLEN, uip_len - UIP_TCPIP_HLEN);

  #endif
  
  
  return (size);
}

/*-------------------------------------------
| Name:dev_ppp_uip_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int dev_ppp_uip_seek(desc_t desc,int offset,int origin){
   return -1;
}

/*-------------------------------------------
| Name:dev_ppp_uip_ioctl
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int dev_ppp_uip_ioctl(desc_t desc,int request,va_list ap){
    
   switch(request){

      case I_LINK:{

         //to do: check all desc and desc_link are initialized. 
         //if it's ok call ppp_init();
         if(desc_rd<0)
            return 0;
         if(desc_wr<0)
            return 0;
         if(ofile_lst[desc_rd].desc_nxt[0]<0)
            return 0;
         if(ofile_lst[desc_wr].desc_nxt[1]<0)
            return 0;
         //
         return 0;
      }
      // No break : intentionally done due to the fact that the I_LINK "case" ends with return

      case I_UNLINK:{
      }
      break;
    
      //
      case PPPUP:{
         //
         iface_ppp_stat_t* iface_ppp_stat = (iface_ppp_stat_t*) va_arg(ap, iface_ppp_stat_t*);
         
         /* Enable PPP */
         ppp_flags = PPP_RX_READY;
         // 
         if((lcp_state & LCP_RX_UP) || (lcp_state & LCP_TX_UP)){
            if( ((lcp_state & (LCP_RX_UP|LCP_TX_UP)) != (LCP_RX_UP|LCP_TX_UP))
               && ++g_iface_ppp_stat.lcp_retry>LCP_RETRY_MAX){
               ipcp_state = 0;
               ipv6cp_state = 0;
               lcp_state    = 0;
               g_iface_ppp_stat.state = PPP_STATE_NULL;
               g_iface_ppp_stat.lcp_retry=0;
            }else if(  ((ipcp_state & (IPCP_TX_UP|IPCP_RX_UP))!=(IPCP_TX_UP|IPCP_RX_UP)) ){
                      
               if(++g_iface_ppp_stat.ipcp_retry>IPCP_RETRY_MAX){
                  g_iface_ppp_stat.ipcp_retry = 0;
                  ipcp_state = 0;
                  g_iface_ppp_stat.state = PPP_STATE_NULL;
                  g_iface_ppp_stat.lcp_retry=0;
                  return 0;
                }
                //
                ipcp_task( &uip_buf[ UIP_LLH_LEN ]);
                //
                g_iface_ppp_stat.state=PPP_STATE_SEND_IPCP_REQ_IN_PROGRESS;
            }
            return 0;
         }
         g_iface_ppp_stat.state=PPP_STATE_SEND_LCP_REQ_IN_PROGRESS;
         //up the ppp link (client mode) test 10/06/2011
         lcp_task(ppp_tx_buffer);
      }
      return 0;
      
      case PPPECHO:{
         if(g_iface_ppp_stat.state&(PPP_STATE_SEND_LCP_ECHO_REQ_IN_PROGRESS))
         {
            g_iface_ppp_stat.lcp_echo_retry++;
         }
         if(g_iface_ppp_stat.lcp_echo_retry>LCP_ECHO_RETRY_MAX){
            g_iface_ppp_stat.lcp_echo_retry = 0;
            g_iface_ppp_stat.is_up = PPP_DOWN;
            return -1;
         }
         g_iface_ppp_stat.state|=PPP_STATE_SEND_LCP_ECHO_REQ_IN_PROGRESS;
         lcp_echo(ppp_tx_buffer);
      }
      return 0;
      
      //
      case PPPDWN:{
        //
        iface_ppp_stat_t* iface_ppp_stat = (iface_ppp_stat_t*) va_arg(ap, iface_ppp_stat_t*);
        //
        lcp_disconnect(ppp_tx_buffer);
        //
        ppp_init();
        //
        g_iface_ppp_stat.state=(PPP_STATE_NULL);
        //
        iface_ppp_stat->is_up = PPP_DOWN;
      }
      return 0;
      
      case PPPSTAT:{ 
         // LVD 23/05/2011
         iface_ppp_stat_t* iface_ppp_stat = (iface_ppp_stat_t*) va_arg(ap, iface_ppp_stat_t*);
         //
         if(!iface_ppp_stat)
            return -1;
         iface_ppp_stat->is_up = g_iface_ppp_stat.is_up;
      }//
      return 0;
  
      //
      default:
         return -1;
   }
   //
   return -1;
}


/*============================================
| End of Source  : dev_ppp_uip.c
==============================================*/