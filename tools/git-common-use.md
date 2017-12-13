# diff的详细解释  
用 diff 来检查改动  
<https://www.git-tower.com/learn/git/ebook/cn/command-line/advanced-topics/diffs>

# 储藏与清理
有时，当你在项目的一部分上已经工作一段时间后，所有东西都进入了混乱的状态，而这时你想要切换到另一个分支做一点别的事情。 问题是，你不想仅仅因为过会儿回到这一点而为做了一半的工作创建一次提交。 针对这个问题的答案是 git stash 命令

now 想要切换分支，但是还不想要提交之前的工作；所以储藏修改。 将新的储藏推送到栈上，运行 git stash 或 git stash save  
```
git stash       # 储藏修改
git stash list  # 查看储藏的内容
git stash apply # 应用最近一次储藏
git stash drop stash@{0}    # 移出最近一次储藏
git stash pop   # 应用 & drop
```

**参考**  
7.3 Git 工具 - 储藏与清理
<https://git-scm.com/book/zh/v2/Git-%E5%B7%A5%E5%85%B7-%E5%82%A8%E8%97%8F%E4%B8%8E%E6%B8%85%E7%90%86>

# 代码回滚：Reset、Checkout、Revert 的选择
- --soft – 缓存区和工作目录都不会被改变
- --mixed – 默认选项。缓存区和你指定的提交同步，但工作目录不受影响
- --hard – 缓存区和工作目录都同步到你指定的提交
把这些标记想成定义 git reset 操作的作用域就容易理解多了。  
![](https://camo.githubusercontent.com/6f605243c7eedce24cd32e53348d7f5b2db20bff/68747470733a2f2f7777772e61746c61737369616e2e636f6d2f6769742f696d616765732f7475746f7269616c732f616476616e6365642f726573657474696e672d636865636b696e672d6f75742d616e642d726576657274696e672f30332e737667)

常用 
```
git reset --mixed HEAD  # 只撤销 commit，默认
git reset --hard HEAD  # 完全丢弃改动
```

# 代码合并：Merge、Rebase 的选择
fork 别人的repo后，如果上游repo更新了，很自然我们这个最好也跟着

```
# 
git remote add [shortname] [url]    # 添加remote
git fetch [remote-name]   # 从远程仓库抓取数据
git rebase [-i] localBranch     # -i 会把commit 显示出来，可以自己调整
# rebase最大的好处是你的项目历史会非常整洁。首先，它不像 git merge 那样引入不必要的合并提交。
```
<http://bluebiu.com/blog/learn-git-rebase.html>

**参考**  
5 Git 基础 - 远程仓库的使用  
<https://git-scm.com/book/zh/v1/Git-%E5%9F%BA%E7%A1%80-%E8%BF%9C%E7%A8%8B%E4%BB%93%E5%BA%93%E7%9A%84%E4%BD%BF%E7%94%A8>  
代码合并：Merge、Rebase 的选择  
<https://github.com/geeeeeeeeek/git-recipes/wiki/5.1-%E4%BB%A3%E7%A0%81%E5%90%88%E5%B9%B6%EF%BC%9AMerge%E3%80%81Rebase-%E7%9A%84%E9%80%89%E6%8B%A9>

# 参考
git Book  
<https://git-scm.com/book/zh/v2>  
高质量的Git中文教程  
<https://github.com/geeeeeeeeek/git-recipes/>  

[这里](https://git-scm.com/book/zh/v2/Git-%E5%B7%A5%E5%85%B7-%E5%82%A8%E8%97%8F%E4%B8%8E%E6%B8%85%E7%90%86)