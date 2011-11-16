
DEVSTATUS lld_PasswordProtectionProgramOp
(
ADDRESS   base_addr,
FLASHDATA pwd0,
FLASHDATA pwd1,
FLASHDATA pwd2,
FLASHDATA pwd3
);

void lld_PasswordProtectionPasswordVerify
( 
ADDRESS   base_addr,
FLASHDATA *pd0,
FLASHDATA *pd1,
FLASHDATA *pd2,
FLASHDATA *pd3
);

void lld_PasswordProtectionPasswordUnlock
(
ADDRESS   base_addr,
FLASHDATA pd0,
FLASHDATA pd1,
FLASHDATA pd2,
FLASHDATA pd3
);

FLASHDATA PPB_Status
(
ADDRESS   base_addr,
ADDRESS   offset
);

FLASHDATA PPB_Program
(
ADDRESS   base_addr,
ADDRESS   offset
);

FLASHDATA PPB_AllErase
(
ADDRESS   base_addr,
ADDRESS   offset
);

void PPB_LockBitSet
(
ADDRESS   base_addr
);

FLASHDATA PPB_LockBitStatus
(
ADDRESS   base_addr
);

void DYB_Write
(
ADDRESS   base_addr,
ADDRESS   offset
);

void DYB_Erase
(
ADDRESS   base_addr,
ADDRESS   offset
);

FLASHDATA DYB_Status
(
ADDRESS   base_addr,
ADDRESS   offset
);

FLASHDATA lld_PasswordProtectionModeLockingBitProgram
(
ADDRESS   base_addr
);

FLASHDATA lld_PersistentProtectionModeLockingBitProgram
(
ADDRESS   base_addr
);

FLASHDATA lld_PasswordProtectionModeLockingBitRead
(
ADDRESS   base_addr
);

FLASHDATA lld_PersistentProtectionModeLockingBitRead
(
ADDRESS   base_addr
);



