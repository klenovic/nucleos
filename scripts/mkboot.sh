#!/bin/sh
##############################################################################
#  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
#
#  This file is part of Nucleos kernel.
#
#  Nucleos kernel is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, version 2 of the License.
##############################################################################
#
# mkboot.sh - make boot floppy, make root device bootable, etc.
#
scripts=../scripts
bin_bootblock=
bin_boot=
bin_image=

installboot=tools/installboot
edparams=tools/edparams
mkfs=tools/mkfs
partition=tools/partition

# indicates whether option needs loop device
need_loop_device=
# used loop device
loop_device=
# loop offset
loop_offset=
# defualt loop offset
def_loop_offset=0
# skip bytes at start of output
seek_bytes=512
# temporary directory
tmp_dir=.$$_mnt

usage()
{
  echo "Usage: `basename $0` [-h] [-a action] [-b boot] [-B bootblock] [-m masterboot] [-o offset] [-i sysimg] diskimg"
  echo "[-a action]: action to do (bootable,cdfdboot,fdboot,hddboot,install,installmbr)"
  echo "[-b boot]: install boot utility on floppy/disk image"
  echo "[-B bootblock]: install bootblock on floppy/disk image"
  echo "[-i image]: install system image on diskimg (type of diskimg needed)"
  echo "[-m masterboot]: install MBR on floppy/disk image"
  echo "[-o offset]: setup loop device with specified offset"
  echo "[-h]: this help"
}

setup_loop_device()
{
  if ( [ -z $diskimg ] || [ ! -e $diskimg ] ); then
    echo "Disk image '$diskimg' doesn't exist!"
    exit 1
  fi

  # get first free loop device and setup
  loop_device=`sudo /sbin/losetup -f`
  echo -n "==> Setup loop device: $loop_device..."

  sudo /sbin/losetup $loop_device $diskimg -o $loop_offset
  exitcode=$?

  if [ ! $exitcode = 0 ]; then
    echo "Can't setup loop device!"
    cleanup_exit 1
  fi

  echo "done"
}

release_loop_device()
{
  echo -n "==> Release $loop_device device..."
  sudo /sbin/losetup -d $loop_device
  echo "done"
}

# first parameter is required return code
cleanup_exit()
{
  # umount if mounted
  if [ -d $tmp_dir ]; then
    sudo umount -f $tmp_dir
    rm -rf $tmp_dir
  fi

  # release loop device if needed
  if [ $need_loop_device ]; then
    if ! [ -z $loop_device ]; then
      release_loop_device
    fi
  fi

  exit $1
}

# Check arguments.
while getopts "a:b:B:hi:m:o:" OPT "$@"
do
  case "$OPT" in
    a) # specifies action to do
       action=$OPTARG
       # some specific settings
       case "$action" in
         bootable)
           need_loop_device="y"
           ;;

         cdfdboot)
           need_loop_device="y"
           ;;

         fdboot)
           need_loop_device="y"
           ;;

         hddboot)
           need_loop_device="y"
           ;;

         install) # just install specified binaries (bootblock,boot,image)
                  # Note: assumes filesystem present
           need_loop_device="y"
           ;;

         installmbr) # Install MBR
           need_loop_device="y"
           ;;

         *)
           echo "Unknown action '$OPTARG'!"
           exit 1
       esac
       ;;

    b) # specifies boot binary
       need_loop_device="y"
       bin_boot=$OPTARG

       if ([ -z $bin_boot ] || [ ! -e $bin_boot ]); then
         echo "No such file '$bin_boot'!"
         exit 1
       fi
       ;;

    B) # specifies bootblock binary
       need_loop_device="y"
       bin_bootblock=$OPTARG

       if ([ -z $bin_bootblock ] || [ ! -e $bin_bootblock ]); then
         echo "No such file '$bin_bootblock'!"
         exit 1
       fi
       ;;

    i) # specifies kernel image
       need_loop_device="y"
       bin_image=$OPTARG

       if ([ -z $bin_image ] || [ ! -e $bin_image ]); then
         echo "No such file '$bin_image'!"
         exit 1
       fi
       ;;

    m) # specifies MBR binary
       need_loop_device="y"
       bin_masterboot=$OPTARG

       if ([ -z $bin_masterboot ] || [ ! -e $bin_masterboot ]); then
         echo "No such file '$bin_masterboot'!"
         exit 1
       fi

       install_masterboot="y"
       ;;

    o) # specifies offset for loop device
       if [ -z $OPTARG ]; then
         echo "Missing argument!"
         exit 1
       fi

       if ! [ $OPTARG -eq $OPTARG 2>/dev/null ]; then
         echo "Wrong value '$OPTARG'!"
         exit 1
       fi

       loop_offset=$OPTARG
       ;;

    h) usage
       exit 0
       ;;

    *) usage
       exit 1
       ;;
  esac
done

# if the offset was not specified then use default
if [ -z $loop_offset ]; then
  loop_offset=$def_loop_offset
fi

# Last one is disk/floppy image
shift `echo $OPTIND-1 | bc`
diskimg="$1"

if ([ -z $diskimg ] || [ ! -e $diskimg ]); then
  echo "Disk image not specified!"
  exit 1
fi

# Setup a loop device if needed
if [ $need_loop_device ]; then
  setup_loop_device
  dev=$loop_device
fi

case $action in
  bootable)
    # install boot monitor and boot block
    # Note: assumes filesystem present
    if [ -z $dev ]; then
      echo "Wrong device!"
      cleanup_exit 1
    fi

    if ([ -z $bin_boot ] || [ ! -e $bin_boot ]); then
      echo "No such file '$bin_boot'!"
      cleanup_exit 1
    fi

    if ([ -z $bin_bootblock ] || ! [ -e $bin_bootblock ]); then
      echo "No such file '$bin_bootblock'!"
      cleanup_exit 1
    fi

    mkdir $tmp_dir || cleanup_exit 1

    sudo mount -t minix $dev $tmp_dir || cleanup_exit 1

    # Install the boot monitor on the root device and make it bootable.
    echo "==> Install boot monitor '$bin_boot'"
    sudo cp -p $bin_boot $tmp_dir/boot/boot || cleanup_exit 1

    sudo umount $tmp_dir || cleanup_exit 1
    rm -rf $tmp_dir

    sudo $installboot -d $dev -B $bin_bootblock -b /boot/boot -p $bin_boot || cleanup_exit 1
    ;;

  install)
    # Install specified binaries on the root device.
    # Note: assumes filesystem present until with `-m masterboot'
    if [ -z $dev ]; then
      echo "Wrong device!"
      cleanup_exit 1
    fi

    if ([ -z $bin_image ] || [ ! -e $bin_image ]); then
      echo "No such file '$bin_image'!"
      cleanup_exit 1
    fi

    if ([ -z $bin_boot ] || [ ! -e $bin_boot ]); then
      echo "No such file '$bin_boot'!"
      cleanup_exit 1
    fi

    if ([ -z $bin_bootblock ] || [ ! -e $bin_bootblock ]); then
      echo "No such file '$bin_bootblock'!"
      cleanup_exit 1
    fi

    mkdir $tmp_dir || cleanup_exit 1
    sudo mount -t minix $dev $tmp_dir || cleanup_exit 1

    sudo mkdir -p $tmp_dir/boot
    sudo mkdir -p $tmp_dir/boot/image

    # Install the boot monitor on the root device and make it bootable.
    echo "==> Install boot monitor '$bin_boot'"
    sudo cp -p $bin_boot $tmp_dir/boot/boot || cleanup_exit 1

    # Install the image.
    echo "==> Install image '$bin_image'"
    sudo cp -p $bin_image $tmp_dir/boot/image/image || cleanup_exit 1

    sudo umount $tmp_dir || cleanup_exit 1
    rm -rf $tmp_dir

    sudo $installboot -d $dev -B $bin_bootblock -b /boot/boot -p $bin_boot || cleanup_exit 1
    sudo $edparams $dev 'nucleos(1,Start Nucleos) { image=/boot/nucleos; boot; }; newnucleos(2,Start Custom Nucleos) { unset image; boot }; main() { echo By default, Nucleos will automatically load in 3 seconds.; echo Press ESC to enter the monitor for special configuration.; trap 3000 boot; menu; }; save' || cleanup_exit 1
    ;;

  installmbr)
    # Install MBR into first sector
    if [ -z $dev ]; then
      echo "Wrong device!"
      cleanup_exit 1
    fi

    if ([ -z $bin_masterboot ] || [ ! -e $bin_masterboot ]); then
      echo "No such file '$bin_masterboot'!"
      cleanup_exit 1
    fi

    # Install the MBR.
    echo "==> Install MBR '$bin_masterboot'"
    sudo $installboot -m -d $dev -B $bin_masterboot || cleanup_exit 1
    ;;

  hddboot)
    # Install a new image on the root device.
    # Note: assumes filesystem present
    if [ -z $dev ]; then
      echo "Wrong device!"
      cleanup_exit 1
    fi

    if ([ -z $bin_image ] || [ ! -e $bin_image ]); then
      echo "No such file '$bin_image'!"
      cleanup_exit 1
    fi

    mkdir $tmp_dir || cleanup_exit 1
    sudo mount -t minix $dev $tmp_dir || cleanup_exit 1

    sudo mkdir -p $tmp_dir/boot
    sudo mkdir -p $tmp_dir/boot/image

    # Install the image.
    echo "==> Install image"
    sudo cp -p $bin_image $tmp_dir/boot/image/image || cleanup_exit 1

    sudo umount $tmp_dir || cleanup_exit 1
    rm -rf $tmp_dir
    ;;

  cdfdboot)
    # cdfdboot: Make a boot floppy image to go on a CD for booting from.
    if [ -z $dev ]; then
      echo "Wrong device!"
      cleanup_exit 1
    fi

    if ([ -z $bin_image ] || [ ! -e $bin_image ]); then
      echo "No such file '$bin_image'!"
      cleanup_exit 1
    fi

    if ([ -z $bin_boot ] || [ ! -e $bin_boot ]); then
      echo "No such file '$bin_boot'!"
      cleanup_exit 1
    fi

    if ([ -z $bin_bootblock ] || [ ! -e $bin_bootblock ]); then
      echo "No such file '$bin_bootblock'!"
      cleanup_exit 1
    fi

    echo "==> Creating minix filesystem (V3) on $dev"
    sudo $mkfs -B 1024 -b 1440 -i 512 $dev || cleanup_exit 1

    mkdir $tmp_dir || cleanup_exit 1

    # Install /dev, /boot
    echo "==> Installing binaries"
    sudo mount $dev $tmp_dir || cleanup_exit 1
    sudo mkdir $tmp_dir/dev
    sudo mkdir $tmp_dir/boot
    sudo mkdir $tmp_dir/boot/image

    echo "==> Installing /dev"
    sudo tar -C $tmp_dir/dev -xjf $scipts/dev.tar.bz2 || cleanup_exit 1

    sudo cp -p $bin_boot $tmp_dir/boot/boot || cleanup_exit 1

    # install kernel image
    sudo cp -p $bin_image $tmp_dir/boot/image/ || cleanup_exit 1

    sudo umount $tmp_dir || cleanup_exit 1
    rm -rf $tmp_dir

    echo "==> Patching boot code address and adding boot paramenters"
    sudo $installboot -d $dev -B $bin_bootblock -b /boot/boot -p $bin_boot || cleanup_exit 1
    sudo $edparams $dev 'unset bootopts; unset servers; disable=inet; image=/boot/image/image; bootbig(1, Regular Nucleos (requires at least 16 MB RAM)) { image=/boot/image/image ; boot } bootsmall(2, Small Nucleos (intended for 8 MB systems)) { image=/boot/image/image_small ; boot } cdproberoot=1; unset rootdev; unset leader; leader() { echo \n--- Welcome to Nucleos. This is the boot monitor. ---\n\nChoose an option from the menu or press ESC if you need to do anything special.\nOtherwise I will boot with my defaults in 10 seconds.\n\n }; bootcd=1; main(){trap 10000 boot; menu; }; save' || cleanup_exit 1
    ;;

  fdboot)
    # fdboot: Make a boot floppy to go on HDD.
    if [ -z $dev ]; then
      echo "Wrong device!"
      cleanup_exit 1
    fi

    if ([ -z $bin_image ]|| [ ! -e $bin_image ]); then
      echo "No such file '$bin_image'!"
      cleanup_exit 1
    fi

    if ([ -z $bin_boot ] || [ ! -e $bin_boot ]); then
      echo "No such file '$bin_boot'!"
      cleanup_exit 1
    fi

    if ([ -z $bin_bootblock ] || [ ! -e $bin_bootblock ]); then
      echo "No such file '$bin_bootblock'!"
      cleanup_exit 1
    fi

    # Make a file system.
    echo "==> Creating minix filesystem (V3) on $dev"
    sudo $mkfs -B 1024 -i 512 $dev || cleanup_exit 1

    mkdir $tmp_dir || cleanup_exit 1

    # Install /dev, /boot/boot and /boot/image.
    echo "==> Installing binaries"
    sudo mount $dev $tmp_dir || cleanup_exit 1
    sudo mkdir -p $tmp_dir/dev
    sudo mkdir -p $tmp_dir/boot/image || cleanup_exit 1

    echo "==> Installing /dev"
    sudo tar -C $tmp_dir/dev -xjf $scripts/dev.tar.bz2 || cleanup_exit 1
    sudo cp -p $bin_boot $tmp_dir/boot/boot || cleanup_exit 1

    # install kernel image
    sudo cp -p $bin_image $tmp_dir/boot/image/ || cleanup_exit 1

    sudo umount -f $tmp_dir

    rm -rf $tmp_dir

    # Make bootable and copy the boot parameters.
    # Note: `/boot/boot' is path inside the image while $bootblock is on
    #       host system ehrrrr...
    echo "==> Patching boot code address and boot paramenters"
    sudo $installboot -d $dev -B $bin_bootblock -b /boot/boot -p $bin_boot || cleanup_exit 1
    pfile=fdbootparams

    if [ -f $pfile ];  then
      echo "Using floppy boot parameters from file $pfile."
      sudo $edparams $dev "`cat $pfile`" || cleanup_exit 1
    else
      echo "Missing boot parameters."
      cleanup_exit 1
    fi
    sudo $edparams $dev 'main(){delay 2000;boot}; save' || cleanup_exit 1
    echo "Test kernel installed on $dev"
    ;;
esac

echo "==> $action done"

# clean & exit
cleanup_exit 0
