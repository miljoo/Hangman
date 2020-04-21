
#include "room.hpp"
#include <pmmintrin.h>



inline void SSESqrt( float * pOut, float * pIn )
{
   _mm_store_ss( pOut, _mm_sqrt_ss( _mm_load_ss( pIn ) ) );
}

float hypotenuse(float x1, float y1, float x2, float y2){
    V2f p1(x1,y1);
    V2f p2(x2,y2);
    float result = 0;
    float length = ((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));

    SSESqrt(&result, &length);

    return result;
}

sf::Vector2f relPosition(float relx, float rely, float offsetx, float offsety)
{
    sf::Vector2f result;
    float x = (relx * SCREEN_W) + offsetx;
    float y = (rely * SCREEN_H) + offsety;
    result.x = x;
    result.y = y;
    return result;
}

float centerX(float size){
    float result = ((SCREEN_W - size) / 2) / SCREEN_W;
    return result;
}

float centerY(float size){
    float result = ((SCREEN_H - size) / 2) / SCREEN_H;
    return result;
}
