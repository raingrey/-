# UDP-Iot
一种基于UDP的物联网数据传输程序

测试用例使用注意：
insert_mysql_modbus_test.sh 
用于自动化插入多一点MeterIdentity的数据和一些DataIdentity数据
需要按顺序输入：
$1--meterID的起始数值
$2--DTUID的起始数值
$3--要添加的DTU个数
$4--每个ModBus总线上仪表个数
需要注意的是，这些设置要与test_modbus_driver想配合
仅在数据库中出现了的仪表和协议 信息才能够被响应
如：
./insert_mysql_modbus_test.sh 0 0 100 30
于
./test_modbus_driver 39.106.62.239 3389 100 5 1000
中的
向数据库添加100个DTUID30个仪表,使用100DTUID和5个仪表是可以的

test_modbus_driver是
测试用UDP发包器
 默认条件：
 * 1.起始DTUID为0
 * 2.起始ModBus地址为0
 * 3.默认发包频率1s
 输入参数：
 * 1.目标IP
 * 2.目标端口
 * 3.模拟DTU数量
 * 4.每个ModBus总线模拟仪表数量
 * 5.发包频率毫秒数
