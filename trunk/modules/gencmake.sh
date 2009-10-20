#!/usr/local/bin/bash

#ADD_LIBRARY(m_die MODULE core/m_die.c)
#SET_TARGET_PROPERTIES(m_die PROPERTIES VERSION 4.2.0 SOVERSION 4 PREFIX "")
#INSTALL(TARGETS m_die DESTINATION lib)

function library_add()
{
  echo "ADD_LIBRARY($CURRNAME MODULE ${FILEPREFIX}$FILENAME)"
  #echo "SET_TARGET_PROPERTIES($CURRNAME PROPERTIES VERSION 4.2.0 SOVERSION 4 PREFIX \"\")"
  echo "SET_TARGET_PROPERTIES($CURRNAME PROPERTIES PREFIX \"\")"
  echo "INSTALL(TARGETS $CURRNAME DESTINATION modules${MODPATH2} RENAME $REALNAME)"
  echo 
}

if [ ! -z "$1" ]; then
  FILEPREFIX="$1"
else
  MODPATH2="/autoload"
  fi


for i in *.c; do 
  FILENAME=$i
  CURRNAME=`echo $i | sed -e "s,\.c,,g"`
  REALNAME=${CURRNAME}.so
  library_add
done

