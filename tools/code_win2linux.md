win上写代码，linux上运行
---
目标：win上使用IDE写好代码后，方便的传送到linux上编译，运行

# 1 依赖
**linux**: ssh server  
**win**: scp  
scp 在 git 的 portable 版本里有，在 `PortableGit\usr\bin\` 下  

# 2 传输代码
scp格式如下
```
remote_ip="192.168.x.x"
remote_dir="/home"
file_list="Makefile *.cpp *.h"

scp ${file_list} root@${remote_ip}:${remote_dir}
```

# 3 执行
依赖 ssh

```
usr="xxx"
remote_ip="192.168.x.x"
remote_dir=/home/$usr

CDIR="cd "${remote_dir}

cmd_r=${CDIR}
cmd_r=${cmd_r}"; ls *.cpp *.h" # 这里填要执行的命令
cmd_r=${cmd_r}"; make -j3"

ssh -t -t -p 22 root@${remote_ip} ${cmd_r}
# win这边会显示结果
# 这个脚本 sh, bash 都可以执行，git portable 里都有

```
如果没有开启 RSA ，上面会弹出输出密码  

如果要开启 ssh 的 rsa，确保`/etc/ssh/sshd_config`中的内容如下
```
RSAAuthentication yes
PubkeyAuthentication yes
AuthorizedKeysFile	%h/.ssh/authorized_keys # 去掉注释
```

把公钥传到指定用户的ssh 认证key的后面(附加到文件)  
之后要重启 ssh server
```
usr="xxx"

pub_key=".pub"
remote_dir="/"$usr"/.ssh/authorized_keys" # 确认有 .ssh 目录

CDIR="cd "${remote_dir}
scp ${file_list} root@${remote_ip}:${remote_dir} # 这里是覆盖
```
win 这一端也是在类似的位置放置私钥