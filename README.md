# pump
超高并发C++服务器无锁框架（PUMP）。以及基于框架开发的消息队列系统（APP）（开发中）

基于C++17实现的flow异步处理框架.
目前框架+LSU_CACHE在未细化调优参数的情况下的极限处理能力为1161028.15QPS.(I78086K12核心)
正实现DPDK模块