#!/bin/sh

initrd_dir=$1
initrd_dir_list=$2
initrd_files_to_copy=$3
initrd_device_table=$4
initrd_name=$5

# create storage for initrd files
mkdir -p $initrd_dir

# create directory structure
for dir in $initrd_dir_list
do
	mkdir -p $dir
done

# copy files
for src_dst in $initrd_files_to_copy
do
	src=`echo $src_dst | awk -F: '{print $1}'`
	dst=`echo $src_dst | awk -F: '{print $2}'`
	install $src $dst
done

genext2fs -b 1024 -d $initrd_dir -D $initrd_device_table $initrd_name
