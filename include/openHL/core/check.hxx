#pragma once

#include "openHL/core/base.hxx"

namespace hl
{

#define HL_TEST_EQ(v1, v2) ((v1) == (v2))
#define HL_TEST_NE(v1, v2) ((v1) != (v2))
#define HL_TEST_LE(v1, v2) ((v1) <= (v2))
#define HL_TEST_LT(v1, v2) ((v1) < (v2))
#define HL_TEST_GE(v1, v2) ((v1) >= (v2))
#define HL_TEST_GT(v1, v2) ((v1) > (v2))

#define HL_CHECK(v1, op, v2, msg)                                                          \
    do {                                                                                   \
        if (!!(HL_TEST_##op((v1), (v2))))                                                  \
            ;                                                                              \
        else hl::error(hl::Error::StsError, msg, __PRETTY_FUNCTION__, __FILE__, __LINE__); \
    }                                                                                      \
    while (0)

#define HL__CHECK_CUSTOM_TEST(test_expr, msg)                                              \
    do {                                                                                   \
        if (!!(test_expr))                                                                 \
            ;                                                                              \
        else hl::error(hl::Error::StsError, msg, __PRETTY_FUNCTION__, __FILE__, __LINE__); \
    }                                                                                      \
    while (0)

/// Supported values of these types: int, float, double
#define HL_CheckEQ(v1, v2, msg)       HL_CHECK(v1, EQ, v2, msg)
#define HL_CheckNE(v1, v2, msg)       HL_CHECK(v1, NE, v2, msg)
#define HL_CheckLE(v1, v2, msg)       HL_CHECK(v1, LE, v2, msg)
#define HL_CheckLT(v1, v2, msg)       HL_CHECK(v1, LT, v2, msg)
#define HL_CheckGE(v1, v2, msg)       HL_CHECK(v1, GE, v2, msg)
#define HL_CheckGT(v1, v2, msg)       HL_CHECK(v1, GT, v2, msg)

#define HL_CheckTypeEQ(t1, t2, msg)   HL_CHECK(t1, EQ, t2, msg)
#define HL_CheckType(test_expr, msg)  HL__CHECK_CUSTOM_TEST(test_expr, msg)
#define HL_CheckDepth(test_expr, msg) HL__CHECK_CUSTOM_TEST(test_expr, msg)
#define HL_Check(test_expr, msg)      HL__CHECK_CUSTOM_TEST(test_expr, msg)

#ifndef NDEBUG
    #define HL_DbgCheck(test_expr, msg) HL_Check(test_expr, msg)
    #define HL_DbgCheckEQ(v1, v2, msg)  HL_CHECK(v1, EQ, v2, msg)
    #define HL_DbgCheckNE(v1, v2, msg)  HL_CHECK(v1, NE, v2, msg)
    #define HL_DbgCheckLE(v1, v2, msg)  HL_CHECK(v1, LE, v2, msg)
    #define HL_DbgCheckLT(v1, v2, msg)  HL_CHECK(v1, LT, v2, msg)
    #define HL_DbgCheckGE(v1, v2, msg)  HL_CHECK(v1, GE, v2, msg)
    #define HL_DbgCheckGT(v1, v2, msg)  HL_CHECK(v1, GT, v2, msg)
#else
    #define HL_DbgCheck(v, test_expr, msg)
    #define HL_DbgCheckEQ(v1, v2, msg)
    #define HL_DbgCheckNE(v1, v2, msg)
    #define HL_DbgCheckLE(v1, v2, msg)
    #define HL_DbgCheckLT(v1, v2, msg)
    #define HL_DbgCheckGE(v1, v2, msg)
    #define HL_DbgCheckGT(v1, v2, msg)
#endif

}    // namespace hl