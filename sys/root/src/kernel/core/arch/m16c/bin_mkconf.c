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


/*===========================================
Includes
=============================================*/
#include "kernel/bin.h"
//see "sbin/init.c"
int init_main(int argc, char* argv[]);

//see "sbin/lsh.c"
int lsh_main(int argc, char* argv[]);

//see "sbin/mkdir.c"
int mkdir_main(int argc, char* argv[]);

//see "sbin/rmdir.c"
int rmdir_main(int argc, char* argv[]);

//see "sbin/rm.c"
int rm_main(int argc, char* argv[]);

//see "sbin/mount.c"
int mount_main(int argc, char* argv[]);

//see "sbin/umount.c"
int umount_main(int argc, char* argv[]);

//see "sbin/ls.c"
int ls_main(int argc, char* argv[]);

//see "sbin/pwd.c"
int pwd_main(int argc, char* argv[]);

//see "sbin/ps.c"
int ps_main(int argc, char* argv[]);

//see "sbin/kill.c"
int kill_main(int argc, char* argv[]);

//see "sbin/touch.c"
int touch_main(int argc, char* argv[]);

//see "sbin/cat.c"
int cat_main(int argc, char* argv[]);

//see "sbin/ecat.c"
int ecat_main(int argc, char* argv[]);

//see "sbin/more.c"
int more_main(int argc, char* argv[]);

//see "sbin/mkfs.c"
int mkfs_main(int argc, char* argv[]);

//see "sbin/df.c"
int df_main(int argc, char* argv[]);

//see "sbin/date.c"
int date_main(int argc, char* argv[]);

//see "sbin/od.c"
int od_main(int argc, char* argv[]);

//see "sbin/stty.c"
int stty_main(int argc, char* argv[]);

//see "sbin/cp.c"
int cp_main(int argc, char* argv[]);

//see "sbin/sleep.c"
int sleep_main(int argc, char* argv[]);

//see "sbin/sync.c"
int sync_main(int argc, char* argv[]);

//see "sbin/wrapr.c"
int wrapr_main(int argc, char* argv[]);

//see "sbin/xmodem.c"
int xmodem_main(int argc, char* argv[]);

//see "sbin/echo.c"
int echo_main(int argc, char* argv[]);

//see "bin/memd.c"
int memd_main(int argc, char* argv[]);

//see "bin/btb.c"
int btb_main(int argc, char* argv[]);

//see "bin/arch/m16c/msr/macq.c"
int macq_main(int argc, char* argv[]);

//see "bin/arch/m16c/msr/mloop.c"
int mloop_main(int argc, char* argv[]);

//see "bin/arch/m16c/msr/mcont.c"
int mcont_main(int argc, char* argv[]);

//see "bin/arch/m16c/msr/mohm.c"
int mohm_main(int argc, char* argv[]);

//see "bin/arch/m16c/msr/mmohm.c"
int mmohm_main(int argc, char* argv[]);

//see "bin/arch/m16c/msr/madj.c"
int madj_main(int argc, char* argv[]);

//see "bin/arch/m16c/msr/mrcd.c"
int mrcd_main(int argc, char* argv[]);

//see "bin/arch/m16c/msr/mearth.c"
int mearth_main(int argc, char* argv[]);

//see "bin/arch/m16c/msrd/msrd.c"
int msrd_main(int argc, char* argv[]);

//see "bin/arch/m16c/tst/madc.c"
int madc_main(int argc, char* argv[]);

//see "bin/arch/m16c/tst/mdac.c"
int mdac_main(int argc, char* argv[]);

//see "bin/arch/m16c/tst/mpga.c"
int mpga_main(int argc, char* argv[]);

//see "bin/arch/m16c/tst/mcal.c"
int mcal_main(int argc, char* argv[]);

//see "bin/arch/m16c/tst/tstport.c"
int tstport_main(int argc, char* argv[]);

//see "bin/arch/m16c/tst/tstslip.c"
int tstslip_main(int argc, char* argv[]);

//see "bin/arch/m16c/tst/tstloop.c"
int tstloop_main(int argc, char* argv[]);

//see "bin/arch/m16c/tst/mtest.c"
int mtest_main(int argc, char* argv[]);



/*===========================================
Global Declaration
=============================================*/
static const bin_t _bin_lst[]={
{          "init",			                  init_main,			100,			512,			1},
{           "lsh",			                   lsh_main,			100,			640,			1},
{         "mkdir",			                 mkdir_main,			100,			512,			1},
{         "rmdir",			                 rmdir_main,			100,			512,			1},
{            "rm",			                    rm_main,			100,			512,			1},
{         "mount",			                 mount_main,			100,			512,			1},
{        "umount",			                umount_main,			100,			512,			1},
{            "ls",			                    ls_main,			100,			640,			1},
{           "pwd",			                   pwd_main,			100,			512,			1},
{            "ps",			                    ps_main,			100,			1024,			1},
{          "kill",			                  kill_main,			100,			512,			1},
{         "touch",			                 touch_main,			100,			512,			1},
{           "cat",			                   cat_main,			100,			512,			1},
{          "ecat",			                  ecat_main,			100,			512,			1},
{          "more",			                  more_main,			100,			512,			1},
{          "mkfs",			                  mkfs_main,			100,			512,			1},
{            "df",			                    df_main,			100,			512,			1},
{          "date",			                  date_main,			100,			1024,			1},
{            "od",			                    od_main,			100,			1024,			1},
{          "stty",			                  stty_main,			100,			1024,			1},
{            "cp",			                    cp_main,			100,			512,			1},
{         "sleep",			                 sleep_main,			100,			256,			1},
{          "sync",			                  sync_main,			100,			128,			1},
{         "wrapr",			                 wrapr_main,			100,			1224,			1},
{        "xmodem",			                xmodem_main,			100,			512,			1},
{          "echo",			                  echo_main,			100,			512,			1},
{          "memd",			                  memd_main,			100,			1024,			1},
{           "btb",			                   btb_main,			100,			512,			1},
{          "macq",			                  macq_main,			100,			1024,			1},
{         "mloop",			                 mloop_main,			100,			1024,			1},
{         "mcont",			                 mcont_main,			100,			1024,			1},
{          "mohm",			                  mohm_main,			100,			1024,			1},
{         "mmohm",			                 mmohm_main,			100,			1024,			1},
{          "madj",			                  madj_main,			100,			512,			1},
{          "mrcd",			                  mrcd_main,			100,			1024,			1},
{        "mearth",			                mearth_main,			100,			1024,			1},
{          "msrd",			                  msrd_main,			100,			11000,			1},
{          "madc",			                  madc_main,			100,			512,			1},
{          "mdac",			                  mdac_main,			100,			512,			1},
{          "mpga",			                  mpga_main,			100,			512,			1},
{          "mcal",			                  mcal_main,			100,			512,			1},
{       "tstport",			               tstport_main,			100,			640,			1},
{       "tstslip",			               tstslip_main,			100,			640,			1},
{       "tstloop",			               tstloop_main,			100,			640,			1},
{         "mtest",			                 mtest_main,			100,			1024,			1}
};

const int bin_lst_size   = sizeof(_bin_lst)/sizeof(bin_t);
const bin_t* bin_lst = &_bin_lst[0];


/*===========================================
Implementation
=============================================*/

/*===========================================
End of Source mklepton.c
=============================================*/
