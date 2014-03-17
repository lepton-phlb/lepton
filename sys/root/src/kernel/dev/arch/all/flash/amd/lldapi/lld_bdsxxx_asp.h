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

DEVSTATUS lld_PasswordProtectionProgramOp
(
   ADDRESS base_addr,
   FLASHDATA pwd0,
   FLASHDATA pwd1,
   FLASHDATA pwd2,
   FLASHDATA pwd3
);

void lld_PasswordProtectionPasswordVerify
(
   ADDRESS base_addr,
   FLASHDATA *pd0,
   FLASHDATA *pd1,
   FLASHDATA *pd2,
   FLASHDATA *pd3
);

void lld_PasswordProtectionPasswordUnlock
(
   ADDRESS base_addr,
   FLASHDATA pd0,
   FLASHDATA pd1,
   FLASHDATA pd2,
   FLASHDATA pd3
);

FLASHDATA PPB_Status
(
   ADDRESS base_addr,
   ADDRESS offset
);

FLASHDATA PPB_Program
(
   ADDRESS base_addr,
   ADDRESS offset
);

FLASHDATA PPB_AllErase
(
   ADDRESS base_addr,
   ADDRESS offset
);

void PPB_LockBitSet
(
   ADDRESS base_addr
);

FLASHDATA PPB_LockBitStatus
(
   ADDRESS base_addr
);

void DYB_Write
(
   ADDRESS base_addr,
   ADDRESS offset
);

void DYB_Erase
(
   ADDRESS base_addr,
   ADDRESS offset
);

FLASHDATA DYB_Status
(
   ADDRESS base_addr,
   ADDRESS offset
);

FLASHDATA lld_PasswordProtectionModeLockingBitProgram
(
   ADDRESS base_addr
);

FLASHDATA lld_PersistentProtectionModeLockingBitProgram
(
   ADDRESS base_addr
);

FLASHDATA lld_PasswordProtectionModeLockingBitRead
(
   ADDRESS base_addr
);

FLASHDATA lld_PersistentProtectionModeLockingBitRead
(
   ADDRESS base_addr
);



