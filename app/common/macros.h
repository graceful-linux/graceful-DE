/*************************************************************************
> FileName: macros.h
> Author  : DingJing
> Mail    : dingjing@live.cn
> Created Time: Wed 07 Sep 2022 21:09:31 PM CST
 ************************************************************************/
#ifndef _MACROS_H
#define _MACROS_H
#include <stddef.h>

#ifdef __GNUC__
#define C_CHECK_VERSION(major, minor)                           ((__GNUC__ > (major)) || ((__GNUC__ == (major)) && (__GNUC_MINOR__ >= (minor))))
#else
#define C_CHECK_VERSION(major, minor)                           0
#endif

#define	C_STRINGIFY_ARG(contents)                               #contents
#define C_STRINGIFY(macro_or_string)                            C_STRINGIFY_ARG (macro_or_string)


#if C_CHECK_VERSION(2, 8)
#define C_EXTENSION                                             __extension__
#else
#define C_EXTENSION
#endif

#ifdef __has_feature
#define c_macro__has_feature                                    __has_feature
#else
#define c_macro__has_feature(x)                                 0
#endif

#ifdef __has_builtin
#define c_macro__has_builtin                                    __has_builtin
#else
#define c_macro__has_builtin(x)                                 0
#endif

#ifdef __has_extension
#define c_macro__has_extension                                  __has_extension
#else
#define c_macro__has_extension(x)                               0
#endif

#ifdef __has_attribute
#define c_macro__has_attribute                                  __has_attribute
#else
// 针对gcc < 5 或其它不支持 __has_attribute 属性的编译器
#define c_macro__has_attribute(x)                               c_macro__has_attribute_##x

#define c_macro__has_attribute___pure__                         C_CHECK_VERSION (2, 96)
#define c_macro__has_attribute___malloc__                       C_CHECK_VERSION (2, 96)
#define c_macro__has_attribute___noinline__                     C_CHECK_VERSION (2, 96)
#define c_macro__has_attribute___sentinel__                     C_CHECK_VERSION (4, 0)
#define c_macro__has_attribute___alloc_size__                   C_CHECK_VERSION (4, 3)
#define c_macro__has_attribute___format__                       C_CHECK_VERSION (2, 4)
#define c_macro__has_attribute___format_arg__                   C_CHECK_VERSION (2, 4)
#define c_macro__has_attribute___noreturn__                     (C_CHECK_VERSION (2, 8) || (0x5110 <= __SUNPRO_C))
#define c_macro__has_attribute___const__                        C_CHECK_VERSION (2, 4)
#define c_macro__has_attribute___unused__                       C_CHECK_VERSION (2, 4)
#define c_macro__has_attribute___no_instrument_function__       C_CHECK_VERSION (2, 4)
#define c_macro__has_attribute_fallthrough                      C_CHECK_VERSION (6, 0)
#define c_macro__has_attribute___deprecated__                   C_CHECK_VERSION (3, 1)
#define c_macro__has_attribute_may_alias                        C_CHECK_VERSION (3, 3)
#define c_macro__has_attribute_warn_unused_result               C_CHECK_VERSION (3, 4)
#define c_macro__has_attribute_cleanup                          C_CHECK_VERSION (3, 3)

#endif

/**
 * @brief
 *  pure 函数返回值仅仅依赖输入参数或全局变量，仅支持gcc编译器
 *  
 *  bool c_type_check_value (const CValue *value) C_GNUC_PURE;
 * 
 * @see
 *  the [GNU C documentation](https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html#index-pure-function-attribute) for more details.
 */
#if c_macro__has_attribute(__pure__)
#define C_PURE                                                  __attribute__((__pure__))
#else
#define C_PURE
#endif

/**
 * @brief
 *  void* c_malloc (unsigned long n_bytes) C_GNUC_MALLOC C_GNUC_ALLOC_SIZE(1);
 * @see
 *  [GNU C `malloc` function attribute](https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html#index-functions-that-behave-like-malloc)
 */
#if c_macro__has_attribute(__malloc__)
#define C_MALLOC                                                __attribute__ ((__malloc__))
#else
#define C_MALLOC
#endif

/**
 * @brief
 *  char* c_strconcat (const char *string1, ...) C_GNUC_NULL_TERMINATED;
 *
 * @see
 *  the [GNU C documentation](https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html#index-sentinel-function-attribute) for more details.
 */
#if c_macro__has_attribute(__sentinel__)
#define C_NULL_TERMINATED                                       __attribute__((__sentinel__))
#else
#define C_NULL_TERMINATED
#endif

/**
 * @brief
 *  void* c_malloc_n (int n_blocks, int n_block_bytes) C_GNUC_MALLOC C_GNUC_ALLOC_SIZE2(1, 2);
 * 
 * @see
 *  the [GNU C documentation](https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html#index-alloc_005fsize-function-attribute) for more details.
 */
#if c_macro__has_attribute(__alloc_size__)
#define C_ALLOC_SIZE(x)                                         __attribute__((__alloc_size__(x)))
#define C_ALLOC_SIZE2(x,y)                                      __attribute__((__alloc_size__(x,y)))
#else
#define C_ALLOC_SIZE(x)
#define C_ALLOC_SIZE2(x,y)
#endif

/**
 * @brief
 *  @param format_idx: 表示第几个参数为 "格式化" 参数(index从1开始)
 *  @param arg_idx: 表示第几个参数为 "第一个变参" 参数(index从1开始)，没有则为0
 *  int g_snprintf (char* string, unsigned long n, char const *format, ...) C_GNUC_PRINTF (3, 4);
 * 
 * @see
 *  [GNU C documentation](https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html#index-Wformat-3288)
 */
#if c_macro__has_attribute(__format__)
#if !defined (__clang__) && C_CHECK_VERSION (4, 4)
#define C_PRINTF( format_idx, arg_idx )                         __attribute__((__format__ (gnu_printf, format_idx, arg_idx)))
#define C_SCANF( format_idx, arg_idx )                          __attribute__((__format__ (gnu_scanf, format_idx, arg_idx)))
#define C_STRFTIME( format_idx )                                __attribute__((__format__ (gnu_strftime, format_idx, 0)))
#else
#define C_PRINTF( format_idx, arg_idx )                         __attribute__((__format__ (__printf__, format_idx, arg_idx)))
#define C_SCANF( format_idx, arg_idx )                          __attribute__((__format__ (__scanf__, format_idx, arg_idx)))
#define C_STRFTIME( format_idx )                                __attribute__((__format__ (__strftime__, format_idx, 0)))
#endif
#else
//
#define C_PRINTF( format_idx, arg_idx )
#define C_SCANF( format_idx, arg_idx )
#define C_STRFTIME( format_idx )
#endif


/**
 * @brief
 *  char* c_dgettext (char *domain_name, char *msgid) G_GNUC_FORMAT (2);
 *
 * @see
 *  [GNU C documentation](https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html#index-Wformat-nonliteral-1) for more details.
 */
#if c_macro__has_attribute(__format_arg__)
#define C_FORMAT(arg_idx)                                       __attribute__ ((__format_arg__ (arg_idx)))
#else
#define C_FORMAT( arg_idx )
#endif


/**
 * @brief
 *  告知编译器，函数无返回参数
 *  void c_abort (void) C_GNUC_NORETURN;
 *
 * @see
 *  [GNU C documentation](https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html#index-noreturn-function-attribute) for more details.
 *  
 */
#if c_macro__has_attribute(__noreturn__)
#define C_NORETURN                                              __attribute__ ((__noreturn__))
#else
#define C_NORETURN
#endif

/**
 * @brief
 *  char c_ascii_tolower (char c) C_GNUC_CONST;
 *
 * @see
 *  [GNU C documentation](https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html#index-const-function-attribute) for more details.
 */
#if c_macro__has_attribute(__const__)
#define C_CONST                                                 __attribute__ ((__const__))
#else
#define C_CONST
#endif

/**
 * @brief
 *  void my_unused_function (C_GNUC_UNUSED gint unused_argument, gint other_argument) C_GNUC_UNUSED;
 *
 * @see
 *  [GNU C documentation](https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html#index-unused-function-attribute) for more details.
 */
#if c_macro__has_attribute(__unused__)
#define C_UNUSED                                                __attribute__ ((__unused__))
#else
#define C_UNUSED
#endif

/**
 * @brief
 *  添加此属性，函数无法被分析
 *  int do_uninteresting_things (void) C_GNUC_NO_INSTRUMENT;
 *
 * @see 
 *  [GNU C documentation](https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html#index-no_005finstrument_005ffunction-function-attribute) for more details.
 *
 */
#if c_macro__has_attribute(__no_instrument_function__)
#define C_NO_INSTRUMENT                                         __attribute__ ((__no_instrument_function__))
#else
#define C_NO_INSTRUMENT
#endif

/**
 * @brief
 *  添加此属性，允许 switch - case不中断此 case
 *  
 *  gcc 打开 `-Wimplicit-fallthrough` 属性使用此功能
 *
 *  switch (foo)
 *  {
 *     case 1:
 *       printf ("it's 1\n");
 *       C_GNUC_FALLTHROUGH;
 *     case 2:
 *       printf("it's either 1 or 2\n");
 *       break;
 *  }
 *
 * @see
 *  [GNU C documentation](https://gcc.gnu.org/onlinedocs/gcc/Statement-Attributes.html#index-fallthrough-statement-attribute) for more details.
 *
 */
#if c_macro__has_attribute(fallthrough)
#define C_FALLTHROUGH                                           __attribute__((fallthrough))
#else
#define C_FALLTHROUGH 
#endif

/**
 * @brief
 *  指定函数已经过时
 *
 *  gcc 打开 `-Wdeprecated-declarations` 属性使用此功能
 *
 *  int my_mistake (void) C_GNUC_DEPRECATED;
 *
 * @See
 *  [GNU C documentation](https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html#index-deprecated-function-attribute) for more details.
 */
#if c_macro__has_attribute(__deprecated__)
#define C_DEPRECATED                                            __attribute__((__deprecated__))
#else
#define C_DEPRECATED
#endif 

/**
 * @brief
 *  当返回值未使用时候，编译器发出警告
 *
 *  CList *c_list_append (CList *list, void* data) C_GNUC_WARN_UNUSED_RESULT;
 *
 * @see 
 *  [GNU C documentation](https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html#index-warn_005funused_005fresult-function-attribute) for more details.
 *
 */
#if c_macro__has_attribute(warn_unused_result)
#define C_WARN_UNUSED_RESULT                                    __attribute__((warn_unused_result))
#else
#define C_WARN_UNUSED_RESULT
#endif

/**
 * @brief
 *  当前函数名字符串
 */
#if defined (__GNUC__) && defined (__cplusplus)
#define C_STRFUNC                                               ((const char*) (__PRETTY_FUNCTION__))
#elif defined (__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#define C_STRFUNC                                               ((const char*) (__func__))
#elif defined (__GNUC__) || (defined(_MSC_VER) && (_MSC_VER > 1300))
#define C_STRFUNC                                               ((const char*) (__FUNCTION__))
#else
#define C_STRFUNC                                               ((const char*) ("???"))
#endif

/**
 * @brief
 *  当前行号
 */
#define C_LINE                                                  C_STRINGIFY (__LINE__)

/**
 * @brief
 *  当前文件中位置
 */
#if defined(__GNUC__) && (__GNUC__ < 3) && !defined(__cplusplus)
#define C_STRLOC                                                __FILE__ ":" C_STRINGIFY (__LINE__) ":" __PRETTY_FUNCTION__ "()"
#else
#define C_STRLOC                                                __FILE__ ":" C_STRINGIFY (__LINE__)
#endif

#ifdef  __cplusplus
#define C_BEGIN_EXTERN_C                                        extern "C" {
#define C_END_EXTERN_C                                          }
#else
#define C_BEGIN_EXTERN_C
#define C_END_EXTERN_C
#endif


#ifndef __GI_SCANNER__ /* The static assert macro really confuses the introspection parser */
#define C_PASTE_ARGS(identifier1,identifier2)                   identifier1 ## identifier2
#define C_PASTE(identifier1,identifier2)                        C_PASTE_ARGS (identifier1, identifier2)
#if !defined(__cplusplus) && defined(__STDC_VERSION__) && \
    (__STDC_VERSION__ >= 201112L || c_macro__has_feature(c_static_assert) || c_macro__has_extension(c_static_assert))
#define C_STATIC_ASSERT(expr)                                   _Static_assert (expr, "Expression evaluates to false")
#elif (defined(__cplusplus) && __cplusplus >= 201103L) || \
      (defined(__cplusplus) && defined (_MSC_VER) && (_MSC_VER >= 1600)) || \
      (defined (_MSC_VER) && (_MSC_VER >= 1800))
#define C_STATIC_ASSERT(expr)                                   static_assert (expr, "Expression evaluates to false")
#else
#ifdef __COUNTER__
#define C_STATIC_ASSERT(expr)                                   typedef char C_PASTE (_GStaticAssertCompileTimeAssertion_, __COUNTER__)[(expr) ? 1 : -1] C_UNUSED
#else
#define C_STATIC_ASSERT(expr)                                   typedef char C_PASTE (_GStaticAssertCompileTimeAssertion_, __LINE__)[(expr) ? 1 : -1] C_UNUSED
#endif
#endif /* __STDC_VERSION__ */
#define C_STATIC_ASSERT_EXPR(expr)                              ((void) sizeof (char[(expr) ? 1 : -1]))
#endif /* !__GI_SCANNER__ */



/**
 * @brief 
 *  提供 NULL 定义
 */
#ifndef NULL
#ifdef __cplusplus
#define NULL                                                    (nullptr)
#else
#define NULL                                                    ((void*)0)
#endif
#endif

/**
 * @brief
 *  定义 false
 */

#ifdef __cplusplus
#define false                                                   false 
#else
#undef false
#define false                                                   (0)
#endif

/**
 * @brief bool
 */
#ifdef __cplusplus
#else 
#ifndef bool
typedef int                                                     bool;
#endif
#endif

/**
 * @brief
 *  定义 true
 */
#ifdef __cplusplus
#define true                                                    true
#else
#undef true
#define true                                                    (!false)
#endif

/**
 * @brief
 *  一些常用基础类型的简写
 */
typedef unsigned char                                           cuchar;
typedef unsigned short                                          cushort;
typedef unsigned long                                           culong;
typedef unsigned int                                            cuint;


/**
 * @brief
 *  定义: MAX(a, b)
 */
#undef MAX
#define MAX(a, b)                                               (((a) > (b)) ? (a) : (b))

#undef MIN
#define MIN(a, b)                                               (((a) < (b)) ? (a) : (b))

#undef ABS
#define ABS(a)                                                  (((a) < 0) ? -(a) : (a))

/**
 * @brief
 *  获取数组容量
 */
#undef C_N_ELEMENTS
#define C_N_ELEMENTS(arr)                                       (sizeof(arr) / sizeof((arr)[0]))

/**
 * @brief
 *  指针转整型 && 整型转指针
 */
#undef C_POINTER_TO_SIZE           
#define C_POINTER_TO_SIZE(p)                                    ((unsigned long)(p))

#undef C_SIZE_TO_POINTER
#define C_SIZE_TO_POINTER(s)                                    ((void*) (unsigned long) (s))


#endif
