[郑州大学 汇编语言程序设计](https://www.icourse163.org/course/ZZU-1001796025) 示例练习代码  
授课老师 钱晓捷 张青

使用 [NMake](https://docs.microsoft.com/en-us/cpp/build/nmake-reference) 管理构建

    nmake /nologo                       # 默认以 Debug 配置构建
    nmake /nologo config=Release        # 以 Release 配置构建
    nmake /nologo clean                 # 清理 Debug 构建时生成的中间文件和结果文件
    nmake /nologo clean config=Release  # 清理 Release 构建时生成的中间文件和结果文件
    windbg -WF <Project>.wew            # 用 WinDbg 打开调试工作区文件

# 示例工程说明

**C8_3_Module**  
C/C++-Assembly 混合编程示例  
使用 VC 6.0 开发包构建，包含 MASM 汇编器、MSVC 编译器，以及 MSVCRT 和 WinAPI 库 (.h/.lib)

VC 6.0 开发包 [百度网盘](https://pan.baidu.com/s/1z_Ds4yGsOlzJ8OIuLaQDVA) [提取码 zwld]  
该开发包是 VS 6.0 Enterprise SP6 + VC 6.0 Processor Pack 的 VC 6.0 目录备份

**除 C8_3_Module 之外的其它示例**  
使用 MASM 6.15 Lite 开发包构建，包含课程中的 I/O 子程序库 (io32.inc/.lib)

MASM 6.15 Lite 开发包 [百度网盘](https://pan.baidu.com/s/1tQoRc_Q_qd1p2w2NfldKPw) [提取码 pxck]  
该 MASM 6.15 精简开发包由以下开发包整合而成：
* VC 1.52 中的 DOS 16bit 工具链 link.exe, lib.exe, CodeView (cv.exe)
* VS 6.0 Enterprise SP6 中的 NMake 构建管理器
* VC 6.0 Processor Pack 中的 MASM 6.15.8803 汇编器
