/**
 * KFR (http://kfrlib.com)
 * Copyright (C) 2016  D Levin
 * See LICENSE.txt for details
 */

#include <kfr/testo/testo.hpp>

#include <kfr/base.hpp>
#include <kfr/io.hpp>

#define MPFR_THREAD_LOCAL
#include "mpfr/mpfrplus.hpp"

using namespace kfr;

using vector_types = ctypes_t<f32, f64, f32x2, f32x8, f32x16, f64x2, f64x4, f64x8>;

template <typename T>
double ulps(T test, const mpfr::number& ref)
{
    if (std::isnan(test) && ref.isnan())
        return 0;
    if (std::isinf(test) && ref.isinfinity())
        return (test < 0) == (ref < 0) ? 0 : NAN;
    return static_cast<double>(mpfr::abs(mpfr::number(test) - ref) /
                               mpfr::abs(mpfr::number(test) - std::nexttoward(test, HUGE_VALL)));
}

template <typename T, size_t N>
double ulps(const vec<T, N>& test, const mpfr::number& ref)
{
    double u = 0;
    for (size_t i = 0; i < N; ++i)
        u = std::max(u, ulps(test[i], ref));
    return u;
}

TEST(test_sin_cos)
{
    testo::matrix(named("type")  = vector_types(),
                  named("value") = make_range(0.0, +constants<f64>::pi * 2, 0.05),
                  [](auto type, double value) {
                      using T = type_of<decltype(type)>;
                      const T x(value);
                      CHECK(ulps(kfr::sin(x), mpfr::sin(subtype<T>(value))) < 2.0);
                      CHECK(ulps(kfr::cos(x), mpfr::cos(subtype<T>(value))) < 2.0);
                  });
    testo::matrix(named("type") = vector_types(), named("value") = make_range(-100.0, 100.0, 0.5),
                  [](auto type, double value) {
                      using T = type_of<decltype(type)>;
                      const T x(value);
                      CHECK(ulps(kfr::sin(x), mpfr::sin(subtype<T>(value))) < 2.0);
                      CHECK(ulps(kfr::cos(x), mpfr::cos(subtype<T>(value))) < 2.0);
                  });
}

TEST(test_tan)
{
    testo::matrix(named("type")  = ctypes_t<f32>(),
                  named("value") = make_range(0.0, +constants<f64>::pi * 2, 0.01),
                  [](auto type, double value) {
                      using T = type_of<decltype(type)>;
                      const T x(value);
                      CHECK(ulps(kfr::tan(x), mpfr::tan(subtype<T>(value))) < 2.0);
                  });
    testo::matrix(named("type") = ctypes_t<f32>(), named("value") = make_range(-100.0, 100.0, 0.5),
                  [](auto type, double value) {
                      using T = type_of<decltype(type)>;
                      const T x(value);
                      CHECK(ulps(kfr::tan(x), mpfr::tan(subtype<T>(value))) < 3.0);
                  });
}

#ifdef __clang__
#define ARCFN_ULP 2.0
#else
#define ARCFN_ULP 2.5
#endif

TEST(test_asin_acos_atan)
{
    testo::matrix(named("type") = vector_types(), named("value") = make_range(-1.0, 1.0, 0.05),
                  [](auto type, double value) {
                      using T = type_of<decltype(type)>;
                      const T x(value);
                      CHECK(ulps(kfr::asin(x), mpfr::asin(subtype<T>(value))) < ARCFN_ULP);
                      CHECK(ulps(kfr::acos(x), mpfr::acos(subtype<T>(value))) < ARCFN_ULP);
                      CHECK(ulps(kfr::atan(x), mpfr::atan(subtype<T>(value))) < ARCFN_ULP);
                  });
}

TEST(test_atan2)
{
    testo::matrix(named("type") = vector_types(), named("value1") = make_range(-1.0, 1.0, 0.1),
                  named("value2") = make_range(-1.0, 1.0, 0.1), [](auto type, double value1, double value2) {
                      using T = type_of<decltype(type)>;
                      const T x(value1);
                      const T y(value2);
                      CHECK(ulps(kfr::atan2(x, y), mpfr::atan2(subtype<T>(value1), subtype<T>(value2))) <
                            ARCFN_ULP);
                  });
}

TEST(test_log)
{
    testo::matrix(named("type") = ctypes_t<f32, f64>(), named("value") = make_range(0.0, 100.0, 0.5),
                  [](auto type, double value) {
                      using T = type_of<decltype(type)>;
                      const T x(value);
                      CHECK(ulps(kfr::log(x), mpfr::log(x)) < 2.0);
                  });
}

TEST(test_log2)
{
    testo::matrix(named("type") = ctypes_t<f32, f64>(), named("value") = make_range(0.0, 100.0, 0.5),
                  [](auto type, double value) {
                      using T = type_of<decltype(type)>;
                      const T x(value);
                      CHECK(ulps(kfr::log2(x), mpfr::log2(x)) < 3.0);
                  });
}

TEST(test_log10)
{
    testo::matrix(named("type") = ctypes_t<f32, f64>(), named("value") = make_range(0.0, 100.0, 0.5),
                  [](auto type, double value) {
                      using T = type_of<decltype(type)>;
                      const T x(value);
                      CHECK(ulps(kfr::log10(x), mpfr::log10(x)) < 3.0);
                  });
}

TEST(test_exp)
{
    testo::matrix(named("type") = ctypes_t<f32, f64>(), named("value") = make_range(-10, +10, 0.05),
                  [](auto type, double value) {
                      using T = type_of<decltype(type)>;
                      const T x(value);
                      CHECK(ulps(kfr::exp(x), mpfr::exp(x)) < 2.0);
                  });
}

TEST(test_exp2)
{
    testo::matrix(named("type") = ctypes_t<f32, f64>(), named("value") = make_range(-10, +10, 0.05),
                  [](auto type, double value) {
                      using T = type_of<decltype(type)>;
                      const T x(value);
                      CHECK(ulps(kfr::exp2(x), mpfr::exp2(x)) < 3.0);
                  });
}

TEST(test_exp10)
{
    testo::matrix(named("type") = ctypes_t<f32, f64>(), named("value") = make_range(-10, +10, 0.05),
                  [](auto type, double value) {
                      using T = type_of<decltype(type)>;
                      const T x(value);
                      CHECK(ulps(kfr::exp10(x), mpfr::exp10(x)) < 3.0);
                  });
}

#ifndef KFR_NO_MAIN
int main()
{
    println(library_version(), " running on ", cpu_runtime());
    mpfr::scoped_precision p(128);
    return testo::run_all("");
}
#endif
