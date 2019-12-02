#ifndef NL_ASSERT_H
#define NL_ASSERT_H

void sendAssertionToBB(const char *ptrFunc, int line);

#ifdef DEBUG
#define NL_ASSERT(cond) \
  if (!(cond))          \
  sendAssertionToBB(__func__, __LINE__)
#define NL_ASSERT_RETURN_VOID(cond)        \
  if (!(cond))                             \
    sendAssertionToBB(__func__, __LINE__); \
  return
#define NL_ASSERT_RETURN_VAL(cond, val)    \
  if (!(cond))                             \
    sendAssertionToBB(__func__, __LINE__); \
  return val
#else
#define NL_ASSERT(cond)
#define NL_ASSERT_RETURN_VOID(cond)
#define NL_ASSERT_RETURN_VAL(cond)
#endif

#endif  //NL_ASSERT_H
