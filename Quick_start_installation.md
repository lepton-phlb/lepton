# Package requirements #
  * Debian 32b bits environment
  * build-essential libpng-dev libjpeg-dev libtiff-dev tcl8.4-dev tk8.4-dev
  * mercurial scons libexpat-dev libgtk2.0-dev minicom gdb
  * kvm user for this installation

# eCos installation #
## eCos core tools ##
Create install and tmp build directories:

`$ mkdir -p ~/dev/ecos-v3.0/{archives,ecos-tools,ecos-build}`

Get ecos revision e1cec8d66444 archive **test needed for current version** from http://hg-pub.ecoscentric.com and unpack it

`$ wget http://hg-pub.ecoscentric.com/ecos/archive/e1cec8d66444.tar.bz2 -P archives`

`$ tar jxf archives/e1cec8d66444.tar.bz2`


`$ ln -s ~/dev/ecos-v3.0/ecos-e1cec8d66444 ~/dev/ecos-v3.0/ecos`

Compile command line tool **ecosconfig**

`$ cd ecos-build`

`$ ../ecos/host/configure --prefix=/home/kvm/dev/ecos-v3.0/ecos-tools --with-tcl=/usr --with-tcl-version=8.4`

`$ make; make install`

Change right of tool ecosadmin.tcl

`$ cd ..`

`$ chmod +x ecos/packages/ecosadmin.tcl`

Grab graphical user interface tool **configtool**

`$ cd archives`

`$ wget [ftp://ftp.mirrorservice.org/sites/sources.redhat.com/pub/ecos/\](ftp://ftp.mirrorservice.org/sites/sources.redhat.com/pub/ecos/\)
anoncvs/ecos-tools-bin-110209.i386linux.tar.bz2`

`$ cd ..`

`$ tar jxf archives/ecos-tools-bin-110209.i386linux.tar.bz2`

`$ mv configtool ecos-tools/bin/`

`$ rm -f ecosconfig ser_filter platforms.tar`

Open graphical tool

`$ ./ecos-tools/bin/configtool`

and fill dialog box with this _/home/kvm/dev/ecos-v3.0/ecos/packages_

To complete installation for future use, provide ECOS\_REPOSITORY environment variable and complete PATH environment variable. For example :

`$ export ECOS_REPOSITORY="/home/kvm/dev/ecos-v3.0/ecos/packages"`

`$ export PATH="$PATH:/home/kvm/dev/ecos-v3.0/ecos-tools/bin"`


## Pre-built toolchains ##

Grab pre-compiled toolchains for ARM target (**arm-eabi**) and i386 target (**i386-elf**)

`$ mkdir -p ~/dev/toolchains`

`$ cd ~/dev/toolchains`

`$ wget --passive-ftp ftp://ecos.sourceware.org/pub/ecos/ecos-install.tcl`

`$ tclsh ecos-install.tcl -t`

Complete PATH environment variable with toolchain binaries. For example,

`$ export PATH="$PATH:/home/kvm/dev/toolchains/gnutools/arm-eabi/bin"`

`$ export PATH="$PATH:/home/kvm/dev/toolchains/gnutools/i386-elf/bin"`

**eCos is ready to use**

# Lepton #
## Lepton setup ##

Get Lepton/Tauon sources from repository and make little setup

`$ mkdir -p ~/dev/tauon`

`$ cd ~/dev/tauon`

`$ hg clone https://code.google.com/p/lepton/`

`$ cd lepton`

`$ sh tauon_ln.sh (create a symlink in ~)`

`$ cd ~/tauon`

Compile tauon\_sample application for targets (**AT91SAM9261-EK**, **TWR-K60N512**) and simulation

`$ cd ~/tauon/tools/config`

`$ scons BUILD_MKLEPTON=True BUILD_VIRTUALCPU=True BUILD_SAMPLEAPP=True`

Wait ...

## Debug for simulation ##

Install simulation environment

`$ sh ~/tauon/tools/host/debian/scripts/build_fifo.sh`

Copy minicom configuration file for simulation

`# cp /home/kvm/tauon/sys/root/prj/config/minicom/minirc.tauonpt \
/etc/minicom/`

Launch a debug session

`$ cd ~/tauon/sys/user/tauon_sampleapp/bin/`

`$ gdb -x ../etc/scripts/gdb_script_synth.gdb tauon_synthetic.elf`

At **gdb** prompt enter run (or r)

Open an **other** terminal

`$ minicom -o tauonpt`

At Lepton/Tauon prompt enjoy!!

lepton#2$ ps

PID PPID PGID     STIME   COMMAND

1    0    1  09:54:59   initd

2    1    1  09:54:59   /usr/sbin/lsh

4    2    1  09:55:27   ps

lepton#2$ ls /usr/sbin

initd  lsh  mount  umount

ls  ps  kill  touch

cat  more  mkfs  df

uname  pwd  rmdir  mkdir

rm  od  date  echo

total 20


lepton#2$ uname -a

lepton-gnu32-32  4.0.0.0 Oct  9 2012-20:50:01


**If you want networking, you need to adapt the script
_~/tauon/tools/host/debian/scripts/start\_br.sh** and launch gdb and
minicom with root privilege (or sudo). We use a tun/tap and a bridge
to provide a dummy ethernet card for simulation so special privileges
are required. 

## Debug for AT91SAM9261-EK target ##

You need a JTAG probe (Zy1000, J-Link, Amontec JTAG Tiny, ..) supported
by OpenOCD (http://openocd.sourceforge.net/).

Launch openocd with jlink\_at91sam9261.cfg if openocd runs on your host
machine.

`$ cd ~/tauon/sys/user/tauon_sampleapp/etc/scripts/`

`$ openocd -f jlink_at91sam9261.cfg`

Open a new terminal and launch a debug session

`$ arm-eabi-gdb -x ../etc/scripts/gdb\_script\_at91sam9261\_sdram\_jlink.gdb \
tauon\_at91sam9261.elf`

Branch a serial cable on Serial Debug Port (J15)

Open a new terminal

Configure minicom with **9600 8N1** (8 bits, no parity, 1 stop bit)

Back to gdb terminal and enter continue (or c)

Back to minicom terminal and enjoy!!!