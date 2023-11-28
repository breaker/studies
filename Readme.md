# 个人研习资料库

该仓库用于存放个人的学习与测试内容，包括：

* 代码片段 (code snippets)
* 书籍、教程、课程的示例练习代码 (samples and practices)
* 实验工程 (experimental projects)
* 技术社区的作答与代码 (answers with code)
* 零碎的技术笔记 (technical notes)
* 其它随性或临时的东西 (quick and dirty)

这个仓库的子目录之间基本上没有什么关系，而且版本信息也不重要（将不定期清理版本信息）。也就是说，它被当作一个网盘使用。

可用 Subversion 的 export 命令下载指定的子目录，并忽略掉版本信息，如下：

```
# 将 GitHub 网址中的 tree/master 替换为 trunk
svn export https://github.com/breaker/studies/trunk/<subdir>
```
