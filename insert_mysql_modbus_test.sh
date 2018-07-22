#!/bin/bash
#@dark_jadeite
#2018.7.22
#自动化插入多一点MeterIdentity的数据
#和一些DataIdentity数据
#用于测试使用
#需要按顺序输入：
#$1--meterID的起始数值
#$2--DTUID的起始数值
#$3--要添加的DTU个数
#$4--每个ModBus总线上仪表个数
if [ $# -ne 4 ];then
	echo "#需要按顺序输入:\n--meterID的起始数值\n--DTUID的起始数值\n--要添加的DTU个数\n--每个ModBus总线上仪表个数"
	exit 1
fi
startMeterID=$1
startDTUID=$2
maxDTUID=$3
maxDeviceNumber=$4
startDeviceNumber=0
meterName="仪表$meterID"
echo "起始meterID:$startMeterID"
echo "起始DTUID:$startDTUID"
echo "要添加的DTU个数:$maxDTUID"
echo "单ModBus总线上要添加仪表个数:$maxDeviceNumber"
echo "ModBus总线上起始仪表地址:$startDeviceNumber"
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
i=0
while [ $i -le $maxDTUID ]
do
	j=0
	while [ $j -le $maxDeviceNumber ]
	do
		meterName="仪表$meterID"
		mysql -u root<<EOF
		use tcp;
		insert into MeterIdentify(meterID,DTUID,deviceNumber,meterName)values($startMeterID,$startDTUID,$startDeviceNumber,'$meterName');
		insert into DataIdentify(meterID,ord,dataName,address,byteNumber,dataType)values($startMeterID,1,"瞬时流量",0,4,7);
		insert into DataIdentify(meterID,ord,dataName,address,byteNumber,dataType)values($startMeterID,2,"累积流量",4,4,7);
		insert into DataIdentify(meterID,ord,dataName,address,byteNumber,dataType)values($startMeterID,3,"温度",8,4,7);
		insert into DataIdentify(meterID,ord,dataName,address,byteNumber,dataType)values($startMeterID,4,"压力",12,4,7);
		insert into DataIdentify(meterID,ord,dataName,address,byteNumber,dataType)values($startMeterID,5,"差压",16,4,7);
EOF
		j=$(($j + 1))
		startMeterID=$(($startMeterID+1))
		startDeviceNumber=$(($startDeviceNumber+1))
		echo "$startMeterID插入完毕！"
	done

	i=$(($i+1))
	startDTUID=$(($startDTUID+1))
done

