This cluster chat server optimizes database connections using a connection pool, implements cross-server communication based on a publish-subscribe Redis message queue, and uses Nginx for load balancing.

This project needs libraries include muduo_net, muduo_base, mysqlclient, hiredis, pthread.