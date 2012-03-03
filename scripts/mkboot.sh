#!/bin/sh
##############################################################################
#  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
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
bin_bootblock=
bin_boot=
bin_image=
bin_masterboot=

# indicates whether option needs loop device
need_loop_device=
# used loop device
loop_device=
# loop offset
loop_offset=
# defualt loop offset
def_loop_offset=0
bootblock_offset=
boot_offset=
masterboot_offset=0
sector_size=512
# skip bytes at start of output
seek_bytes=512
# temporary directory
tmp_dir=.$$_mnt

boot_offset=
bootblock_offset=

# Parse the string which si in form first:second and print the
# required one according to second argument
#  arg1  string to parse
#  arg2  which string is required to return
parse_string()
{
	# what to return
	if [[ $2 == 1 ]]; then
		echo ${1%:*}
	fi

	if [[ $2 == 2 ]]; then
		# remove the first string
		_str2=${1#${1%:*}}
		# remove `:' from begining
		_str2=${_str2#:}
		echo $_str2
	fi

	echo ""
}

usage()
{
  echo "Usage: `basename $0` [-h] [-b boot:offset] [-B bootblock:offset] [-m masterboot] [-i sysimg:offset] diskimg"
  echo "[-b boot:offset]: install boot utility on floppy/disk image at specified offset (offset in 512 size)"
  echo "[-B bootblock:offset]: install bootblock on floppy/disk image at specified offset (offset in 512 size)"
  echo "[-i image:offset]: install system image on diskimg (type of diskimg needed)."
  echo "                   The 'offset' specifies an offset in loop device"
  echo "[-m masterboot]: install MBR on floppy/disk image at offset 0"
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
while getopts "b:B:hi:m:o:" OPT "$@"
do
	case "$OPT" in
	b) # specifies boot binary
		bin_boot=`parse_string $OPTARG 1`
		boot_offset=`parse_string $OPTARG 2`
 
		if ([ -z $bin_boot ] || [ ! -e $bin_boot ]); then
			echo "No such file '$bin_boot'!"
			exit 1
		fi
	;;

	B) # specifies bootblock binary
		bin_bootblock=`parse_string $OPTARG 1` 
		bootblock_offset=`parse_string $OPTARG 2`

		if ([ -z $bin_bootblock ] || [ ! -e $bin_bootblock ]); then
			echo "No such file '$bin_bootblock'!"
			exit 1
		fi
	;;

	i) # specifies kernel image
		need_loop_device="y"
		bin_image=`parse_string $OPTARG 1` 
		loop_offset=`parse_string $OPTARG 2`

		if ([ -z $bin_image ] || [ ! -e $bin_image ]); then
			echo "No such file '$bin_image'!"
			exit 1
		fi
	;;

	m) # specifies MBR binary
		bin_masterboot=$OPTARG

		if ([ -z $bin_masterboot ] || [ ! -e $bin_masterboot ]); then
			echo "No such file '$bin_masterboot'!"
			exit 1
		fi
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

# Last one is disk/floppy image
shift `echo $OPTIND-1 | bc`
diskimg="$1"

if ([ -z $diskimg ] || [ ! -e $diskimg ]); then
	echo "Disk image not specified!"
	exit 1
fi

if [ ! -z $need_loop_device ]; then
	# if the offset was not specified then use default
	if [ -z $loop_offset ]; then
		loop_offset=$def_loop_offset
	fi
	# Setup a loop device if needed
	setup_loop_device
	dev=$loop_device

	if [ -z $dev ]; then
		echo "Wrong device!"
		cleanup_exit 1
	fi
fi

# Install the image.
if [ ! -z $bin_image ]; then
	if [ ! -e $bin_image ]; then
		echo "No such file '$bin_image'!"
		cleanup_exit 1
	fi

	mkdir $tmp_dir || cleanup_exit 1
	sudo mount -t minix $dev $tmp_dir || cleanup_exit 1

	sudo mkdir -p $tmp_dir/boot

	echo "==> Installing image '$bin_image' ..."
	sudo cp -p $bin_image $tmp_dir/boot/image || cleanup_exit 1

	sudo umount $tmp_dir || cleanup_exit 1
	rm -rf $tmp_dir
fi

# Install the boot monitor at specified offset
if [ ! -z $bin_boot ]; then
	if [ ! -e $bin_boot ]; then
		echo "No such file '$bin_boot'!"
		cleanup_exit 1
	fi

	echo "==> Installing boot monitor '$bin_boot' at offset '$boot_offset*$sector_size' ..."
	dd conv=notrunc if=$bin_boot of=$diskimg seek=$boot_offset obs=512
fi

if [ ! -z $bin_bootblock ]; then
	if [ ! -e $bin_bootblock ]; then
		echo "No such file '$bin_bootblock'!"
		cleanup_exit 1
	fi

	echo "==> Installing bootblock '$bin_bootblock' at offset '$bootblock_offset*$sector_size' ..."
	dd conv=notrunc if=$bin_bootblock of=$diskimg seek=$bootblock_offset obs=512
fi

if [ ! -z $bin_masterboot ]; then
	if [ ! -e $bin_masterboot ]; then
		echo "No such file '$bin_masterboot'!"
		cleanup_exit 1
	fi

	echo "==> Installing masterboot '$bin_masterboot' at offset '$masterboot_offset*$sector_size' ..."
	dd conv=notrunc if=$bin_masterboot of=$diskimg skip=32 ibs=1 count=1 obs=512
fi

# clean & exit
cleanup_exit 0
