/*-------------------------------------------
| Copyright(C) 2007 CHAUVIN-ARNOUX
---------------------------------------------
| Project:
| Project Manager:
| Source: mklepton.c
| Path: X:\TOOLS\mklepton
| Authors:
| Plateform:
| Created:
| Revision/Date:
| Description:
---------------------------------------------
| Historic:
---------------------------------------------
| Authors	| Date	| Comments
---------------------------------------------*/


/*===========================================
Includes
=============================================*/
#include "kernel/core/bin.h"
//see "sbin/initd.c"
int initd_main(int argc, char* argv[]);

//see "sbin/lsh.c"
int lsh_main(int argc, char* argv[]);

//see "sbin/mount.c"
int mount_main(int argc, char* argv[]);

//see "sbin/umount.c"
int umount_main(int argc, char* argv[]);

//see "sbin/ls.c"
int ls_main(int argc, char* argv[]);

//see "sbin/ps.c"
int ps_main(int argc, char* argv[]);

//see "sbin/kill.c"
int kill_main(int argc, char* argv[]);

//see "sbin/touch.c"
int touch_main(int argc, char* argv[]);

//see "sbin/cat.c"
int cat_main(int argc, char* argv[]);

//see "sbin/more.c"
int more_main(int argc, char* argv[]);

//see "sbin/mkfs.c"
int mkfs_main(int argc, char* argv[]);

//see "sbin/df.c"
int df_main(int argc, char* argv[]);

//see "sbin/uname.c"
int uname_main(int argc, char* argv[]);

//see "sbin/pwd.c"
int pwd_main(int argc, char* argv[]);

//see "sbin/rmdir.c"
int rmdir_main(int argc, char* argv[]);

//see "sbin/mkdir.c"
int mkdir_main(int argc, char* argv[]);

//see "sbin/rm.c"
int rm_main(int argc, char* argv[]);

//see "sbin/od.c"
int od_main(int argc, char* argv[]);

//see "sbin/date.c"
int date_main(int argc, char* argv[]);

//see "sbin/echo.c"
int echo_main(int argc, char* argv[]);

//see "sbin/stty.c"
int stty_main(int argc, char* argv[]);

//see "sbin/net/ifconfig.c"
int ifconfig_main(int argc, char* argv[]);

//see "bin/dhrystone.c"
int dhrystone_main(int argc, char* argv[]);

//see "bin/free.c"
int free_main(int argc, char* argv[]);

//see "bin/udpsrvd.c"
int udpsrvd_main(int argc, char* argv[]);



/*===========================================
Global Declaration
=============================================*/
static const bin_t _bin_lst[]={
{         "initd",			                 initd_main,			10,			2048,			5},
{           "lsh",			                   lsh_main,			10,			2048,			5},
{         "mount",			                 mount_main,			10,			2048,			5},
{        "umount",			                umount_main,			10,			2048,			5},
{            "ls",			                    ls_main,			10,			2048,			5},
{            "ps",			                    ps_main,			10,			2048,			5},
{          "kill",			                  kill_main,			10,			2048,			5},
{         "touch",			                 touch_main,			10,			2048,			5},
{           "cat",			                   cat_main,			10,			2048,			5},
{          "more",			                  more_main,			10,			2048,			5},
{          "mkfs",			                  mkfs_main,			10,			2048,			5},
{            "df",			                    df_main,			10,			2048,			5},
{         "uname",			                 uname_main,			10,			2048,			5},
{           "pwd",			                   pwd_main,			10,			2048,			5},
{         "rmdir",			                 rmdir_main,			10,			2048,			5},
{         "mkdir",			                 mkdir_main,			10,			2048,			5},
{            "rm",			                    rm_main,			10,			2048,			5},
{            "od",			                    od_main,			10,			2048,			5},
{          "date",			                  date_main,			10,			2048,			5},
{          "echo",			                  echo_main,			10,			2048,			5},
{          "stty",			                  stty_main,			10,			2048,			5},
{      "ifconfig",			              ifconfig_main,			100,			4096,			1},
{     "dhrystone",			             dhrystone_main,			10,			1024,			5},
{          "free",			                  free_main,			2,			1024,			5},
{       "udpsrvd",			               udpsrvd_main,			100,			2048,			5}
};

const int bin_lst_size   = sizeof(_bin_lst)/sizeof(bin_t);
const bin_t* bin_lst = &_bin_lst[0];


/*===========================================
Implementation
=============================================*/

/*===========================================
End of Source mklepton.c
=============================================*/
