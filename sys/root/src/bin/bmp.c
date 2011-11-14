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


/*============================================
| Includes    
==============================================*/
#include "kernel/core/signal.h"
#include "kernel/core/libstd.h"
#include "kernel/core/devio.h"
#include "kernel/core/ioctl_lcd.h"
#include "kernel/core/fcntl.h"
#include "lib/libc/stdio/stdio.h"


/*============================================
| Global Declaration 
==============================================*/
typedef struct {unsigned char byte0,byte1,byte2,byte3;} four_byte_t;

typedef struct {unsigned char byte0,byte1;} two_byte_t;

typedef struct { 
   two_byte_t magic_nr;
   four_byte_t file_length;
   four_byte_t reserved_area;
   four_byte_t data_pointer;
} bmp_file_header_t;

typedef struct { 
   four_byte_t format_header_length;
   four_byte_t picture_width;
   four_byte_t picture_height;
   two_byte_t  planes_count;
   two_byte_t  bits_per_pixel;
   four_byte_t compression;
   four_byte_t imagesize;
   four_byte_t x_pixel_per_meter;
   four_byte_t y_pixel_per_meter;
   four_byte_t amount_used_colors;
   four_byte_t amount_important_colors;
} bmp_format_header_t;

typedef struct { 
   bmp_file_header_t    file_header;
   bmp_format_header_t  format_header;
} bmp_header_t;

/*============================================
| Implementation 
==============================================*/
/*--------------------------------------------
| Name:        read_bmp_header
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int read_bmp_header(char* filename,bmp_header_t* p_bmp_header_t){
   FILE *fp;
   bmp_header_t header;

   if((fp=fopen(filename,"rb"))==(void*)0)
      return -1;

   header.file_header.magic_nr.byte0 = fgetc(fp);
   header.file_header.magic_nr.byte1 = fgetc(fp);

   header.file_header.file_length.byte0 = fgetc(fp);
   header.file_header.file_length.byte1 = fgetc(fp);
   header.file_header.file_length.byte2 = fgetc(fp);
   header.file_header.file_length.byte3 = fgetc(fp);

   header.file_header.reserved_area.byte0= fgetc(fp);
   header.file_header.reserved_area.byte1= fgetc(fp);
   header.file_header.reserved_area.byte2= fgetc(fp);
   header.file_header.reserved_area.byte3= fgetc(fp);

   header.file_header.data_pointer.byte0 = fgetc(fp);
   header.file_header.data_pointer.byte1 = fgetc(fp);
   header.file_header.data_pointer.byte2 = fgetc(fp);
   header.file_header.data_pointer.byte3 = fgetc(fp);


   header.format_header.format_header_length.byte0 = fgetc(fp);
   header.format_header.format_header_length.byte1 = fgetc(fp);
   header.format_header.format_header_length.byte2 = fgetc(fp);
   header.format_header.format_header_length.byte3 = fgetc(fp);

   header.format_header.picture_width.byte0 = fgetc(fp);
   header.format_header.picture_width.byte1 = fgetc(fp);
   header.format_header.picture_width.byte2 = fgetc(fp);
   header.format_header.picture_width.byte3 = fgetc(fp);

   header.format_header.picture_height.byte0 = fgetc(fp);
   header.format_header.picture_height.byte1 = fgetc(fp);
   header.format_header.picture_height.byte2 = fgetc(fp);
   header.format_header.picture_height.byte3 = fgetc(fp);

   header.format_header.planes_count.byte0 = fgetc(fp);
   header.format_header.planes_count.byte1 = fgetc(fp);

   header.format_header.bits_per_pixel.byte0 = fgetc(fp);
   header.format_header.bits_per_pixel.byte1 = fgetc(fp);

   header.format_header.compression.byte0 = fgetc(fp);
   header.format_header.compression.byte1 = fgetc(fp);
   header.format_header.compression.byte2 = fgetc(fp);
   header.format_header.compression.byte3 = fgetc(fp);

   header.format_header.imagesize.byte0 = fgetc(fp);
   header.format_header.imagesize.byte1 = fgetc(fp);
   header.format_header.imagesize.byte2 = fgetc(fp);
   header.format_header.imagesize.byte3 = fgetc(fp);

   header.format_header.x_pixel_per_meter.byte0 = fgetc(fp);
   header.format_header.x_pixel_per_meter.byte1 = fgetc(fp);
   header.format_header.x_pixel_per_meter.byte2 = fgetc(fp);
   header.format_header.x_pixel_per_meter.byte3 = fgetc(fp);

   header.format_header.y_pixel_per_meter.byte0 = fgetc(fp);
   header.format_header.y_pixel_per_meter.byte1 = fgetc(fp);
   header.format_header.y_pixel_per_meter.byte2 = fgetc(fp);
   header.format_header.y_pixel_per_meter.byte3 = fgetc(fp);

   header.format_header.amount_used_colors.byte0 = fgetc(fp);
   header.format_header.amount_used_colors.byte1 = fgetc(fp);
   header.format_header.amount_used_colors.byte2 = fgetc(fp);
   header.format_header.amount_used_colors.byte3 = fgetc(fp);

   header.format_header.amount_important_colors.byte0 = fgetc(fp);
   header.format_header.amount_important_colors.byte1 = fgetc(fp);
   header.format_header.amount_important_colors.byte2 = fgetc(fp);
   header.format_header.amount_important_colors.byte3 = fgetc(fp);

   fclose(fp);

   memcpy(p_bmp_header_t,&header,sizeof(bmp_header_t));

   return 0;
}

/*--------------------------------------------
| Name:        bmp2raw
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int bmp2raw(int fd, char* bmp_filename){
   bmp_header_t header;
   int xdim,ydim;
   int l=0;
   int offset;
   int fd_bmp=0;
   unsigned char l_buf[40*64];//320 pixels by line
   unsigned char* p_l_buf=l_buf;

   if(read_bmp_header(bmp_filename,&header)<0)
      return -1;

   xdim= header.format_header.picture_width.byte0
   + header.format_header.picture_width.byte1 * 256
   + header.format_header.picture_width.byte2 * 256*256
   + header.format_header.picture_width.byte3 * 256*256*256;

   ydim= header.format_header.picture_height.byte0
   + header.format_header.picture_height.byte1 * 256
   + header.format_header.picture_height.byte2 * 256*256
   + header.format_header.picture_height.byte3 * 256*256*256;

   offset = header.file_header.data_pointer.byte0
   + header.file_header.data_pointer.byte1 *256
   + header.file_header.data_pointer.byte2 *256*256
   + header.file_header.data_pointer.byte3 *256*256*256;

   //
   fd_bmp=open(bmp_filename,O_RDONLY,0);
   
   //
   lseek(fd_bmp,0,SEEK_END);

   l=0;
   while(l<ydim){
      int cb=0;
      int r=0;

      p_l_buf=l_buf;
      
      //read bmp file
      while( (cb+=r)<sizeof(l_buf) ){
         lseek(fd_bmp,-(xdim/8),SEEK_CUR);
         if((r=read(fd_bmp,p_l_buf,xdim/8))<=0){
            break;
         }
         l++;
         p_l_buf+=r;
         if(l==ydim)
            break;
         lseek(fd_bmp,-(xdim/8),SEEK_CUR);
      }
      if(!cb)
         break;
      //write in frame buffer
      //lseek(fd,(l*(xdim/8)),SEEK_SET);
      write(fd,l_buf,cb);
   }

   ioctl(fd,LCDFLSBUF,(void*)0);

   return 0;
}

/*--------------------------------------------
| Name:        bmp_main
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int bmp_main(int argc,char* argv[]){
   int fd=-1;//stdout
   int i;

   //get options
   for(i=1;i<argc;i++){
      if (argv[i][0] == '-') {
         switch (argv[i][1]) {
            case 'o':   //output device
               if(argc >= i+1) {
                  if((fd=open(argv[i+1],O_WRONLY,0))<0){
                     printf("error: cannot open output device\r\n");
                     return -1;
                  }
                  i++;
               }
               break;
         }  //fin du switch
      }  //fin du if(argv...
      else{
         if(argv[i]){
            if(fd<0)
               fd=1;//stdout
            if(bmp2raw(fd,argv[i])<0)
               printf("error: cannot display%s\r\n",argv[i]);
         }

      }
   }  //fin du for
   

   return 0;
}

/*============================================
| End of Source  : bmp.c
==============================================*/
