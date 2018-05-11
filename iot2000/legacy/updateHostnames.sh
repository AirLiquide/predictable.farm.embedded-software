#!/bin/sh                                                                                                                                   
FILE=/etc/hosts

PREDICTABLE_NODES="[predictable1.local=B4:21:8A:F8:29:03][predictable14.local=B4:21:8A:F8:63:7F][predictable4.local=B4:21:8A:F8:64:AD][predictable22.local=B4:21:8A:F8:59:14]"
NB_PREDICTABLE_NODES=$(echo $PREDICTABLE_NODES | awk -vRS="]" -vFS="[" '{print $2}' | awk -F'=' '{print $2}' | wc -w)

scanNetwork()
{
  echo "scanning network"
  nmap -sS -p 22 192.168.1.0/24 > scanNetwork.log
}

getMacAddr()
{
  INDEX=$1
  echo $PREDICTABLE_NODES | awk -vRS="]" -vFS="[" '{print $2}' | awk -F'=' '{print $2}' | head -$INDEX | tail -n 1
}

getHostname()
{
  INDEX=$1
  echo $PREDICTABLE_NODES | awk -vRS="]" -vFS="[" '{print $2}' | awk -F'=' '{print $1}' | head -$INDEX | tail -n 1
}

getCurrentIP()
{
  ADDR=$1
  IP="$(cat scanNetwork.log | grep $ADDR -B 4 | grep -o '[0-9]\{1,3\}\.[0-9]\{1,3\}\.[0-9]\{1,3\}\.[0-9]\{1,3\}')"
  echo $IP
}
                                                                                                                    
scanNetwork                                                                                                                                                    
                       
rm -f $FILE
touch $FILE
echo "127.0.0.1 localhost" >> $FILE   
LOOP=$(expr $NB_PREDICTABLE_NODES)
for i in $(seq 1 $LOOP)
do
  echo loop:$i
  MAC=$(getMacAddr $i)
  HOSTNAME=$(getHostname $i)
  echo $MAC $HOSTNAME
  IPADDR=$(getCurrentIP $MAC)

  if [ ! -z "$IPADDR" ]; then
        echo $MAC $HOSTNAME $IPADDR
        echo $IPADDR" "$HOSTNAME >> $FILE
  fi                               
done
