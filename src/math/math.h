#pragma once

#include "hop.h"
#include "types.h"
#include <cmath>
#include <limits>
#include <cfloat>
#include <cstdint>
#include <type_traits>
#include <algorithm>

#include <emmintrin.h>
#include <xmmintrin.h>
#include <immintrin.h>

#if defined(__clang__) || defined(__GNUC__)
#   define MAYBE_UNUSED __attribute__((unused))
#else
#   define MAYBE_UNUSED
#endif

namespace hop {

struct M128
{
    union
    {
        __m128      m128;
        __m128d     m128d;
        __m128i     m128i;

        float       f32[4];
        double      f64[2];

        int8        i8[16];
        int16       i16[8];
        int32       i32[4];
        int64       i64[2];
        uint8       u8[16];
        uint16      u16[8];
        uint32      u32[4];
        uint64      u64[2];
    };
};

struct M256
{
    union
    {
        __m256      m256;
        __m256d     m256d;
        __m256i     m256i;

        float       f32[8];
        double      f64[4];

        int8        i8[32];
        int16       i16[16];
        int32       i32[8];
        int64       i64[4];
        uint8       u8[32];
        uint16      u16[16];
        uint32      u32[8];
        uint64      u64[4];
    };
};

int log2(int n);

template <typename T>
bool is_power_of_2(T x)
{
    return x > 0 && !(x & (x - 1));
}

template<class T>
typename std::enable_if<!std::numeric_limits<T>::is_integer, bool>::type almost_equal(T x, T y, int ulp = 1)
{
    // the machine epsilon has to be scaled to the magnitude of the values used
    // and multiplied by the desired precision in ULPs (units in the last place)
    return std::abs(x-y) < std::numeric_limits<T>::epsilon() * std::abs(x+y) * ulp
    // unless the result is subnormal
           || std::abs(x-y) < std::numeric_limits<T>::min();
}

static struct NegInfType
{
    inline operator float   () const { return -std::numeric_limits<float>::infinity();  }
    inline operator double  () const { return -std::numeric_limits<double>::infinity(); }
    inline operator int8_t  () const { return  std::numeric_limits<int8_t>::min();      }
    inline operator int16_t () const { return  std::numeric_limits<int16_t>::min();     }
    inline operator int32_t () const { return  std::numeric_limits<int32_t>::min();     }
    inline operator int64_t () const { return  std::numeric_limits<int64_t>::min();     }
    inline operator uint8_t () const { return  std::numeric_limits<uint8_t>::min();     }
    inline operator uint16_t() const { return  std::numeric_limits<uint16_t>::min();    }
    inline operator uint32_t() const { return  std::numeric_limits<uint32_t>::min();    }
    inline operator uint64_t() const { return  std::numeric_limits<uint64_t>::min();    }
} neg_inf MAYBE_UNUSED;

static struct PosInfType
{
    inline operator float   () const { return std::numeric_limits<float>::infinity();  }
    inline operator double  () const { return std::numeric_limits<double>::infinity(); }
    inline operator int8_t  () const { return std::numeric_limits<int8_t>::max();      }
    inline operator int16_t () const { return std::numeric_limits<int16_t>::max();     }
    inline operator int32_t () const { return std::numeric_limits<int32_t>::max();     }
    inline operator int64_t () const { return std::numeric_limits<int64_t>::max();     }
    inline operator uint8_t () const { return std::numeric_limits<uint8_t>::max();     }
    inline operator uint16_t() const { return std::numeric_limits<uint16_t>::max();    }
    inline operator uint32_t() const { return std::numeric_limits<uint32_t>::max();    }
    inline operator uint64_t() const { return std::numeric_limits<uint64_t>::max();    }
} pos_inf MAYBE_UNUSED;

static struct PiType
{
    inline operator double() const { return double(M_PI); }
    inline operator float () const { return  float(M_PI); }
} pi MAYBE_UNUSED;

static struct OneOverPiType
{
    inline operator double() const { return double(M_1_PI); }
    inline operator float () const { return  float(M_1_PI); }
} one_over_pi MAYBE_UNUSED;

inline int cast_f2i(float f) {
    union { float f; int i; } v; v.f = f; return v.i;
}

inline float cast_i2f(int i) {
    union { float f; int i; } v; v.i = i; return v.f;
}

inline float sign(const float x) { return x < 0 ? -1.0f : 1.0f; }
inline float sqr (const float x) { return x * x; }

inline float rcp(const float x)
{
    const __m128 a = _mm_set_ss(x);
    const __m128 r = _mm_rcp_ps(a);
    return _mm_cvtss_f32(_mm_mul_ps(r, _mm_sub_ps(_mm_set_ss(2.0f), _mm_mul_ps(r, a))));
}

inline float rsqrt(const float x)
{
    // Newton Raphson iteration over rsqrt to increase the accuracy ret = y * (1.5 - 0.5 * x * y * y), with y = 1/sqrt(x)
    const __m128 a = _mm_set_ss(x);
    const __m128 r = _mm_rsqrt_ps(a);
    const __m128 c = _mm_add_ps(_mm_mul_ps(_mm_set_ps(1.5f, 1.5f, 1.5f, 1.5f), r),
                                _mm_mul_ps(_mm_mul_ps(_mm_mul_ps(a, _mm_set_ps(-0.5f, -0.5f, -0.5f, -0.5f)), r), _mm_mul_ps(r, r)));
    return _mm_cvtss_f32(c);
}

inline float abs  (const float x) { return std::fabs (x); }
inline float acos (const float x) { return std::acos (x); }
inline float asin (const float x) { return std::asin (x); }
inline float atan (const float x) { return std::atan (x); }
inline float atan2(const float y, const float x) { return std::atan2(y, x); }
inline float cos  (const float x) { return std::cos  (x); }
inline float cosh (const float x) { return std::cosh (x); }
inline float exp  (const float x) { return std::exp  (x); }
inline float fmod (const float x, const float y) { return std::fmod (x, y); }
inline float log  (const float x) { return std::log  (x); }
inline float log10(const float x) { return std::log10(x); }
inline float pow  (const float x, const float y) { return std::pow  (x, y); }
//inline float rcp  (const float x) { return 1.0f / x;      }
//inline float rsqrt(const float x) { return 1.0f / std::sqrt(x); }
inline float sin  (const float x) { return std::sin  (x); }
inline float sinh (const float x) { return std::sinh (x); }
inline float sqrt (const float x) { return std::sqrt (x); }
inline float tan  (const float x) { return std::tan  (x); }
inline float tanh (const float x) { return std::tanh (x); }
inline float floor(const float x) { return std::floor(x); }
inline float ceil (const float x) { return std::ceil (x); }
inline float frac (const float x) { return x-std::floor (x); }
inline float nextafter(float x, float y) { return std::nextafter(x, y); }
inline float radians(const float x) { return M_PI * x / 180.0f; }

inline double abs  (const double x) { return std::fabs (x); }
inline double sign (const double x) { return x < 0 ? -1.0 : 1.0; }
inline double acos (const double x) { return std::acos (x); }
inline double asin (const double x) { return std::asin (x); }
inline double atan (const double x) { return std::atan (x); }
inline double atan2(const double y, const double x) { return std::atan2(y, x); }
inline double cos  (const double x) { return std::cos  (x); }
inline double cosh (const double x) { return std::cosh (x); }
inline double exp  (const double x) { return std::exp  (x); }
inline double fmod (const double x, const double y) { return std::fmod (x, y); }
inline double log  (const double x) { return std::log  (x); }
inline double log10(const double x) { return std::log10(x); }
inline double pow  (const double x, const double y) { return std::pow  (x, y); }
inline double rcp  (const double x) { return 1.0 / x;    }
inline double rsqrt(const double x) { return 1.0 / std::sqrt(x); }
inline double sin  (const double x) { return std::sin  (x); }
inline double sinh (const double x) { return std::sinh (x); }
inline double sqr  (const double x) { return x * x ;        }
inline double sqrt (const double x) { return std::sqrt (x); }
inline double tan  (const double x) { return std::tan  (x); }
inline double tanh (const double x) { return std::tanh (x); }
inline double floor(const double x) { return std::floor(x); }
inline double ceil (const double x) { return std::ceil (x); }
inline double nextafter(double x, double y) { return std::nextafter(x, y); }
inline double radians(const double x) { return M_PI * x / 180.0; }

inline float madd (const float a, const float b, const float c) { return  a * b + c; }
inline float msub (const float a, const float b, const float c) { return  a * b - c; }
inline float nmadd(const float a, const float b, const float c) { return -a * b + c; }
inline float nmsub(const float a, const float b, const float c) { return -a * b - c; }

inline bool  select(bool s, bool  t , bool f) { return s ? t : f; }
inline int   select(bool s, int   t,   int f) { return s ? t : f; }
inline float select(bool s, float t, float f) { return s ? t : f; }

#if defined(__SSE4_1__)
    inline float mini(float a, float b) {
        const __m128i ai = _mm_castps_si128(_mm_set_ss(a));
        const __m128i bi = _mm_castps_si128(_mm_set_ss(b));
        const __m128i ci = _mm_min_epi32(ai,bi);
        return _mm_cvtss_f32(_mm_castsi128_ps(ci));
    }
#endif

#if defined(__SSE4_1__)
    inline float maxi(float a, float b) {
        const __m128i ai = _mm_castps_si128(_mm_set_ss(a));
        const __m128i bi = _mm_castps_si128(_mm_set_ss(b));
        const __m128i ci = _mm_max_epi32(ai,bi);
        return _mm_cvtss_f32(_mm_castsi128_ps(ci));
    }
#endif

/*
//inline    float min(float    a, float    b) { return a < b ? a : b; }
//inline   double min(double   a, double   b) { return a < b ? a : b; }
inline   int8_t min(int8_t   a, int8_t   b) { return a < b ? a : b; }
inline  int16_t min(int16_t  a, int16_t  b) { return a < b ? a : b; }
inline  int32_t min(int32_t  a, int32_t  b) { return a < b ? a : b; }
inline  int64_t min(int64_t  a, int64_t  b) { return a < b ? a : b; }
inline  uint8_t min(uint8_t  a, uint8_t  b) { return a < b ? a : b; }
inline uint16_t min(uint16_t a, uint16_t b) { return a < b ? a : b; }
inline uint32_t min(uint32_t a, uint32_t b) { return a < b ? a : b; }
inline uint64_t min(uint64_t a, uint64_t b) { return a < b ? a : b; }
*/

template <typename T>
inline T min(T a, T b)
{
    return a < b ? a : b;
    //return std::min(a, b);
}

template <typename T>
inline T max(T a, T b)
{
    return a > b ? a : b;
    //return std::max(a, b);
}
/*
template <>
inline float min(float left, float right)
{
    float ret;
    _mm_store_ss(&ret, _mm_min_ss(_mm_set_ss(left), _mm_set_ss(right)));
    return ret;
}

template <>
inline float max(float left, float right)
{
    float ret;
    _mm_store_ss(&ret, _mm_max_ss(_mm_set_ss(left), _mm_set_ss(right)));
    return ret;
}

template <>
inline double min(double left, double right)
{
    double ret;
    _mm_store_sd(&ret, _mm_min_sd(_mm_set_sd(left), _mm_set_sd(right)));
    return ret;
}

template <>
inline double max(double left, double right)
{
    double ret;
    _mm_store_sd(&ret, _mm_max_sd(_mm_set_sd(left), _mm_set_sd(right)));
    return ret;
}
*/

template<typename T> inline T min(const T& a, const T& b, const T& c) { return min(min(a, b), c); }
template<typename T> inline T min(const T& a, const T& b, const T& c, const T& d) { return min(min(a, b), min(c, d)); }
template<typename T> inline T min(const T& a, const T& b, const T& c, const T& d, const T& e) { return min(min(min(a, b), min(c, d)), e); }

template<typename T> inline T mini(const T& a, const T& b, const T& c) { return mini(mini(a, b), c); }
template<typename T> inline T mini(const T& a, const T& b, const T& c, const T& d) { return mini(mini(a, b), mini(c, d)); }
template<typename T> inline T mini(const T& a, const T& b, const T& c, const T& d, const T& e) { return mini(mini(mini(a, b), mini(c, d)), e); }

/*
//inline    float max(float    a, float    b) { return a > b ? a : b; }
//inline   double max(double   a, double   b) { return a > b ? a : b; }
inline   int8_t max(int8_t   a, int8_t   b) { return a > b ? a : b; }
inline  int16_t max(int16_t  a, int16_t  b) { return a > b ? a : b; }
inline  int32_t max(int32_t  a, int32_t  b) { return a > b ? a : b; }
inline  int64_t max(int64_t  a, int64_t  b) { return a > b ? a : b; }
inline  uint8_t max(uint8_t  a, uint8_t  b) { return a > b ? a : b; }
inline uint16_t max(uint16_t a, uint16_t b) { return a > b ? a : b; }
inline uint32_t max(uint32_t a, uint32_t b) { return a > b ? a : b; }
inline uint64_t max(uint64_t a, uint64_t b) { return a > b ? a : b; }
*/

template<typename T> inline T max(const T& a, const T& b, const T& c) { return max(max(a, b), c); }
template<typename T> inline T max(const T& a, const T& b, const T& c, const T& d) { return max(max(a, b), max(c, d)); }
template<typename T> inline T max(const T& a, const T& b, const T& c, const T& d, const T& e) { return max(max(max(a, b), max(c, d)), e); }

template<typename T> inline T maxi(const T& a, const T& b, const T& c) { return maxi(maxi(a, b), c); }
template<typename T> inline T maxi(const T& a, const T& b, const T& c, const T& d) { return maxi(maxi(a, b), maxi(c, d)); }
template<typename T> inline T maxi(const T& a, const T& b, const T& c, const T& d, const T& e) { return maxi(maxi(maxi(a, b), maxi(c, d)), e); }

template<typename T> inline T clamp(const T& x, const T& lower = T(0), const T& upper = T(1)) { return max(min(x, upper), lower); }
template<typename T> inline T clampz(const T& x, const T& upper) { return max(T(0), min(x, upper)); }

template<typename T> inline T deg2rad(const T& x) { return x * T(1.74532925199432957692e-2f);  }
template<typename T> inline T rad2deg(const T& x) { return x * T(5.72957795130823208768e1f);   }
template<typename T> inline T sin2cos(const T& x) { return sqrt(max(T(0), T(1) - x * x)); }
template<typename T> inline T cos2sin(const T& x) { return sin2cos(x);                         }

template<typename T> T random() { return T(0); }

#ifdef RAND_HARDWARE

template<> inline float random()
{
    unsigned long long val;
    //_rdseed64_step(&val);
    _rdrand64_step(&val);
    return (float)val / (float)std::numeric_limits<unsigned long long>::max();
}

template<> inline double random()
{
    unsigned long long val;
    //_rdseed64_step(&val);
    _rdrand64_step(&val);
    return (double)val / (double)std::numeric_limits<unsigned long long>::max();
}

template<> inline uint32_t random()
{
    unsigned int val;
    _rdrand32_step(&val);
    return uint32_t(val);
}

template<> inline int random()
{
    unsigned int val;
    _rdrand32_step(&val);
    return int(val);
}

#else

template<> inline float    random() { return rand() / float(RAND_MAX); }
template<> inline double   random() { return rand() / double(RAND_MAX); }
template<> inline uint32_t random() { return uint32_t(rand()) ^ (uint32_t(rand()) << 16); }
template<> inline int      random() { return int(rand()); }

#endif

template<typename T> inline T lerp(const T& v0, const T& v1, const float t) {
    return madd(T(1.0f - t), v0, t * v1);
}

template<typename T> inline T lerp2(const float x0, const float x1, const float x2, const float x3, const T& u, const T& v) {
    return madd((1.0f - u), madd((1.0f - v), T(x0), v * T(x2)), u * madd((1.0f - v), T(x1), v * T(x3)));
}

} // namespace hop
