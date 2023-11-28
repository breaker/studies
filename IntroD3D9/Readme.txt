Introduction to 3D Game Programming with DirectX 9.0 (2003.6), by Frank D. Luna
https://www.amazon.com/dp/1556229135/
DirectX 9.0 3D 游戏开发编程基础 (2007.4) 段菲 译
https://book.douban.com/subject/2111771/

----------
示例练习代码

使用 DirectX SDK August 2008

对原示例代码的修改：
1. 将配书代码的 Win32 程序改为 MFC 程序
2. 将原公共工具代码 d3dUtility.h/.cpp 改为 OOD 的 DLL 程序 SGL
   SGL 对 MFC 的依赖是 header-only 的，即不依赖 MFC DLL/LIB，所以 SGL 的用户程序可以不是 MFC 程序
