3D 数学基础：图形与游戏开发 (2005.7) 史银雪 等译
https://book.douban.com/subject/1400419/
3D Math Primer for Graphics and Game Development (2002.6), by Fletcher Dunn, Ian Parberry
https://www.amazon.com/dp/1556229119/

----------
示例练习代码

对原示例代码的修改：

1. 将公共代码改为 DLL 程序 MyEngine。
   MyEngine 涉及本书的主要内容，即 3D Math 功能：
   矩阵变换、欧拉角、四元数和 AABB 外接体等
   类似于 D3DXMath (DX7~DX11) 或 XNAMath/DirectXMath (DX11+ in Win8+ SDK) 等数学库。
   MyEngine 基于 D3D8 实现图形渲染。

2. MyViewer 是一个基于 MyEngine 的简单模型查看器，附带一个沙发的 S3D 模型。
   将其改为窗口模式，以及从命令行指定 S3D 模型：
   MyViewer AR_COUCH.s3d
