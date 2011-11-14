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
#include "kernel/core/signal.h"
#include "kernel/core/libstd.h"
#include "kernel/core/stat.h"
#include "kernel/core/statvfs.h"
#include "kernel/core/devio.h"
#include "kernel/core/time.h"
#include "kernel/core/wait.h"
#include "kernel/core/fcntl.h"

#include "lib/libc/stdio/stdio.h"

#include <string.h>
/*===========================================
Global Declaration
=============================================*/


/*===========================================
Implementation
=============================================*/



/*-------------------------------------------
| Name:more_main
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int more_main(int argc,char* argv[]){

	int	fd;
	char	*name;
	char	ch;
	int	line;
	int	col;
	char	buf[80];

   
	while (argc-- >= 1) {

      if(argc){
		   name = *(++argv);

		   fd = open(name, O_RDONLY,0);
		   if (fd == -1) {
			   //perror(name);
			   exit(1);
		   }

         write(STDOUT_FILENO,"<< ",3);
		   write(STDOUT_FILENO,name,strlen(name));
		   write(STDOUT_FILENO," >>\n",4);

      }else{
         fd = STDIN_FILENO;
      }

		
		line = 1;
		col = 0;

		while ((fd > -1) && ((read(fd, &ch, 1)) > 0)) {
			switch (ch) {
				case '\r':
					col = 0;
					break;

				case '\n':
					line++;
					col = 0;
					break;

				case '\t':
					col = ((col + 1) | 0x07) + 1;
					break;

				case '\b':
					if (col > 0)
						col--;
					break;

            case '\x18': //ctrl-x:exit
               return 0;

				default:
					col++;
			}

			//putchar(ch);
         write(STDOUT_FILENO,&ch,1);
			if (col >= 80) {
				col -= 80;
				line++;
			}

			if (line < 24)
				continue;

			if (col > 0)
				write(STDOUT_FILENO,"\n",1);

			write(STDOUT_FILENO,"--More--",8);
			
			if ((read(0, buf, sizeof(buf)) < 0)) {
				if (fd > -1)
					close(fd);
				exit(0);
			}

			ch = buf[0];
			if (ch == ':')
				ch = buf[1];

			switch (ch) {
				case 'N':
				case 'n':
					close(fd);
					fd = -1;
					break;

				case 'Q':
				case 'q':
					close(fd);
					exit(0);
			}

			col = 0;
			line = 1;
		}
		if (fd)
			close(fd);
	}
	
   return 0;
}

/*===========================================
End of Sourcemore.c
=============================================*/
