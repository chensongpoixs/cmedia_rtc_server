#!/bin/bash

PROJECT=`pwd`

PROTC_EXE=protoc

PROTO_OUT_DIR=../../Server/common/cclient_msg




function compile_proto()
{
	proto_file=$1
	proto_out_dir=$2
	
	if [ ! -d $proto_file ]; then
		echo "not find dir $proto_file";
        	return;
	fi
	
	if [ ! -d $proto_out_dir ]; then
		 mkdir $proto_out_dir;
	fi
	
	echo $proto_file
	
	cd $proto_file
	
	for fileproto in $(ls *.proto);
	do
		echo "compile ${fileproto} " #copy_cc
		${PROTC_EXE} --cpp_out=${proto_out_dir} ${fileproto}
		
	done
}




compile_proto  .  $PROTO_OUT_DIR






