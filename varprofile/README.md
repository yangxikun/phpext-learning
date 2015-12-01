## Description

~~~
array varprofile () 
~~~

该函数用于统计PHP程序运行时各变量的内存占用情况。

## Parameters

占无

## Return Values

返回数组，格式类似：
~~~
array(4) {
    'global' =>                 //全局变量
    array(*) {
        '_GET' =>               //变量名，且是数组
        array(1) {
          't' =>                //数组内容
          int(4)                //$_GET['t']使用的字节数
        }
        ……
    },
    'active' =>                 //当前作用于变量
    array(*) {
        ……
    },
    'function.static' =>        //函数的静态变量
    array(*) {
        ……
    },
    'class.static' =>           //类的静态变量
    array(*) {
        ……
    }
}
~~~

## 变量的内存计算规则

* 没有将zval、zvalue、hashtable等结构体的内存占用计算在内；
* BOOL、LONG算sizeof(long)个字节；
* DOUBLE算sizeof(double)个字节；
* STRING算字符串占用的字节数；

## varprofile()返回的结果进行处理（排序、筛选……）

![]()
