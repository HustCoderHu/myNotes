转自 [这里](https://github.com/uniqueyehu/blog/blob/master/blogmd/git.md)

# 写在前面的话
有些东西如果不经常使用的话，很快就容易忘记，我们记得很熟悉的一般都是平时使用得很频繁的，命令也好，快捷键也罢，都是如此。记录一下Git的使用，免得哪天自己又忘记了，可以回过头来看看 :smile:

# 基本用法
## Git原理简图
![git.png](https://raw.githubusercontent.com/uniqueyehu/blog/master/images/git.png)

图中几个关键的名词：
```
Workspace：工作区
Index / Stage：暂存区
Repository：仓库区（或本地仓库）
Remote：远程仓库
```

## 常用命令
### 新建代码库
在当前目录新建一个Git代码库
```
$ git init
```

新建一个目录，将其初始化为Git代码库
```
$ git init [project-name]
```

下载一个项目和它的整个代码历史
```
$ git clone [url]
```

### 配置Git
Git的设置文件为 `.gitconfig`，它可以在用户主目录下（全局配置），也可以在项目目录下（项目配置）。

显示当前的Git配置
```
$ git config --list
```

编辑Git配置文件
```
$ git config -e [--global]
```

设置提交代码时的用户信息
```
$ git config [--global] user.name "[name]"
$ git config [--global] user.email "[email address]"
```

### 添加文件
添加当前目录的所有文件到暂存区
```
$ git add .
```

### 代码提交
允许空白更改（--allow-empty），提交工作区上次commit之后的变化直接到仓库区（-a），附带message信息（-m "message"）
```
$ git commit --allow-empty -am "message"
$ git commit -m '	# 多行注释
>>1. 1st line
>>2. 2nd line
>>3. 3rd line
>>'
```

### 远程同步
上传本地指定分支到远程仓库
```
$ git push [remote] [branch]
```

 取回远程仓库的变化，并与本地分支合并
```
$ git pull [remote] [branch]
```

### 分支
不加参数时列出所有本地分支，`-r` 列出所有远程分支，`-a` 列出所有本地和远程分支
```
$ git branch [-r/a]
```

新建一个分支，并切换到该分支
```
$ git checkout -b [branch]
```

切换到指定分支，并更新工作区
```
$ git checkout [branch-name]
```

合并指定分支到当前分支
```
$ git merge [branch]
```

删除分支
```
$ git branch -d [branch-name]
```

### 查看状态
显示有变更的文件
```
$ git status
```

显示当前分支的版本历史，`--stat` 显示commit历史，以及每次commit发生变更的文件
```
$ git log [--stat]
```

显示暂存区和工作区的差异
```
$ git diff
```

## 基本命令
基本命令如果忘记了的话，可以直接用help命令来快速查看
```
$ git help

usage: git [--version] [--help] [-C <path>] [-c name=value]
           [--exec-path[=<path>]] [--html-path] [--man-path] [--info-path]
           [-p | --paginate | --no-pager] [--no-replace-objects] [--bare]
           [--git-dir=<path>] [--work-tree=<path>] [--namespace=<name>]
           <command> [<args>]

These are common Git commands used in various situations:

start a working area (see also: git help tutorial)
   clone      Clone a repository into a new directory
   init       Create an empty Git repository or reinitialize an existing one

work on the current change (see also: git help everyday)
   add        Add file contents to the index
   mv         Move or rename a file, a directory, or a symlink
   reset      Reset current HEAD to the specified state
   rm         Remove files from the working tree and from the index

examine the history and state (see also: git help revisions)
   bisect     Use binary search to find the commit that introduced a bug
   grep       Print lines matching a pattern
   log        Show commit logs
   show       Show various types of objects
   status     Show the working tree status

grow, mark and tweak your common history
   branch     List, create, or delete branches
   checkout   Switch branches or restore working tree files
   commit     Record changes to the repository
   diff       Show changes between commits, commit and working tree, etc
   merge      Join two or more development histories together
   rebase     Reapply commits on top of another base tip
   tag        Create, list, delete or verify a tag object signed with GPG

collaborate (see also: git help workflows)
   fetch      Download objects and refs from another repository
   pull       Fetch from and integrate with another repository or a local branch
   push       Update remote refs along with associated objects
```

# 安装Git
## Windows
下载安装即可，完成安装之后，就可以使用命令行的 git 工具（已经自带了 ssh 客户端）了，另外还有一个图形界面的 Git 项目管理工具。
[https://git-for-windows.github.io](https://git-for-windows.github.io)

# 参考
1. [常用Git命令清单](http://www.ruanyifeng.com/blog/2015/12/git-cheat-sheet.html)
2. [Pro Git](https://git-scm.com/book/zh/v1/)