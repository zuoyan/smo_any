#ifndef FILE_391EF435_CB2B_4254_9012_AA3ECABECACC_H
#define FILE_391EF435_CB2B_4254_9012_AA3ECABECACC_H

#define PP_STR_(X) #X
#define PP_STR(X) PP_STR_(X)

#define PP_EMPTY()
#define PP_DEFER(ID) ID PP_EMPTY()
#define PP_OBSTRUCT(...) __VA_ARGS__ PP_DEFER(PP_EMPTY)()
#define PP_EXPAND(...) __VA_ARGS__
#define PP_CALL(MACRO, ...) MACRO(__VA_ARGS__)

#define PP_CAT(A, ...) PP_CAT_(A, __VA_ARGS__)
#define PP_CAT_(A, ...) A ## __VA_ARGS__

#define PP_CHECK_N(x, n, ...) n
#define PP_CHECK(...) PP_CHECK_N(__VA_ARGS__, 0,)

#define PP_NOT(x) PP_CHECK(PP_CAT_(PP_NOT_, x))
#define PP_NOT_0 ~, 1,

#define PP_COMPL(B) PP_CAT_(PP_COMPL_, B)
#define PP_COMPL_0 1
#define PP_COMPL_1 0

#define PP_BOOL(x) PP_COMPL(PP_NOT(x))

#define PP_IF_BOOL(c) PP_CAT_(PP_IF_BOOL_, c)
#define PP_IF_BOOL_0(t, ...) __VA_ARGS__
#define PP_IF_BOOL_1(t, ...) t

#define PP_IF(c) PP_IF_BOOL(PP_BOOL(c))

#define PP_IFA_BOOL(c, t, ...) PP_CALL(PP_CAT_(PP_IF_BOOL_, c), t, __VA_ARGS__)

#define PP_IFA(c, ...) PP_IFA_BOOL(PP_BOOL(c), __VA_ARGS__)

#define PP_IS_PAREN(x) PP_CHECK(PP_IS_PAREN_ x)
#define PP_IS_PAREN_(...) ~, 1

#define PP_EVAL(...) PP_EVAL1(PP_EVAL1(PP_EVAL1(PP_EVAL1(__VA_ARGS__))))
#define PP_EVAL1(...) PP_EVAL2(PP_EVAL2(PP_EVAL2(PP_EVAL2(__VA_ARGS__))))
#define PP_EVAL2(...) PP_EVAL3(PP_EVAL3(PP_EVAL3(PP_EVAL3(__VA_ARGS__))))
#define PP_EVAL3(...) PP_EVAL4(PP_EVAL4(PP_EVAL4(PP_EVAL4(__VA_ARGS__))))
#define PP_EVAL4(...) PP_EVAL5(PP_EVAL5(PP_EVAL5(PP_EVAL5(__VA_ARGS__))))
#define PP_EVAL5(...) PP_EVAL6(PP_EVAL6(PP_EVAL6(PP_EVAL6(__VA_ARGS__))))
#define PP_EVAL6(...) PP_EVAL7(PP_EVAL7(PP_EVAL7(PP_EVAL7(__VA_ARGS__))))
#define PP_EVAL7(...) __VA_ARGS__

#define PP_FOREACH_H(MACRO, DATA, H, ...) \
  PP_IF(PP_IS_PAREN(H))                   \
  (, MACRO(DATA, H) PP_OBSTRUCT(PP_FOREACH_HH)()(MACRO, DATA, ##__VA_ARGS__))

#define PP_FOREACH_HH() PP_FOREACH_H

#define PP_FOREACH(MACRO, DATA, ...) \
  PP_EVAL(PP_FOREACH_H(MACRO, DATA, ##__VA_ARGS__, ()))

#endif // FILE_391EF435_CB2B_4254_9012_AA3ECABECACC_H
