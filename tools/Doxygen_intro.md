根据注释生成文档
---

# 注释规范
```cpp
/*
 * 正常注释
 */

/**
 * 待输出成文档的注释
 */
or
/*!
 * 要输出成文档的注释， 本行前面的*可以省略
 */

// 单行文档注释
/// 要输出成文档的注释
//! 要输出成文档的注释

// 后方注释增加一个 <
#define DEV_ON      ((int)(1))      //!< Simple device is power on.

//***************************************************************************************
//
//! @brief  Print Int number to terimal device.
//!
//! @param  [in] number is the data you want to print.
//! @retval the number of print information, in bytes. return zero indicate print error !.
//!
//! @note
//! * Be sure you have called \ref Dev_Init function before call this fuction.
//! * Remember to check return value.
//
//***************************************************************************************
extern int Dev_PrintInt(int number);

// @ 可以替换成 \
```

自定义格式
``` cpp
/*!
 * @brief  Thisis a brief description.
 * @param[in]
 *  - ids. description
 * @param[out]
 *  - paramN.
 * @Return:    description of the return value
 *
 * @note   matters needing attention
 * @see    other functions   
 */
```

# reference
Doxygen 10 分钟入门教程  
<https://cedar-renjun.github.io/2014/03/21/learn-doxygen-in-10-minutes/>  
Doxygen的注释规则  
<http://www.mikewootc.com/wiki/tool/sw_develop/doxygen_comment_format.html>  