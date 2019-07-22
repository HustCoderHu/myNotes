进阶
---

#多用户密钥

##添加私钥
```sh
ssh-add ~/.ssh/id_rsa_github
```

可能会报错，参照 <https://blog.csdn.net/qq_19926599/article/details/86380544> 解决

##创建配置
`.ssh` 目录下创建 `config` 文件
```sh
# gitee and github
# 注意：多个Host公用一个公钥时，
# 对应的HostName和Port必须跟在Host后面
Host gitee.com
HostName gitee.com
Host github.com
HostName github.com
PreferredAuthentications publickey
IdentityFile ~/.ssh/id_rsa

# gerrit
Host gerrit.awaimai.com
HostName gerrit.awaimai.com
Port 8000
PreferredAuthentications publickey
IdentityFile ~/.ssh/id_rsa_gerrit
```

完毕后测试
```sh
ssh -T git@github.com # -v 显示更详细的内容
```

##用户名和邮箱
```sh
git config --global --unset user.name
git config --global --unset user.email
```
仅对某个项目有效

#reference 
https://www.awaimai.com/2200.html