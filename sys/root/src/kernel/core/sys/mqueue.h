/*
The contents of this file are subject to the Mozilla Public License Version 1.1
(the "License"); you may not use this file except in compliance with the License.
You may obtain a copy of the License at http://www.mozilla.org/MPL/

Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License for the
specific language governing rights and limitations under the License.

The Original Code is Lepton.

The Initial Developer of the Original Code is Chauvin-Arnoux.
Portions created by Chauvin-Arnoux are Copyright (C) 2011. All Rights Reserved.

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
| Compiler Directive
==============================================*/
#ifndef _MQUEUE_H
#define _MQUEUE_H


/*============================================
| Includes
==============================================*/


/*============================================
| Declaration
==============================================*/

typedef struct mq_attr {
   long mq_flags;    //Attributs : 0 ou O_NONBLOCK. ?reste � d�finir sous lepton.
   long mq_maxmsg;   //Nombre maximum de messages dans la file.
   long mq_msgsize;  //Taille maximum des messages (octets).
   long mq_curmsgs;  //Nombre de messages actuellement dans la file.
}mq_attr_t;

typedef int (*fdev_posix_mqueue_read_t)(desc_t desc, char* buf,int size,...);
typedef int (*fdev_posix_mqueue_write_t)(desc_t desc, const char* buf,int size,...);




#endif
