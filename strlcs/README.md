strlcs
======

## Description

~~~
string strlcs ( string $str1 , string $str2 [, bool $continues] ) 
~~~

该函数用于求`$str1`和`$str2`的最长公共子序列或最长连续公共子序列。

## Parameters

`$str1`：输入的字符串

`$str2`：输入的字符串

`$continues`：可选的参数，默认为false，当为true时，即查找最长连续公共子序列

## Return Values

返回找到的字符串，如果没找到，返回空字符串。

## Examples

~~~
<?php
$str1 = 'ababaaaaaaaaab';
$str2 = 'bavvaaaaaaaaab';
echo strlcs($str1, $str2)."\n";
echo strlcs($str1, $str2,TRUE)."\n";
?>
~~~