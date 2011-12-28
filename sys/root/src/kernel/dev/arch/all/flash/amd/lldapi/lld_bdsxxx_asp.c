
#include "lld.h"

/* Protection Addresses */
#define LLD_PUV7_OW_ADDR (0x001A)
#define LLD_PUV7_PL_ADDR (0x000A)
#define LLD_PUV7_SL_ADDR (0x0012)
#define LLD_PUV7_WP_ADDR (0x003A)

/*  Dynamic Protection Bit Commands */
#define LLD_PUV7_DYB_STATUS_CMD  ((LLD_DEV_MULTIPLIER) * (0x58))
#define LLD_PUV7_DYB_WRITE_CMD   ((LLD_DEV_MULTIPLIER) * (0x48))
#define LLD_PUV7_DYB_ERASE_CMD   ((LLD_DEV_MULTIPLIER) * (0x48))
#define LLD_PUV7_DYB_WRITE_DATA  ((LLD_DEV_MULTIPLIER) * (0x01))
#define LLD_PUV7_DYB_ERASE_DATA  ((LLD_DEV_MULTIPLIER) * (0x00))

/*  SecSi Sector Protect Commands */
#define LLD_PUV7_SECSI_SEC_LOCK_SETUP_CMD   ((LLD_DEV_MULTIPLIER) * (0x60))
#define LLD_PUV7_SECSI_SEC_LOCK_CMD         ((LLD_DEV_MULTIPLIER) * (0x68))
#define LLD_PUV7_SECSI_SEC_LOCK_PGM_VER_CMD ((LLD_DEV_MULTIPLIER) * (0x48))

/* Persistent Protection Bit Commands */
#define LLD_PUV7_PPB_SETUP_CMD        ((LLD_DEV_MULTIPLIER) * (0x60))
#define LLD_PUV7_PPB_PGM_CMD          ((LLD_DEV_MULTIPLIER) * (0x68))
#define LLD_PUV7_PPB_PGM_VER_CMD      ((LLD_DEV_MULTIPLIER) * (0x48))
#define LLD_PUV7_PPB_ERASE_CMD        ((LLD_DEV_MULTIPLIER) * (0x60))
#define LLD_PUV7_PPB_ERASE_VER_CMD    ((LLD_DEV_MULTIPLIER) * (0x40))

#define LLD_PUV7_PPB_LOCK_BIT_SET_CMD     ((LLD_DEV_MULTIPLIER) * (0x78))
#define LLD_PUV7_PPB_LOCK_BIT_STATUS_CMD  ((LLD_DEV_MULTIPLIER) * (0x58))

/* Password Protection Commands */
#define LLD_PUV7_PASSWD_PGM_CMD       ((LLD_DEV_MULTIPLIER) * (0x38))
#define LLD_PUV7_PASSWD_VERIFY_CMD    ((LLD_DEV_MULTIPLIER) * (0xC8))
#define LLD_PUV7_PASSWD_UNLOCK_CMD    ((LLD_DEV_MULTIPLIER) * (0x28))

#define PPB_PROTECTED     (1*LLD_DEV_MULTIPLIER)
#define PPB_UNPROTECTED   (0*LLD_DEV_MULTIPLIER)

#define	DQ0_BIT_MASK	(0x01)		/* select DQ0 */
#define	DQ1_BIT_MASK	(0x02)		/* select DQ1 */
#define	DQ2_BIT_MASK	(0x04)		/* select DQ2 */
#define	DQ5_BIT_MASK	(0x20)		/* select DQ5 */
#define	DQ6_BIT_MASK	(0x40)		/* select DQ6 */
#define	DQ7_BIT_MASK	(0x80)		/* select DQ7 */

#define LLD_DEV_DQ0_MASK	 ((LLD_DEV_MULTIPLIER) * (DQ0_BIT_MASK))
#define LLD_DEV_DQ1_MASK	 ((LLD_DEV_MULTIPLIER) * (DQ1_BIT_MASK))
#define LLD_DEV_DQ6_MASK	 ((LLD_DEV_MULTIPLIER) * (DQ6_BIT_MASK))
#define LLD_DEV_DQ7_MASK	 ((LLD_DEV_MULTIPLIER) * (DQ7_BIT_MASK))
/*
*
*
*
*
*/
DEVSTATUS lld_PasswordProtectionProgramOp
(
ADDRESS   base_addr,
FLASHDATA pwd0,
FLASHDATA pwd1,
FLASHDATA pwd2,
FLASHDATA pwd3
)
{
  FLASHDATA act_data_ptr;
  DEVSTATUS status;
  int attempts = 0;

  FLASH_WR(base_addr, LLD_UNLOCK_ADDR1, NOR_UNLOCK_DATA1);
  FLASH_WR(base_addr, LLD_UNLOCK_ADDR2, NOR_UNLOCK_DATA2);
  FLASH_WR(base_addr, LLD_UNLOCK_ADDR1, LLD_PUV7_PASSWD_PGM_CMD);
  FLASH_WR(base_addr, 0, pwd0); 
  status = lld_Poll(base_addr, 0, &pwd0, &act_data_ptr, LLD_P_POLL_PGM);
  if (status != DEV_NOT_BUSY)
  {
    FLASH_WR(base_addr, 0x00, NOR_RESET_CMD); /* go back to read array mode */
    return(status);
  }

  attempts = 0;
  FLASH_WR(base_addr, LLD_UNLOCK_ADDR1, NOR_UNLOCK_DATA1);       
  FLASH_WR(base_addr, LLD_UNLOCK_ADDR2, NOR_UNLOCK_DATA2);       
  FLASH_WR(base_addr, LLD_UNLOCK_ADDR1, LLD_PUV7_PASSWD_PGM_CMD);
  FLASH_WR(base_addr, 1, pwd1);
  status = lld_Poll(base_addr, 1, &pwd1, &act_data_ptr, LLD_P_POLL_PGM);
  if (status != DEV_NOT_BUSY)
  {
    FLASH_WR(base_addr, 0x00, NOR_RESET_CMD); /* go back to read array mode */
    return(status);
  }

  attempts = 0;
  FLASH_WR(base_addr, LLD_UNLOCK_ADDR1, NOR_UNLOCK_DATA1);       
  FLASH_WR(base_addr, LLD_UNLOCK_ADDR2, NOR_UNLOCK_DATA2);       
  FLASH_WR(base_addr, LLD_UNLOCK_ADDR1, LLD_PUV7_PASSWD_PGM_CMD);
  FLASH_WR(base_addr, 2, pwd2);
  status = lld_Poll(base_addr, 2, &pwd2, &act_data_ptr, LLD_P_POLL_PGM);
  if (status != DEV_NOT_BUSY)
  {
    FLASH_WR(base_addr, 0x00, NOR_RESET_CMD); /* go back to read array mode */
    return(status);
  }

  attempts = 0;
  FLASH_WR(base_addr, LLD_UNLOCK_ADDR1, NOR_UNLOCK_DATA1);       
  FLASH_WR(base_addr, LLD_UNLOCK_ADDR2, NOR_UNLOCK_DATA2);       
  FLASH_WR(base_addr, LLD_UNLOCK_ADDR1, LLD_PUV7_PASSWD_PGM_CMD);
  FLASH_WR(base_addr, 3, pwd3);
  status = lld_Poll(base_addr, 3, &pwd3, &act_data_ptr, LLD_P_POLL_PGM);

  FLASH_WR(base_addr, 0x00, NOR_RESET_CMD); /* go back to read array mode */
  return(status);
}

/*
*
*
*
*
*/
void lld_PasswordProtectionPasswordVerify
( 
ADDRESS   base_addr,
FLASHDATA *pd0,
FLASHDATA *pd1,
FLASHDATA *pd2,
FLASHDATA *pd3
)
{
  FLASH_WR(base_addr, LLD_UNLOCK_ADDR1, NOR_UNLOCK_DATA1);
  FLASH_WR(base_addr, LLD_UNLOCK_ADDR2, NOR_UNLOCK_DATA2);
  FLASH_WR(base_addr, LLD_UNLOCK_ADDR1, LLD_PUV7_PASSWD_VERIFY_CMD);
  *pd0 = FLASH_RD(base_addr, 0x0000);

  FLASH_WR(base_addr, LLD_UNLOCK_ADDR1, NOR_UNLOCK_DATA1);
  FLASH_WR(base_addr, LLD_UNLOCK_ADDR2, NOR_UNLOCK_DATA2);
  FLASH_WR(base_addr, LLD_UNLOCK_ADDR1, LLD_PUV7_PASSWD_VERIFY_CMD);
  *pd1 = FLASH_RD(base_addr, 0x0000);

  FLASH_WR(base_addr, LLD_UNLOCK_ADDR1, NOR_UNLOCK_DATA1);
  FLASH_WR(base_addr, LLD_UNLOCK_ADDR2, NOR_UNLOCK_DATA2);
  FLASH_WR(base_addr, LLD_UNLOCK_ADDR1, LLD_PUV7_PASSWD_VERIFY_CMD);
  *pd2 = FLASH_RD(base_addr, 0x0000);

  FLASH_WR(base_addr, LLD_UNLOCK_ADDR1, NOR_UNLOCK_DATA1);
  FLASH_WR(base_addr, LLD_UNLOCK_ADDR2, NOR_UNLOCK_DATA2);
  FLASH_WR(base_addr, LLD_UNLOCK_ADDR1, LLD_PUV7_PASSWD_VERIFY_CMD);
  *pd3 = FLASH_RD(base_addr, 0x0000);

  FLASH_WR(base_addr, 0x00, NOR_RESET_CMD); /* go back to read array mode */
}

/*
*
*
*
*
*/
void lld_PasswordProtectionPasswordUnlock
(
ADDRESS   base_addr,
FLASHDATA pd0,
FLASHDATA pd1,
FLASHDATA pd2,
FLASHDATA pd3
)
{
  FLASH_WR(base_addr, LLD_UNLOCK_ADDR1, NOR_UNLOCK_DATA1);           
  FLASH_WR(base_addr, LLD_UNLOCK_ADDR2, NOR_UNLOCK_DATA2);           
  FLASH_WR(base_addr, LLD_UNLOCK_ADDR1, LLD_PUV7_PASSWD_UNLOCK_CMD); 
  FLASH_WR(base_addr, 0, pd0);
  FLASH_WR(base_addr, 1, pd1);
  FLASH_WR(base_addr, 2, pd2);
  FLASH_WR(base_addr, 3, pd3);

  FLASH_WR(base_addr, 0x00, NOR_RESET_CMD); /* go back to read array mode */
}

/*
*
*
*
*
*/
FLASHDATA PPB_Status
(
ADDRESS   base_addr,
ADDRESS   offset
)
{
  FLASHDATA retval;

  /* Write command Sequence */
  FLASH_WR(base_addr, LLD_UNLOCK_ADDR1, NOR_UNLOCK_DATA1);             /* *(xxx555) = 0xAA */
  FLASH_WR(base_addr, LLD_UNLOCK_ADDR2, NOR_UNLOCK_DATA2);             /* *(xxx2AA) = 0x55 */
  FLASH_WR(base_addr, offset + LLD_UNLOCK_ADDR1, NOR_AUTOSELECT_CMD);  /* *(sax555) = 0x90 */
  retval = (FLASH_RD(base_addr, offset + 2) );                         /* read ppb */

  /* Exit Mode */
  FLASH_WR(base_addr, 0, NOR_RESET_CMD);
  return(retval);
}

/*
*
*
*
*
*/
FLASHDATA PPB_Program
(
ADDRESS   base_addr,
ADDRESS   offset
)
{
  FLASHDATA retval = PPB_UNPROTECTED;
  int plscnt = 0;

  /* if PPB already protected, don't over program it */
  if (PPB_Status(base_addr, offset) == PPB_PROTECTED)
  {
    return(PPB_PROTECTED);
  }

  /* Write command Sequence */
  FLASH_WR(base_addr, LLD_UNLOCK_ADDR1, NOR_UNLOCK_DATA1);       /* *(xxx555) = 0xAA */
  FLASH_WR(base_addr, LLD_UNLOCK_ADDR2, NOR_UNLOCK_DATA2);       /* *(xxx2AA) = 0x55 */
  FLASH_WR(base_addr, LLD_UNLOCK_ADDR1, LLD_PUV7_PPB_SETUP_CMD); /* *(xxx2AA) = 0x60 */

  while ((plscnt++ < 25) && (retval != PPB_PROTECTED) )
  {
    FLASH_WR(base_addr, offset + 2, LLD_PUV7_PPB_PGM_CMD);     /* *(sa+2) = 0x68  */
    DelayMicroseconds(150);                                    /* datasheet p. 22 */
    FLASH_WR(base_addr, offset + 2, LLD_PUV7_PPB_PGM_VER_CMD); /* *(sa+2) = 0x48  */
    retval  = (FLASH_RD(base_addr, offset + 2) );              /* return verify information */

  }

  /* Exit Mode */
  FLASH_WR(base_addr, offset + 2, NOR_RESET_CMD);

  return(retval);
}

/*
*
*
*
*
*/
FLASHDATA PPB_AllErase
(
ADDRESS   base_addr,
ADDRESS   offset
)
{
  FLASHDATA retval = PPB_PROTECTED;
  int plscnt = 0;

  /* pre-program the protect bits */
/*  for (i = 0; i < number_of_sector_groups; i++)
  {
    if ( lld_ppb(i) == PPB_UNPROTECTED)
    {
      while (lld_PpbProtectSector(i) != PPB_PROTECTED);
    }
    psg++;
  }
*/

  /* Write command Sequence */
  FLASH_WR(base_addr, LLD_UNLOCK_ADDR1, NOR_UNLOCK_DATA1);       /* *(xxx555) = 0xAA */
  FLASH_WR(base_addr, LLD_UNLOCK_ADDR2, NOR_UNLOCK_DATA2);       /* *(xxx2AA) = 0x55 */
  FLASH_WR(base_addr, LLD_UNLOCK_ADDR1, LLD_PUV7_PPB_SETUP_CMD); /* *(xxx2AA) = 0x60 */

  while ((plscnt++ < 25) && (retval != PPB_UNPROTECTED) )
  {
    FLASH_WR(base_addr, 2, LLD_PUV7_PPB_SETUP_CMD);              /* *(xxx002) = 0x60 */
    DelayMilliseconds(15);                                       /* datasheet p. 22 */

    FLASH_WR(base_addr, 2, LLD_PUV7_PPB_ERASE_VER_CMD);          /* *(xxx002) = 0x40 */
    retval  = (FLASH_RD(base_addr, 2) ); /* return verify information (if non-zero do lld_PpbUnProtectSector again) */

  }

  /* Exit Mode */
  FLASH_WR(base_addr, 0, NOR_RESET_CMD);
  return(retval);
}

/*
*
*
*
*
*/
void PPB_LockBitSet
(
ADDRESS   base_addr
)
{
  FLASH_WR(base_addr, LLD_UNLOCK_ADDR1, NOR_UNLOCK_DATA1);
  FLASH_WR(base_addr, LLD_UNLOCK_ADDR2, NOR_UNLOCK_DATA2);
  FLASH_WR(base_addr, LLD_UNLOCK_ADDR1, LLD_PUV7_PPB_LOCK_BIT_SET_CMD);
  FLASH_WR(base_addr, 0x00, NOR_RESET_CMD); /* go back to read array mode */
}

/*
*
*
*
*
*/
FLASHDATA PPB_LockBitStatus
(
ADDRESS   base_addr
)
{
  FLASHDATA retval;

  FLASH_WR(base_addr, LLD_UNLOCK_ADDR1, NOR_UNLOCK_DATA1);
  FLASH_WR(base_addr, LLD_UNLOCK_ADDR2, NOR_UNLOCK_DATA2);
  FLASH_WR(base_addr, LLD_UNLOCK_ADDR1, LLD_PUV7_PPB_LOCK_BIT_STATUS_CMD);
  retval = ( (FLASH_RD(base_addr, 0) & (LLD_DEV_DQ1_MASK)) >> 1 );
  FLASH_WR(base_addr, 0x00, NOR_RESET_CMD); /* go back to read array mode */

  return(retval);
}

/*
*
*
*
*
*/
void DYB_Write
(
ADDRESS   base_addr,
ADDRESS   offset
)
{
  /* Write command Sequence */
  FLASH_WR(base_addr, LLD_UNLOCK_ADDR1, NOR_UNLOCK_DATA1);
  FLASH_WR(base_addr, LLD_UNLOCK_ADDR2, NOR_UNLOCK_DATA2);
  FLASH_WR(base_addr, LLD_UNLOCK_ADDR1, LLD_PUV7_DYB_WRITE_CMD);
  FLASH_WR(base_addr, offset, LLD_PUV7_DYB_WRITE_DATA);

  /* Exit Mode */

  FLASH_WR(base_addr, LLD_UNLOCK_ADDR1, NOR_UNLOCK_DATA1);
  FLASH_WR(base_addr, LLD_UNLOCK_ADDR2, NOR_UNLOCK_DATA2);
  FLASH_WR(base_addr, offset + LLD_UNLOCK_ADDR1, NOR_AUTOSELECT_CMD);
  FLASH_WR(base_addr, 0, NOR_RESET_CMD);
}

/*
*
*
*
*
*/
void DYB_Erase
(
ADDRESS   base_addr,
ADDRESS   offset
)
{
  /* Write command Sequence */
  FLASH_WR(base_addr, LLD_UNLOCK_ADDR1, NOR_UNLOCK_DATA1);
  FLASH_WR(base_addr, LLD_UNLOCK_ADDR2, NOR_UNLOCK_DATA2);
  FLASH_WR(base_addr, LLD_UNLOCK_ADDR1, LLD_PUV7_DYB_ERASE_CMD);
  FLASH_WR(base_addr, offset, LLD_PUV7_DYB_WRITE_DATA);

  /* Exit Mode */

  FLASH_WR(base_addr, LLD_UNLOCK_ADDR1, NOR_UNLOCK_DATA1);
  FLASH_WR(base_addr, LLD_UNLOCK_ADDR2, NOR_UNLOCK_DATA2);
  FLASH_WR(base_addr, offset + LLD_UNLOCK_ADDR1, NOR_AUTOSELECT_CMD);
  FLASH_WR(base_addr, 0, NOR_RESET_CMD);
}

/*
*
*
*
*
*/
FLASHDATA DYB_Status
(
ADDRESS   base_addr,
ADDRESS   offset
)
{
  FLASHDATA data;

  /* Write command Sequence */
  FLASH_WR(base_addr, LLD_UNLOCK_ADDR1, NOR_UNLOCK_DATA1);
  FLASH_WR(base_addr, LLD_UNLOCK_ADDR2, NOR_UNLOCK_DATA2);
  FLASH_WR(base_addr, LLD_UNLOCK_ADDR1, LLD_PUV7_DYB_STATUS_CMD);
  data = FLASH_RD(base_addr, offset);

  /* Exit Mode */

  FLASH_WR(base_addr, LLD_UNLOCK_ADDR1, NOR_UNLOCK_DATA1);
  FLASH_WR(base_addr, LLD_UNLOCK_ADDR2, NOR_UNLOCK_DATA2);
  FLASH_WR(base_addr, offset + LLD_UNLOCK_ADDR1, NOR_AUTOSELECT_CMD);
  FLASH_WR(base_addr, 0, NOR_RESET_CMD);

  return(data);
}

/*
*
*
*
*
*/
FLASHDATA lld_PasswordProtectionModeLockingBitProgram
(
ADDRESS   base_addr
)
{
  FLASHDATA retval = 0;
  int plscnt = 0;

  FLASH_WR(base_addr, LLD_UNLOCK_ADDR1, NOR_UNLOCK_DATA1);                    /*  */
  FLASH_WR(base_addr, LLD_UNLOCK_ADDR2, NOR_UNLOCK_DATA2);                    /*  */
  FLASH_WR(base_addr, LLD_UNLOCK_ADDR1, LLD_PUV7_SECSI_SEC_LOCK_SETUP_CMD);   /*  */

  while ( (retval != LLD_DEV_DQ0_MASK) && (plscnt++ < 1000) )
  {
    FLASH_WR(base_addr, LLD_PUV7_PL_ADDR, LLD_PUV7_SECSI_SEC_LOCK_CMD);         /*  */
    DelayMicroseconds(150);
    FLASH_WR(base_addr, LLD_PUV7_PL_ADDR, LLD_PUV7_SECSI_SEC_LOCK_PGM_VER_CMD); /*  */
    retval = ( FLASH_RD(base_addr, LLD_PUV7_PL_ADDR) & LLD_DEV_DQ0_MASK );      /*  */
  }

  /* extra pulses for margin, recommended by factory */

  FLASH_WR(base_addr, LLD_PUV7_PL_ADDR, LLD_PUV7_SECSI_SEC_LOCK_CMD);         /*  */
  DelayMicroseconds(150);                                                                       /*  */
  FLASH_WR(base_addr, LLD_PUV7_PL_ADDR, LLD_PUV7_SECSI_SEC_LOCK_PGM_VER_CMD); /*  */
  retval = ( FLASH_RD(base_addr, LLD_PUV7_PL_ADDR) & LLD_DEV_DQ0_MASK );      /*  */

  FLASH_WR(base_addr, LLD_PUV7_PL_ADDR, LLD_PUV7_SECSI_SEC_LOCK_CMD);         /*  */
  DelayMicroseconds(150);                                                                       /*  */
  FLASH_WR(base_addr, LLD_PUV7_PL_ADDR, LLD_PUV7_SECSI_SEC_LOCK_PGM_VER_CMD); /*  */
  retval = ( FLASH_RD(base_addr, LLD_PUV7_PL_ADDR) & LLD_DEV_DQ0_MASK );      /*  */

  FLASH_WR(base_addr, LLD_PUV7_PL_ADDR, LLD_PUV7_SECSI_SEC_LOCK_CMD);         /*  */
  DelayMicroseconds(150);                                                                       /*  */
  FLASH_WR(base_addr, LLD_PUV7_PL_ADDR, LLD_PUV7_SECSI_SEC_LOCK_PGM_VER_CMD); /*  */
  retval = ( FLASH_RD(base_addr, LLD_PUV7_PL_ADDR) & LLD_DEV_DQ0_MASK );      /*  */

  FLASH_WR(base_addr, 0x00, NOR_RESET_CMD); /* go back to read array mode */

  return(retval);
}

/*
*
*
*
*
*/
FLASHDATA lld_PersistentProtectionModeLockingBitProgram
(
ADDRESS   base_addr
)
{
  FLASHDATA retval = 0;
  int plscnt = 0;

  FLASH_WR(base_addr, LLD_UNLOCK_ADDR1, NOR_UNLOCK_DATA1);                    /*  */
  FLASH_WR(base_addr, LLD_UNLOCK_ADDR2, NOR_UNLOCK_DATA2);                    /*  */
  FLASH_WR(base_addr, LLD_UNLOCK_ADDR1, LLD_PUV7_SECSI_SEC_LOCK_SETUP_CMD);   /*  */

  while ( (retval != LLD_DEV_DQ0_MASK) && (plscnt++ < 1000) )
  {
    FLASH_WR(base_addr, LLD_PUV7_SL_ADDR, LLD_PUV7_SECSI_SEC_LOCK_CMD);         /*  */
    DelayMicroseconds(150);
    FLASH_WR(base_addr, LLD_PUV7_SL_ADDR, LLD_PUV7_SECSI_SEC_LOCK_PGM_VER_CMD); /*  */
    retval = ( FLASH_RD(base_addr, LLD_PUV7_SL_ADDR) & LLD_DEV_DQ0_MASK );      /*  */
  }

  /* extra pulses for margin, recommended by factory */

  FLASH_WR(base_addr, LLD_PUV7_SL_ADDR, LLD_PUV7_SECSI_SEC_LOCK_CMD);         /*  */
  DelayMicroseconds(150);
  FLASH_WR(base_addr, LLD_PUV7_SL_ADDR, LLD_PUV7_SECSI_SEC_LOCK_PGM_VER_CMD); /*  */
  retval = ( FLASH_RD(base_addr, LLD_PUV7_SL_ADDR) & LLD_DEV_DQ0_MASK );      /*  */

  FLASH_WR(base_addr, LLD_PUV7_SL_ADDR, LLD_PUV7_SECSI_SEC_LOCK_CMD);         /*  */
  DelayMicroseconds(150);
  FLASH_WR(base_addr, LLD_PUV7_SL_ADDR, LLD_PUV7_SECSI_SEC_LOCK_PGM_VER_CMD); /*  */
  retval = ( FLASH_RD(base_addr, LLD_PUV7_SL_ADDR) & LLD_DEV_DQ0_MASK );      /*  */

  FLASH_WR(base_addr, LLD_PUV7_SL_ADDR, LLD_PUV7_SECSI_SEC_LOCK_CMD);         /*  */
  DelayMicroseconds(150);
  FLASH_WR(base_addr, LLD_PUV7_SL_ADDR, LLD_PUV7_SECSI_SEC_LOCK_PGM_VER_CMD); /*  */
  retval = ( FLASH_RD(base_addr, LLD_PUV7_SL_ADDR) & LLD_DEV_DQ0_MASK );      /*  */

  FLASH_WR(base_addr, 0x00, NOR_RESET_CMD); /* go back to read array mode */

  return(retval);
}

/*
*
*
*
*
*/
FLASHDATA lld_PasswordProtectionModeLockingBitRead
(
ADDRESS   base_addr
)
{
  FLASHDATA retval;
/* THIS FUNCTION DEVIATES FROM DOCUMENTATION, BUT FACTORY SPECIFIED - JCH */
/* DOCUMENTED METHOD DID NOT WORK */
  FLASH_WR(base_addr, LLD_UNLOCK_ADDR1, NOR_UNLOCK_DATA1);                 
  FLASH_WR(base_addr, LLD_UNLOCK_ADDR2, NOR_UNLOCK_DATA2);                 
  FLASH_WR(base_addr, LLD_UNLOCK_ADDR1, LLD_PUV7_SECSI_SEC_LOCK_SETUP_CMD);
  FLASH_WR(base_addr, LLD_PUV7_PL_ADDR, LLD_PUV7_PPB_PGM_VER_CMD);         
  DelayMicroseconds(250);                                                          
  retval = ( FLASH_RD(base_addr, LLD_PUV7_PL_ADDR) & LLD_DEV_DQ0_MASK );   
  FLASH_WR(base_addr, 0x00, NOR_RESET_CMD); /* go back to read array mode */

  return(retval);
}

/*
*
*
*
*
*/
FLASHDATA lld_PersistentProtectionModeLockingBitRead
(
ADDRESS   base_addr
)
{
  FLASHDATA retval = 0;
/* THIS FUNCTION DEVIATES FROM DOCUMENTATION, BUT FACTORY SPECIFIED - JCH */
/* DOCUMENTED METHOD DID NOT WORK */
  FLASH_WR(base_addr, LLD_UNLOCK_ADDR1, NOR_UNLOCK_DATA1);                 
  FLASH_WR(base_addr, LLD_UNLOCK_ADDR2, NOR_UNLOCK_DATA2);                 
  FLASH_WR(base_addr, LLD_UNLOCK_ADDR1, LLD_PUV7_SECSI_SEC_LOCK_SETUP_CMD);
  FLASH_WR(base_addr, LLD_PUV7_SL_ADDR, LLD_PUV7_PPB_PGM_VER_CMD); /*  */
  DelayMicroseconds(250);                                                          
  retval = ( FLASH_RD(base_addr, LLD_PUV7_SL_ADDR) & LLD_DEV_DQ0_MASK );   
  FLASH_WR(base_addr, 0x00, NOR_RESET_CMD); /* go back to read array mode */

  return(retval);
}



