objects=main.cpp TcpServer.cpp TcpConnection.cpp Threadpool.cpp ThreadTask.cpp \
LogicServer.cpp InetAddress.cpp EventLoop.cpp EventLoopThread.cpp EventLoopThreadpool.cpp Buffer.cpp \
./include/net/Buffer.h ./include/net/EventLoop.h ./include/net/EventLoopThread.h ./include/net/EventLoopThreadpool.h \
./include/net/InetAddress.h ./include/net/TcpConnection.h ./include/net/TcpServer.h ./include/util/Condition.h ./include/util/MutexLock.h \
./include/chart_logic/LogicServer.h ./include/chart_logic/protocol.h ./include/chart_logic/Threadpool.h \
./include/chart_logic/ThreadTask.h ./include/chart_logic/dao/ChartOneRecord.h ./include/chart_logic/dao/FriendInfo.h



chart_server: $(objects)
	g++ -o chart_server $(objects) -lpthread