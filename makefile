modbusDriver:modbusDriver.o rbtree.o dataProcess.o dataSave.o
	gcc -g modbusDriver.o rbtree.o dataProcess.o dataSave.o -o modbusDriver -I/usr/include/mysql -L/usr/lib64/mysql -lmysqlclient -lpthread -lm -L/lib -L/usr/lib
modbusDriver.o:modbusDriver.c modbusDriver.h rbtree.h dataProcess.h dataSave.h
	gcc -g -c modbusDriver.c -o modbusDriver.o -I/usr/include/mysql -L/usr/lib64/mysql -lmysqlclient -lpthread
rbtree.o:rbtree.c rbtree.h
	gcc -g -c rbtree.c -o rbtree.o 
dataProcess.o:dataProcess.c dataProcess.h modbusDriver.h rbtree.h
	gcc -g -c dataProcess.c -o dataProcess.o -I/usr/include/mysql -L/usr/lib64/mysql -lmysqlclient -lpthread -lm -L/lib -L/usr/lib
dataSave.o:dataSave.c dataSave.h modbusDriver.h rbtree.h
	gcc -g -c dataSave.c -o dataSave.o -I/usr/include/mysql -L/usr/lib64/mysql -lmysqlclient -lpthread

