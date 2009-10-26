#!/usr/local/bin/bash

function library_add()
{
  echo "CREATE_MODULE($CURRNAME ${FILEPREFIX}$FILENAME modules${MODPATH2})"
}

if [ ! -z "$1" ]; then
  FILEPREFIX="$1"
else
  MODPATH2="/autoload"
fi


for i in *.c; do 
  FILENAME=$i
  CURRNAME=`echo $i | sed -e "s,\.c,,g"`
  library_add
done

