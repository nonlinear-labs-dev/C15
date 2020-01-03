#ifndef NL_ASSERT_H
#define NL_ASSERT_H

void sendAssertionToBB(const char *ptr);

#define STR_HELPER(x) #x
#define STR(x)        STR_HELPER(x)

#ifdef DEBUG
#define NL_ASSERT(cond) \
  if (!(cond))          \
  sendAssertionToBB(__func__)
#define NL_ASSERT_RETURN_VOID(cond) \
  if (!(cond))                      \
    sendAssertionToBB(__func__);    \
  return
#define NL_ASSERT_RETURN_VAL(cond, val) \
  if (!(cond))                          \
    sendAssertionToBB(__func__);        \
  return val
#else
#define NL_ASSERT(cond)
#define NL_ASSERT_RETURN_VOID(cond)
#define NL_ASSERT_RETURN_VAL(cond, val)
#endif

#endif  //NL_ASSERT_H
