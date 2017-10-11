早上来实验室 pull前晚在寝室提交的代码
结果提示
```
error: Your local changes to the following files would be overwritten by merge:
```

git diff 显示如下
```
diff --git a/CppPrimer/chapter_12/main.cpp b/CppPrimer/chapter_12/main.cpp
index 0787952..e4f4e45 100644
--- a/CppPrimer/chapter_12/main.cpp
+++ b/CppPrimer/chapter_12/main.cpp
@@ -85,5 +85,4 @@ shared_ptr<int> mixShared(void)

 //    return new int(1024); // <E9><94><99><E8><AF><AF><EF><BC><8C><E5><8E><9F><E5>▒<A0><E5><90><8C> p1
     return shared_ptr<int>(new int(1024));
-
 }
```

一时间没看懂，遂google之
[这篇blog解释的很清楚](https://www.git-tower.com/learn/git/ebook/cn/command-line/advanced-topics/diffs)

