#include "gb_math.h"
#include "gb_fmt.h"

int main() {

    // Some math from a colour temperature chart
    constexpr Greenbell::Vec3 cf{1.0f, 0.3992f, 0.0950f};
    constexpr auto cf2l = Greenbell::Math::SRGBToLinear(cf);
    constexpr auto cf2s = Greenbell::Math::LinearToSRGB(cf);    
    constexpr auto result1a = Greenbell::Math::SRGBColourTemperature(2700.0f);
    constexpr auto result1b = Greenbell::Math::LinearColourTemperature(2700.0f);
    
    // Prove that this was calculated at compile time
    static_assert(result1a.x > 0.99f && result1a.x < 1.01f); 
        
    fmt::print("Chart float                      R={:f} G={:f} B={:f}\n", 
            cf.x, cf.y, cf.z);
    fmt::print("Chart float to linear            R={:f} G={:f} B={:f}\n", 
            cf2l.x, cf2l.y, cf2l.z);
    fmt::print("Chart float to sRGB              R={:f} G={:f} B={:f}\n", 
            cf2s.x, cf2s.y, cf2s.z);            
    fmt::print("SRGBColourTemperature(2700.0f)   R={:f} G={:f} B={:f}\n", 
            result1a.x, result1a.y, result1a.z);
    fmt::print("LinearColourTemperature(2700.0f) R={:f} G={:f} B={:f}\n", 
            result1b.x, result1b.y, result1b.z);            

    return 0;
}
