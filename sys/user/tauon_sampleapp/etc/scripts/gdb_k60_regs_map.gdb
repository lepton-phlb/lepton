# file for peripherals registers
# define SDHC memory map
set variable $SDHC_DSADDR=0x400b1000
set variable $SDHC_BLKATTR=0x400b1004
set variable $SDHC_CMDARG=0x400b1008
set variable $SDHC_XFERTYP=0x400b100c
set variable $SDHC_CMDRSP0=0x400b1010
set variable $SDHC_CMDRSP1=0x400b1014
set variable $SDHC_CMDRSP2=0x400b1018
set variable $SDHC_CMDRSP3=0x400b101c
set variable $SDHC_DATPORT=0x400b1020
set variable $SDHC_PRSSTAT=0x400b1024
set variable $SDHC_PROCTL=0x400b1028
set variable $SDHC_SYSCTL=0x400b102c
set variable $SDHC_IRQSTAT=0x400b1030
set variable $SDHC_IRQSTATEN=0x400b1034
set variable $SDHC_IRQSIGEN=0x400b1038
set variable $SDHC_AC12ERR=0x400b103c
set variable $SDHC_HTCAPVLT=0x400b1040
set variable $SDHC_WML=0x400b1044
set variable $SDHC_FEVT=0x400b1050
set variable $SDHC_ADMAES=0x400b1054
set variable $SDHC_ADSADDR=0x400b1058
set variable $SDHC_VENDOR=0x400b10c0
set variable $SDHC_MMCBOOT=0x400b10c4
set variable $SDHC_HOSTVER=0x400b10c8

#display SDHC memory map
define print_k60_sdhc
   printf "[w] SDHC_DSADDR : 0x%x\n", *$SDHC_DSADDR
   printf "[w] SDHC_BLKATTR : 0x%x\n", *$SDHC_BLKATTR
   printf "[w] SDHC_CMDARG : 0x%x\n", *$SDHC_CMDARG
   printf "[w] SDHC_XFERTYP : 0x%x\n", *$SDHC_XFERTYP
   printf " SDHC_CMDRSP0 : 0x%x\n", *$SDHC_CMDRSP0
   printf " SDHC_CMDRSP1 : 0x%x\n", *$SDHC_CMDRSP1
   printf " SDHC_CMDRSP2 : 0x%x\n", *$SDHC_CMDRSP2
   printf " SDHC_CMDRSP3 : 0x%x\n", *$SDHC_CMDRSP3
   printf "[w] SDHC_DATPORT : 0x%x\n", *$SDHC_DATPORT
   printf "SDHC_PRSSTAT : 0x%x\n", *$SDHC_PRSSTAT
   printf "[w] SDHC_PROCTL : 0x%x (0x20)\n", *$SDHC_PROCTL
   printf "[w] SDHC_SYSCTL : 0x%x (0x8008)\n", *$SDHC_SYSCTL
   printf "[w] SDHC_IRQSTAT : 0x%x\n", *$SDHC_IRQSTAT
   printf "[w] SDHC_IRQSTATEN : 0x%x (0x117f013f)\n", *$SDHC_IRQSTATEN
   printf "[w] SDHC_IRQSIGEN : 0x%x\n", *$SDHC_IRQSIGEN
   printf " SDHC_AC12ERR : 0x%x\n", *$SDHC_AC12ERR
   printf " SDHC_HTCAPVLT : 0x%x\n", *$SDHC_HTCAPVLT
   printf "[w] SDHC_WML : 0x%x (0x100010)\n", *$SDHC_WML
   printf " SDHC_ADMAES : 0x%x\n", *$SDHC_ADMAES
   printf "[w] SDHC_ADSADDR : 0x%x\n", *$SDHC_ADSADDR
   printf "[w] SDHC_VENDOR : 0x%x (0x1)\n", *$SDHC_VENDOR
   printf "[w] SDHC_MMCBOOT : 0x%x\n", *$SDHC_MMCBOOT
end 

#document SDHC memory map
document print_k60_sdhc
   Show Kinetis K60 SDHC registers
end 

###
# define SPI2 memory map
set variable $SPI2_MCR=0x400ac000
set variable $SPI2_TCR=0x400ac008
set variable $SPI2_CTAR0=0x400ac00c
set variable $SPI2_CTAR1=0x400ac010
set variable $SPI2_SR=0x400ac02c
set variable $SPI2_RSER=0x400ac030
set variable $SPI2_PUSHR=0x400ac034
set variable $SPI2_POPR=0x400ac038
set variable $SPI2_TXFR0=0x400ac03c
set variable $SPI2_TXFR1=0x400ac040
set variable $SPI2_TXFR2=0x400ac044
set variable $SPI2_TXFR3=0x400ac048
set variable $SPI2_RXFR0=0x400ac07c
set variable $SPI2_RXFR1=0x400ac080
set variable $SPI2_RXFR2=0x400ac084
set variable $SPI2_RXFR3=0x400ac088

#display SPI2 memory map
define print_k60_spi2
   printf "SPI2_MCR : 0x%x\n", *$SPI2_MCR
   printf "SPI2_TCR : 0x%x\n", *$SPI2_TCR
   printf "SPI2_CTAR0 : 0x%x\n", *$SPI2_CTAR0
   printf "SPI2_CTAR1 : 0x%x\n", *$SPI2_CTAR1
   printf "SPI2_SR : 0x%x\n", *$SPI2_SR
   printf "SPI2_RSER : 0x%x\n", *$SPI2_RSER
   printf "SPI2_PUSHR : 0x%x\n", *$SPI2_PUSHR
   printf "SPI2_POPR : 0x%x\n", *$SPI2_POPR
   printf "SPI2_TXFR0 : 0x%x\n", *$SPI2_TXFR0
   printf "SPI2_TXFR1 : 0x%x\n", *$SPI2_TXFR1
   printf "SPI2_TXFR2 : 0x%x\n", *$SPI2_TXFR2
   printf "SPI2_TXFR3 : 0x%x\n", *$SPI2_TXFR3
   printf "SPI2_RXFR0 : 0x%x\n", *$SPI2_RXFR0
   printf "SPI2_RXFR1 : 0x%x\n", *$SPI2_RXFR1
   printf "SPI2_RXFR2 : 0x%x\n", *$SPI2_RXFR2
   printf "SPI2_RXFR3 : 0x%x\n", *$SPI2_RXFR3
end 

#document SPI2 memory map
document print_k60_spi2
   Show Kinetis K60 SPI2 registers
end 

###
# define MCG memory map
set variable $MCG_C1=0x40064000
set variable $MCG_C2=0x40064001
set variable $MCG_C3=0x40064002
set variable $MCG_C4=0x40064003
set variable $MCG_C5=0x40064004
set variable $MCG_C6=0x40064005
set variable $MCG_S=0x40064006
set variable $MCG_ATC=0x40064008
set variable $MCG_ATCVH=0x4006400a
set variable $MCG_ATCVL=0x4006400b

#display MCG memory map
define print_k60_mcg
   printf "MCG_C1 : 0x%x\n", *$MCG_C1
   printf "MCG_C2 : 0x%x\n", *$MCG_C2
   printf "MCG_C3 : 0x%x\n", *$MCG_C3
   printf "MCG_C4 : 0x%x\n", *$MCG_C4
   printf "MCG_C5 : 0x%x\n", *$MCG_C5
   printf "MCG_C6 : 0x%x\n", *$MCG_C6
   printf "MCG_S : 0x%x\n", *$MCG_S
   printf "MCG_ATC : 0x%x\n", *$MCG_ATC
   printf "MCG_ATCVH : 0x%x\n", *$MCG_ATCVH
   printf "MCG_ATCVL : 0x%x\n", *$MCG_ATCVL
end 

#document MCG memory map
document print_k60_mcg
   Show Kinetis K60 MCG registers
end 

###
# define RTC memory map
set variable $RTC_TSR=0x4003d000
set variable $RTC_TPR=0x4003d004
set variable $RTC_TAR=0x4003d008
set variable $RTC_TCR=0x4003d00c
set variable $RTC_CR=0x4003d010
set variable $RTC_SR=0x4003d014
set variable $RTC_LR=0x4003d018
set variable $RTC_IER=0x4003d01c
set variable $RTC_WAR=0x4003d800
set variable $RTC_RAR=0x4003d804

#display RTC memory map
define print_k60_rtc
   printf "RTC_TSR : 0x%x\n", *$RTC_TSR
   printf "RTC_TPR : 0x%x\n", *$RTC_TPR
   printf "RTC_TAR : 0x%x\n", *$RTC_TAR
   printf "RTC_TCR : 0x%x\n", *$RTC_TCR
   printf "RTC_CR : 0x%x\n", *$RTC_CR
   printf "RTC_SR : 0x%x\n", *$RTC_SR
   printf "RTC_LR : 0x%x\n", *$RTC_LR
   printf "RTC_IER : 0x%x\n", *$RTC_IER
   printf "RTC_WAR : 0x%x\n", *$RTC_WAR
   printf "RTC_RAR : 0x%x\n", *$RTC_RAR
end 

#document MCG memory map
document print_k60_rtc
   Show Kinetis K60 RTC registers
end

###
# define FTM0 memory map
set variable $FTM0_SC=0x40038000
set variable $FTM0_CNT=0x40038004
set variable $FTM0_MOD=0x40038008
set variable $FTM0_CNTIN=0x4003804c
set variable $FTM0_MODE=0x40038054

#display SDHC memory map
define print_k60_ftm0
   printf "[w] FTM0_SC : 0x%x\n", *$FTM0_SC
   printf "[w] FTM0_CNT : 0x%x\n", *$FTM0_CNT
   printf "[w] FTM0_MOD : 0x%x\n", *$FTM0_MOD
   printf "[w] FTM0_CNTIN : 0x%x\n", *$FTM0_CNTIN
   printf "[w] FTM0_MODE : 0x%x\n", *$FTM0_MODE
end 

#document FTM0 memory map
document print_k60_ftm0
   Show Kinetis K60 FTM 0 registers
end 
