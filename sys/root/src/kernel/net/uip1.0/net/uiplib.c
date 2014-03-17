
#include "uip.h"
#include "uiplib.h"


/*-----------------------------------------------------------------------------------*/
unsigned char
uiplib_ipaddrconv(char *addrstr, unsigned char *ipaddr)
{
  unsigned char tmp;
  char c;
  unsigned char i, j;

  tmp = 0;
  
  for(i = 0; i < 4; ++i) {
    j = 0;
    do {
      c = *addrstr;
      ++j;
      if(j > 4) {
	return 0;
      }
      if(c == '.' || c == 0) {
	*ipaddr = tmp;
	++ipaddr;
	tmp = 0;
      } else if(c >= '0' && c <= '9') {
	tmp = (tmp * 10) + (c - '0');
      } else {
	return 0;
      }
      ++addrstr;
    } while(c != '.' && c != 0);
  }
  return 1;
}

/*-----------------------------------------------------------------------------------*/
