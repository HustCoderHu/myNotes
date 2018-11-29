工作流
---

# 多分支
以github 为例，先fork别人的repo，比如说repo_a
```
git clone 自己的git url
git remote add REMOTE_NAME URL_REPO_A # 上游的repo加到 REMOTE_NAME 里
git remote -v # 查看remote
```

如果上游更新了，下面操作会更新github上自己的fork，因为用的不是merge，所以不会有合并的commit
```
git fetch REMOTE_NAME  # 先拉一下上游的代码
git checkout master # 切换到本地 master
git rebase -i REMOTE_NAME/master # 交互式 将REMOTE_NAME的master提交并到当前的分支, 
# 就是上面checkout的
git push -f # be careful， 强行覆盖自己github上 的当前分支
```

在团队里时，开发的分支和master分开，常见是每个人自己有个分支
如果要开始开发，新开一个分支
```
git checkout -b BRANCH_NAME # 本地新建一个分支用来添加代码
git push -u origin BRANCH_NAME # 将这个分支推到 github 上 (新建分支)
# -u 表示 远程分支作为 upstream，pull和push都会对应
git rebase -i REMOTE_NAME/BRANCH # 将上游的 BRANCH 分支并到当前操作的分支
git rebase -i master # 将本地的master分支并到当前分支
```
因为 rebase 会改写历史，所以如果要把执行 rebase 时的分支 push 到 github 上，
就要加 `-f` 参数

push的更详细的用法是
```
git push REPO_NAME BRANCH_NAME_A : BRANCH_NAME_B
```
三种功能
- 只留push，当前分支更新remote的同一分支
- 留A不留B，上面提到的在github上新建分支
- 留B不留A，删除github(remote)的分支B

```
git branch -d BRANCH_NAME # 删除本地分支，要先切到其他分支才能删除
```

# 冲突
多人开发过程中，他人提交代码到 master 上，自己本地分支修改了相关的代码，pull 的过程中就会发生冲突

pull 等效 `fetch + merge`  
加上`--rebase` 则是 `fetch + rebase`
```
git pull [--rebase] <远程主机名> <远程分支名>:<本地分支名>
```

如果出现冲突，输入以下进行解决
```
# 开始解决
git mergetool
# 解决之后
git rebase --continue
```

## 可能的情况

`git status` 看到 `branch ... have diverged`

# 比较工具
查看支持哪些有效
```
git difftool --tool-help
```
如果有 `bc3` 就继续往下

``` shell
# difftool 配置
git config --global diff.tool bc3
git config --global difftool.bc3.path "I:\git\Beyond Compare \BCompare.exe"

# mergeftool 配置
git config --global merge.tool bc3
git config --global mergetool.bc3 "I:\git\Beyond Compare \BCompare.exe"
git config --global mergetool.bc3.trustExitCode true
```

# 参考
Understanding the GitHub Flow  
<https://guides.github.com/introduction/flow/index.html>  
Git 工作流程 作者： 阮一峰  
<http://www.ruanyifeng.com/blog/2015/12/git-workflow.html>  
github 创建新分支  
<http://www.jianshu.com/p/a34f81a29704>  
git的突出解决--git rebase之abort、continue、skip  
<https://www.cnblogs.com/chenjunjie12321/p/6876220.html>  
<https://www.yiibai.com/git/git_pull.html>  
git调用beyond Compare  
<https://blog.csdn.net/unsv29/article/details/78485359>
Beyond Compare作为git的比对与合并工具  
<https://blog.csdn.net/zhangbinsijifeng/article/details/48029683>  
Git - Your branch and 'origin/xxx' have diverged  
<https://blog.csdn.net/d6619309/article/details/52711035>  