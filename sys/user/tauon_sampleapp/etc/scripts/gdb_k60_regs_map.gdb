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
