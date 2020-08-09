# 混编实验：MATLAB - C - FORTRAN（MEX方法）

## 0. 概述

科研需求，要在MATLAB中调用FORTRAN老代码。那么首先我不咋会FORTRAN，大一倒是学了一点C，参考MATLAB官方文档后，决定用C做接口语言，将FORTRAN老代码编译为MEX文件。

如果有MATLAB调用C或FORTRAN的需要，可以参考这个工程。

老代码是Tsyganenko博士的GEOPACK和TS04模型，前者包含IGRF参考场，后者是磁层物理常用的T系列模型的04款，如果有同行，欢迎去他的[主页](http://geo.phys.spbu.ru/~tsyganenko/modeling.html)观摩。

## 1. 工程介绍

### 1.1. 运行环境

- MATLAB: R2018a Win10 64bit
- C: Visual C++ 2013. (随VS2013安装)
- FORTRAN: Intel Visual FORTRAN Composer XE 2013. （是VS2013的扩展包）

后两者需要被设置为mex的编译器选项（`mex -setup`查看，详见MATLAB文档）

> 需要注意混合编程时C和FORTRAN的编译器需要兼容。比如VC++和IVF兼容，gfortran和gcc兼容，但不交叉兼容。MATLAB由于不支持mingw的gfortran编译器，因此这一支我们就不用考虑了。一般来说，装一下VS就可以自动解决这一步

### 1.2 文件说明

- geopack_08.f: GEOPACK程序包，由Tsyganenko博士编写，[原地址](http://geo.phys.spbu.ru/~tsyganenko/Geopack-2008_dp.for)。这里用的是double precision版本
- t04.f: Tsyganenko的T04模型，[原地址](http://geo.phys.spbu.ru/~tsyganenko/TS04c.html)。**注意有一处改动**：开头T04_s参数的变量声明为REAL，疑似笔误，改为REAL*8。
- igrft04.c: 这是接口语言的主要程序，也是本工程主要的工作。
- rebuild.bat: 一个用于编译的批处理。如果需要使用请自行修改IVF库的路径。
- igrft04.mexw64: 编译后的mex文件。**注意**：只兼容相同平台相同版本MATLAB。其他平台请重新编译。
- testmex.m: 测试文件。和编译后mexw64一起放在工作目录可以执行。

### 1.3 已实现的函数列表

- t04.f:T04_S：T04主函数，根据给定参数和位置计算T04磁场。拓展了参数使得位置（XYZ）可以接受矩阵，并把输出改成matlab格式（输出参数在左）。
- geopack_08.f:RECALC_08：geopack初始化函数，输入时间、太阳风速度，初始化各种全局变量（COMMON块），以供后续函数使用。参数自动把double转为整形。
- geopack_08.f:IGRF_GSW_08：计算给定位置的IGRF场。可接受矩阵参数，输出改为matlab格式。需要初始化。
- 一系列坐标转换函数（如SMGSW_08等）

## 2. 混合编程笔记（i.e. 简易教程）

> 这里的笔记可能比较支离破碎。悄悄说一下，如果有MATLAB调用C/Fortran的mex工程相关问题，可以直接问我，可以issue或者email: yangfan1@pku.edu.cn，我尽量回答（如果我会的话）

### 2.1 关于接口文件写法

MEX的接口文件需要一个名为`mexFunction`的入口函数，地位相当于main。这个函数会输入所有输入和输出的参数列表。基本的流程是先验证参数数目和合理性，然后将导出输入参数数据，创建输出参数内存，计算，最后处理输出参数（需要按输出变量数分类处理）

一个mex文件只对应一个函数，但老代码是一整个模块。一个可行的方案是添加一个输入参数mode，在主函数体内用switch-case语句分开调用。如果觉得这样ugly，可以在matlab里多定义一些m文件封装一下。

具体写法基本上是要参考mex的官方文档。最重要的两个系列是C Matrix API和C MEX API，随时放在手边。

C Matrix API对应mx系列函数。MATLAB变量的数据类型是`mxArray`，我们一般用它的指针`mxArray*`。C/Fortran计算需要用的数据类型为int, double等等，在mex中也有封装，如`mxInt32`,`mxDouble`等，本质是typedef。两者之间的操作函数几乎都是mx系列。

C MEX API对应的mex函数则主要是一些和MATLAB本身通信的函数，比如控制台输出、报错、执行语句、和工作区交换变量等等。

这里简单列了一下需求-函数的笔记：（注：以下用*表示一系列）

- 获取变量中的数据（或元数据，矩阵大小等）：mxGet*。见Access Data
- 创建一个变量：mxCreate*。见Create and Delete Data
- 验证变量类型：mxIs*。见Validate Data
- 删除一个变量并释放内存：`mxDestroyArray`
- 分配/释放内存：`mxCalloc`/`mxFree`。注意要`#include <stdlib.h>`。一般来说即使不调用Free在mex调用完后也会自动释放内存。但特别注意和Struct, Cell相关有些变量是不能`mxFree`的，主函数的`plhs`/`prhs`也不可以。
- 将数据存到变量中：mxSet*。注意，被存储的数据必须是被`mxCalloc`等mx函数创建出来的。如果不是，则需要`memcpy`一下
- 控制台输出、报错等：`mexPrintf`,`mexErrMsgIdAndTxt`
- 执行语句：`mexEvalString`,`mexCallMATLAB`
- 交换变量：`mexGetVariable`,`mexPutVariable`

几个坑（想到啥写到啥）

- 32bit和64bit的问题一定要注意。C语言的int占4个字节，size_t（即指针大小）占8个字节。但是我的IVF按默认设置，不显式声明`INTEGER*4`，是占8个字节，这点需要非常注意。可以用`-compatibleArrayDims`和`-largeArrayDims`来调整这个大小。善用`sizeof`。
- 使用结构体的时候管理内存要小心，没信心就忽略Free的过程交给MATLAB自动管理。
- 一定要有先验证变量存在，类型正确，size正确再操作的习惯。这边如果出BUG是直接整个MATLAB闪退的。
- MATLAB里如果不管变量类型，默认是double。因此如果操作整形变量时一定要注意。
- 如果要用Fortran的COMMON块，可以看我后面专门写的一节。

### 2.2 关于C-Fortran混编

C和Fortran混编相对比较容易。推荐看这篇[笔记](http://www.math.utah.edu/software/c-with-fortran.html)（英文）

C中对于外部定义的函数、变量等需要加extern关键字。这一点无论对调用外部的C还是Fortran都一样。

一个比较麻烦的事是，不同编译器对两种混编的接口标准不太一样，有些是用大写，有些是用小写，还有些要加_，总之分清自己的编译器最重要。这一步如果出错在编译阶段不会有问题，只会在链接阶段出错。

还有一个链接阶段的错误是，作为一个C入口的程序，调用Fortran程序时需要链接Fortran的库函数，因此在链接时需要加-L参数。

### 2.3 关于全局变量和COMMON块

COMMON块是老Fortran（F77）使用全局变量的方式。由于COMMON没有对类型的定义，因此某种程度上COMMON也可以作为共用变量使用，在内存比较贵的时代还是很有意义的。

当然这些都不重要。我们只需要知道COMMON块==一整块内存即可。在C里，我们可以用结构体构造出长度相等的内存。当然，由于这些COMMON是在Fortran中定义的，我们需要声明为extern。至于原来COMMON是什么类型没有关系，编译器只管自家的变量类型正确，外面的是什么类型它不care。比如下面这两组定义就是一回事

```C
extern struct type1{
    double a[40]
}var1;
extern struct type2{
    char a[320]
}var1;
```

在我们的code中，老代码使用了GEOPACK1和GEOPACK2两个COMMON，其大小分别是34个和315个double。为了回避C编译器的类型检查，我们可以用memcpy进行数据的拷贝工作。

之前对MATLAB调用mex的机制有所误解，走了好多弯路。事实上，mex一经调用便常驻MATLAB内存，直到被clear掉或MATLAB主程序退出。而C语言中，一个变量只能初始化一次。这就导致了，一个mex中定义的全局变量，在执行后可以保留，达到一种类似static的效果。当然，多个mex之间的全局变量不可以通信，这也告诉我们最好把一个模块所有函数写到一个mex文件里。除此以外，MATLAB也提供了`mexMakeMemoryPersistent`等方法来将主程序中动态定义的数据保持下来，不需要存储到MATLAB工作区或文件中也能在下一次调用中继续使用。当然对我们的问题，是用不到的。

### 2.4 关于调试

> MEX的调试方法在MATLAB文档中有写。Windows平台建议用VS调试，将启动的MATLAB进程挂载到VS内，然后打上断点，在MATLAB中调用mex文件，就可以被VS捕获了

话是这么说，但这种方法对我无效，调试时会跳出“msvsmon.exe意外退出，调试已终止”，甚至我MATLAB还没有执行mex代码就会这样退出。网上查了下，可能因为我装的是VS2013 32bit版本（pjb），而MATLAB是64bit，因此调试时必须调用远程调试程序，而这就可能有很多问题，比如网络问题等等。但这个问题我最终没有解决。或许重装64bit VS能够解决这个问题吧。希望有高人指点。

最后我的调试方法是：在源码里插入mexErrMsgIdAndTxt的抛出异常语句作为手动断点调试。因为mex出错时，整个MATLAB会崩掉直接退出，MATLAB启动又慢，整个过程非常痛苦。