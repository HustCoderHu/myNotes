win10 zip 方式 安装 mysql server

---

# 初始化
到下面的 URL 下载 x64的zip  
<https://dev.mysql.com/downloads/mysql/>  
解压到某个目录下，将这个文件夹添加到环境变量的 `PATH`

在 bin 所在目录创建 `my.ini` 内容如下
``` ini
[mysqld]
# Remove leading # and set to the amount of RAM for the most important data
# cache in MySQL. Start at 70% of total RAM for dedicated server, else 10%.
# innodb_buffer_pool_size = 128M

# Remove leading # to turn on a very important data integrity option: logging
# changes to the binary log between backups.
# log_bin

# These are commonly set, remove the # and set as required.
basedir = D:/coding/mysql-8.0.11-winx64/
datadir = D:/coding/mysql-8.0.11-winx64/data
port = 3306
# server_id = .....


# Remove leading # to set options mainly useful for reporting servers.
# The server defaults are faster for transactions and fast SELECTs.
# Adjust sizes as needed, experiment to find the optimal values.
# join_buffer_size = 128M
# sort_buffer_size = 2M
# read_rnd_buffer_size = 2M 

sql_mode=NO_ENGINE_SUBSTITUTION,STRICT_TRANS_TABLES 

character-set-server = utf8mb4

performance_schema_max_table_instances = 600
table_definition_cache = 400
table_open_cache = 256

[mysql]
default-character-set = utf8mb4

[client]
default-character-set = utf8mb4
```

admin 权限运行 cmd，执行 `mysqld --initialize-insecure`
- insecure 会导致没有设置密码

# 安装服务
`mysqld --install`  
端口由上面的配置文件给定

安装完成之后，就可以通过命令 `net start mysql` 启动MySQL的服务了

# 启动
```
mysql -uroot -p
```
没有设置密码的时候可以省略 `-p`，下面的语句可以设置密码
``` sql
ALTER USER 'root'@'localhost' IDENTIFIED BY 'new_password'
```

# reference
MySQL 8.0 Windows zip 安装过程  
<https://blog.csdn.net/zwj1030711290/article/details/80039780>  
<https://dev.mysql.com/doc/refman/8.0/en/data-directory-initialization-mysqld.html>  