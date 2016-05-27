# ImageDataAugmentation

工程源码GitHub: [yhlleo/ImageDataAugmentation](https://github.com/yhlleo/DataAugmentation)

编译环境：

 - CMake
 - OpenCV
 - Visual Studio 2010

--------------------------------------------------------------

在机器学习和深度学习中，通常使用为了增加数据（Data Augmentation）可以对数据进行例如一系列的旋转（rotate）、镜像（flip）等操作，本文将讲解如何求取旋转图像的最大内接矩形问题，这里的内接矩形，并不是数学上严格的内接概念，而是获得的矩形是不含如图 1所示的旋转导致的空白区域。

[<center>
<img src="http://img.blog.csdn.net/20160527111612913"></img>
**图 1**</center>](http://img.blog.csdn.net/20160527111612913)

[<center>
<img src="http://img.blog.csdn.net/20160526204727593" width=65%></img>
**图 2**</center>](http://img.blog.csdn.net/20160526204727593)

如图2所示，红色矩形ABCD是绿色矩形旋转一定角度后对应的结果，旋转中心定义在图像的中心，坐标轴方向`x->col, y->row`，则可有以下推导：

[<center><img src="http://img.blog.csdn.net/20160526205303517" width=65%></img></center>](http://img.blog.csdn.net/20160526205303517)

但是测试过程中就会发现，上面的分析，对于`col`近似等于`row`的图像会有问题，问题出自哪里呢？如图 3，很容得到的解是这样的蓝色的矩形框，因此，除了上面的极值条件，还必须添加相应的边界条件，从而获得紫色的矩形框边界。

[<center>
<img src="http://img.blog.csdn.net/20160527111109367" width=60%></img>
**图 3**</center>](http://img.blog.csdn.net/20160526223435853)

可以看出，两种情况的最大不同就是， 矩形边AB与坐标轴`col`相交的点是否位于绿色矩形框的内部，前面推导的结果对于位于绿色矩形框内部是成立的，对于位于外部的情形，可有如下分析：

[<center><img src="http://img.blog.csdn.net/20160527112236291" width=65%></img></center>](http://img.blog.csdn.net/20160527112236291)

以上的推导在顺时针旋转角度a属于`(0, 90)`时成立，当角度位a属于`(90, 180)`  时等价于180-a，而当a为90°或180°时相对简单，更多的旋转角度也是以180°为周期，因此，整个旋转最大内接矩形的求取过程就分析完毕~
