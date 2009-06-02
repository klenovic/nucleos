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
# partition type
parttype=81
# default block size
blocksize=1024
# default filesystem version is 3
fs_version=

usage()
{
	echo "Usage: `basename $0` [-h] [-2] [-a action] [-B blocksize] [-o offset] [-s partsize] diskimg" >&2
	echo "[-2]: version 2 of filesystem to be created"
	echo "[-a action]: action to do {mkfs,part}"
	echo "[-B blocksize]: required blocksize on image"
	echo "[-o offset]: setup loop device with specified offset"
	echo "[-s part]: partition size, zero means maximum"
	echo "[-h]: this help"
}

setup_loop_device()
{
	if [[ -z $diskimg || ! -a $diskimg ]]; then
		echo "Disk image '$diskimg' doesn't exist!"
		exit 1
	fi

	# get first free loop device and setup
	loop_device=`sudo /sbin/losetup -f`
	echo -n "==> Setup loop device: $loop_device..."

	sudo /sbin/losetup $loop_device $diskimg -o $loop_offset
	exitcode=$?

	if [[ $exitcode != 0 ]]; then
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
while getopts "a:B:ho:s:2" OPT "$@"
do
	case "$OPT" in
	a) # specifies action to do
		action=$OPTARG
		# some specific settings
		case "$action" in
		mkfs) # Create filesystem
			need_loop_device="y"
			;;

		part) # Partition table
			need_loop_device="y"
			;;

		*)
			echo "Unknown action '$OPTARG'!"
			exit 1
		esac
		;;

	B) # specifies blocksize binary
		if [ -z $OPTARG ]; then
			echo "Missing argument!"
			exit 1
		fi

		if ! [ $OPTARG -eq $OPTARG 2>/dev/null ]; then 
			echo "Wrong value '$OPTARG'!"
			exit 1
		fi

		blocksize=$OPTARG
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

	s) # specifies the partition size
		if [ -z $OPTARG ]; then
			echo "Missing argument!"
			exit 1
		fi

		if ! [ $OPTARG -eq $OPTARG 2>/dev/null ]; then 
			echo "Wrong value '$OPTARG'!"
			exit 1
		fi

		partsize=$OPTARG

		# zero means max. size
		if [ $partsize == 0 ]; then
			partsize=0+
		fi
		;;

	2) # version 2 of filesystem to be created
		fs_version=-2
		;;

	h) # help
		usage
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

if [[ -z $diskimg || ! -a $diskimg ]]; then
	echo "Disk image not specified!"
	exit 1
fi

# Setup a loop device if needed
if [ $need_loop_device ]; then
	setup_loop_device
	dev=$loop_device
fi

case $action in
	mkfs)
		# Create filesystem on device. Specify the offset of partition by using `-o' option.
		if [[ "$fs_version" == "-2" ]]; then
			if [[ $blocksize != 1024 ]]; then
				echo "Can't specify a block size if FS version is <3"
				cleanup_exit 1
			fi
			# creat filesystem version 2
			$mkfs $fs_version $dev || cleanup_exit 1
		else
			# creat filesystem version 3
			$mkfs $fs_version -B $blocksize $dev || cleanup_exit 1
		fi
		;;

	part)
		# Create entry in partition table. Partition has maximum size 
		# (depends on specified offset) and marked bootable!
		$partition $dev 1 $parttype:$partsize* || cleanup_exit 1
		;;
esac

echo "==> $action done"

# clean & exit
cleanup_exit 0
