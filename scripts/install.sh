#!/bin/bash

olddir=$(pwd)
basedir=$(dirname $(readlink -f "$0"))

cd $basedir

base=$1
iso=$base/$2
if [[ $3 == /* ]]; then
	dev=$3
else
	dev=$base/$3
fi
log=$base/$4

cmd="sudo dd if=$iso of=$dev"
read -p "Install command: '$cmd'"$'\n'"Do you want to run this command? [y/n] "
if [[ $REPLY == "y" ]]; then
	$cmd 2>$log
	if [[ $? -ne 0 ]]; then cat $log && exit; fi
fi
