/*-------------------------------------------
| Copyright(C) 2008 CHAUVIN-ARNOUX
---------------------------------------------
| Project:
| Project Manager:
| Source: mklepton.c
| Path:mkleptux
| Authors:
| Plateform:GNU/Linux
| Created:
| Revision/Date:	$Revision: 1.3 $ $Date: 2009-08-28 09:56:38 $
| Description:
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

//see "sbin/cat.c"
int cat_main(int argc, char* argv[]);

//see "sbin/df.c"
int df_main(int argc, char* argv[]);

//see "sbin/uname.c"
int uname_main(int argc, char* argv[]);

//see "bin/test.c"
int test_main(int argc, char* argv[]);

//see "bin/net/ifconfig.c"
int ifconfig_main(int argc, char* argv[]);

//see "bin/net/telnetd.c"
int telnetd_main(int argc, char* argv[]);



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
{           "cat",			                   cat_main,			10,			2048,			5},
{            "df",			                    df_main,			10,			2048,			5},
{         "uname",			                 uname_main,			10,			2048,			5},
{          "test",			                  test_main,			10,			1024,			5},
{      "ifconfig",			              ifconfig_main,			10,			2048,			5},
{       "telnetd",			               telnetd_main,			10,			2048,			5}
};

const int bin_lst_size   = sizeof(_bin_lst)/sizeof(bin_t);
const bin_t* bin_lst = &_bin_lst[0];


/*===========================================
Implementation
=============================================*/

/*===========================================
| End of Source : bin_mkconf.c
|--------------------------------------------
| Historic:
|--------------------------------------------
| Authors	| Date	| Comments
|--------------------------------------------
=============================================*/
