// Math functions providing constexpr functionality when possible.
// Filename to prevent confusion with C math.h header.
#ifndef GB_MATH_H
#define GB_MATH_H

// As of C++17 the std algorithms support constexpr
#include <algorithm>

// The std math library does not have constexpr but this one does.
// Unfortunately it gives some warnings with Clang 11
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wimplicit-int-float-conversion"    
#include "../gcem/include/gcem.hpp"
#pragma clang diagnostic pop

namespace Greenbell {
// glm is a better alternative to these unless constexpr is needed and glm is
// using SIMD optimizations. Performance is not usually a concern for constexpr.
// These are very limited in operations and only support xyzw coordinates but
// work with constexpr functions below.
struct Vec3 {
    float x;
    float y;
    float z;
    constexpr Vec3() noexcept : x{0.0f}, y{0.0f}, z{0.0f} {}
    constexpr Vec3(float a, float b, float c) noexcept : x{a}, y{b}, z{c} {}
};
constexpr Vec3 operator* (Vec3 const& v, float a) {
    return Vec3{v.x * a, v.y * a, v.z * a};
}
constexpr Vec3 operator* (float a, Vec3 const& v) {
    return Vec3{v.x * a, v.y * a, v.z * a};
}
struct Vec4 {
    float x;
    float y;
    float z;
    float w;
    constexpr Vec4() noexcept : x{0.0f}, y{0.0f}, z{0.0f}, w{0.0f} {}
    constexpr Vec4(float a, float b, float c, float d) noexcept : 
            x{a}, y{b}, z{c}, w{d} {}         
};
constexpr Vec4 operator* (Vec4 const& v, float a) {
    return Vec4{v.x * a, v.y * a, v.z * a, v.w * a};
}
constexpr Vec4 operator* (float a, Vec4 const& v) {
    return Vec4{v.x * a, v.y * a, v.z * a, v.w * a};
}
} // namespace Greenbell

namespace Greenbell::Math {

// glm has "convertSRGBToLinear" and "convertLinearToSRGB" but they use standard 
// cmath library so are not constexpr even when the glm vectors are
constexpr float SRGBToLinear(const float in) noexcept {
    // From sRGB spec https://www.w3.org/Graphics/Color/srgb
    if (in <= 0.04045f) {
        return in / 12.92f;
    } else {
        return gcem::pow((in + 0.055f) / 1.055f, 2.4f);
    }
}

constexpr float LinearToSRGB(const float in) noexcept {
    // From sRGB spec https://www.w3.org/Graphics/Color/srgb
    if (in <= 0.0031308f) {
        return in * 12.92f;
    } else {
        return 1.055f * gcem::pow(in, 1.0f/2.4f) - 0.055f;
    }
}

constexpr Vec3 SRGBToLinear(Vec3 const& in) noexcept {
    return Vec3{SRGBToLinear(in.x), SRGBToLinear(in.y), 
            SRGBToLinear(in.z)};
}
constexpr Vec4 SRGBToLinear(Vec4 const& in) noexcept {
    return Vec4{SRGBToLinear(in.x), SRGBToLinear(in.y), 
            SRGBToLinear(in.z), in.w};
}
constexpr Vec3 LinearToSRGB(Vec3 const& in) noexcept {
    return Vec3{LinearToSRGB(in.x), LinearToSRGB(in.y), 
            LinearToSRGB(in.z)};
}
constexpr Vec4 LinearToSRGB(Vec4 const& in) noexcept {
    return Vec4{LinearToSRGB(in.x), LinearToSRGB(in.y), 
            LinearToSRGB(in.z), in.w};
}

constexpr Vec3 TemperatureToColour(const float temperature,
        const bool linear = true) noexcept {
    // From an algorithm by Tanner Helland based on the chart by Mitchell 
    // Charity on vendian.org.
    // The algorithm approximates the 0-255 "rgb" values on the chart which
    // seem to be in sRGB colour space. The chart also has floats which seem
    // to be linear. (Running linear to sRGB conversion on the floats will
    // convert them to approximately the byte versions.)
    // Two versions of the algorithm are presented here:
    // 1. With "linear" set false it is the original Helland algorithm, adjusted
    //    to output floats in the range of 0.0f to 1.0f.
    // 2. With "linear" set true the algorithm is adjusted to approximate the
    //    floats from the chart.
    // Calculations are done as doubles because of the number of significant
    // digits provided by the coefficients with return values cast to floats.
    const double dk = static_cast<double>(temperature) / 100.0;

    // Parameters adjusted for linear based on exponent rule plus tweaks
    const auto ra = linear ? 1.81656 : 1.29293618606;
    const auto rb = linear ? -0.293050470 : -0.1332047592;
    double red = 1.0;
    if (dk > 66.0) {
        red = std::clamp(ra * gcem::pow((dk - 60.0), rb), 0.0, 1.0);
    }

    const auto ga = -0.6318414;
    const auto gb = 0.39008158;
    const auto gc = linear ? 1.33056 : 1.12989086;
    const auto gd = linear ? -0.166132668 : -0.0755148492;
    double green = 1.0;
    if (dk <= 66.0) {
        green = std::clamp(ga + gb * gcem::log(dk), 0.0, 1.0);
        if (linear) green = gcem::pow(green, 2.2); // Approximate gamma
    } else {
        green = std::clamp(gc * gcem::pow((dk - 60.0), gd), 0.0, 1.0);
    }
    
    const auto ba = -1.19625409;
    const auto bb = 0.54320679;
    double blue = 1.0;
    if (dk < 66.0) {
        if (dk <= 19.0) {
            blue = 0.0;
        } else {
            blue = std::clamp(ba + bb * gcem::log(dk - 10.0), 0.0, 1.0);
            if (linear) blue = gcem::pow(blue, 2.2); // Approximate gamma
        }
    }
  
    return Vec3{static_cast<float>(red), static_cast<float>(green), 
            static_cast<float>(blue)};
}

constexpr Vec3 LinearColourTemperature(const float temperature) {
    return TemperatureToColour(temperature, true);
}

constexpr Vec3 SRGBColourTemperature(const float temperature) {
    return TemperatureToColour(temperature, false);
}

} // namespace Greenbell::Math

#endif
