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


#include "lib/libc/libc.h"
#include "lib/libc/stdio/stdio.h"

/*============================================
| Global Declaration 
==============================================*/


/*============================================
| Implementation 
==============================================*/
/*--------------------------------------------
| Name:        _init_stdin
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
void _init_stdin(FILE* std, unsigned char* buf){
   pthread_mutexattr_t  mutex_attr=0;
   std->bufpos   = buf;
   std->bufread  = buf;
   std->bufwrite = buf;
   std->bufstart = buf;
   std->bufend   = (buf+BUFSIZ);
   std->fd = 0;
   std->mode =  _IOFBF | __MODE_READ | __MODE_IOTRAN;

   //kernel_pthread_mutex_init(&std->mutex,&mutex_attr);
}

/*--------------------------------------------
| Name:        _init_stdout
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
void _init_stdout(FILE* std, unsigned char* buf){
   //stdout
   pthread_mutexattr_t  mutex_attr=0;
   std->bufpos   = buf;
   std->bufread  = buf;
   std->bufwrite = buf;
   std->bufstart = buf;
   std->bufend   = (buf+BUFSIZ);

   std->fd = 1;
   std->mode =  _IONBF | __MODE_WRITE | __MODE_IOTRAN;

   //kernel_pthread_mutex_init(&std->mutex,&mutex_attr);
}

/*--------------------------------------------
| Name:        _init_stderr
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
void _init_stderr(FILE* std, unsigned char* buf){
   //stdout
   pthread_mutexattr_t  mutex_attr=0;
   std->bufpos   = buf;
   std->bufread  = buf;
   std->bufwrite = buf;
   std->bufstart = buf;
   std->bufend   = (buf+BUFSIZ);

   std->fd = 2;
   std->mode =  _IONBF | __MODE_WRITE | __MODE_IOTRAN;

   //kernel_pthread_mutex_init(&std->mutex,&mutex_attr);
}

/*--------------------------------------------
| Name:        __implement_fct_lib_entrypoint(libc)
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
#if defined(__KERNEL_LOAD_LIB)
int __implement_fct_lib_entrypoint(libc)(kernel_pthread_t* pthread_ptr){
   libc_stdio_data_t* p_libc_stdio_data;
   
   //alloc libc data
   p_libc_stdio_data = (libc_stdio_data_t*)__lib_data_alloc(pthread_ptr,sizeof(libc_stdio_data_t));

   if(!p_libc_stdio_data)
      return -1;

   _init_stdin(&p_libc_stdio_data->_stdin,p_libc_stdio_data->_buf_in);
   _init_stdout(&p_libc_stdio_data->_stdout,p_libc_stdio_data->_buf_out);
   _init_stderr(&p_libc_stdio_data->_stderr,p_libc_stdio_data->_buf_err);
   
   return 0;
}
#endif

/*============================================
| End of Source  : libc.c
==============================================*/
