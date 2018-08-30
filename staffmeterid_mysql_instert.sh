#!/bin/bash
#@dark_jadeite
#2018.8.30
#为用户关联一些仪表
#用于显示好看
#需要按顺序输入：
#$1--起始用户的account值
#$2--起始meterID
#$3--要关联仪表的用户个数
#$4--每个用户关联的仪表个数
if [ $# -le 3 ];then
	echo "#需要按顺序输入:\n--起始用户的account值\n--起始meterID\n--要关联仪表的用户个数\n--每个用户关联的仪表个数\n--y可直接执行"
	exit 1
fi
startAccount=$1
startMeterID=$2
accountNumber=$3
meterNumber=$4
echo "起始account:$startAccount"
echo "起始meterID:$startMeterID"
echo "要关联仪表的用户数:$accountNumber"
echo "每个用户关联的仪表个数:$meterNumber"
if [ $# -eq 4 ];then
echo "请确认输入y/n(n)"
read a
case $a in
	"y")
	echo "已确认"
	;;
	*)
	echo "取消操作"
	exit 1
	;;
esac
else
sleep 1;
fi
i=0
while [ $i -lt $accountNumber ]
do
	j=0
	while [ $j -lt $meterNumber ]
	do
		mysql -u root<<EOF
		use tcp;
		insert into StaffMeterID(Saccount,meterID)values($startAccount,$startMeterID);
EOF
		echo "$startMeterID插入完毕！"
		j=$(($j + 1))
		startMeterID=$(($startMeterID+1))
	done

	i=$(($i+1))
	startAccount=$(($startAccount+1))
done

