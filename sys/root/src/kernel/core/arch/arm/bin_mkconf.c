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

//see "bin/tstfltk.c"
int tstfltk_main(int argc, char* argv[]);

//see "bin/test2.c"
int test2_main(int argc, char* argv[]);

//see "bin/test.c"
int test_main(int argc, char* argv[]);

//see "bin/net/ifconfig.c"
int ifconfig_main(int argc, char* argv[]);

//see "bin/net/telnetd.c"
int telnetd_main(int argc, char* argv[]);

//see "bin/net/mongoosed.c"
int mongoosed_main(int argc, char* argv[]);



/*===========================================
Global Declaration
=============================================*/
static const bin_t _bin_lst[]={
{         "initd",			                 initd_main,			10,			4096,			5},
{           "lsh",			                   lsh_main,			10,			4096,			5},
{         "mount",			                 mount_main,			10,			2048,			5},
{        "umount",			                umount_main,			10,			2048,			5},
{            "ls",			                    ls_main,			10,			4096,			5},
{            "ps",			                    ps_main,			10,			4096,			5},
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
{       "tstfltk",			               tstfltk_main,			10,			16384,			5},
{         "test2",			                 test2_main,			10,			1024,			5},
{          "test",			                  test_main,			10,			8192,			5},
{      "ifconfig",			              ifconfig_main,			10,			8192,			5},
{       "telnetd",			               telnetd_main,			10,			8192,			5},
{     "mongoosed",			             mongoosed_main,			10,			31744,			1}
};

const int bin_lst_size   = sizeof(_bin_lst)/sizeof(bin_t);
const bin_t* bin_lst = &_bin_lst[0];


/*===========================================
Implementation
=============================================*/

/*===========================================
| End of Source : bin_mkconf.c
=============================================*/
