#include "IE_PCH.h"
#include "CapsuleCollider.h"
namespace inceptionengine
{
    CapsuleCollider::~CapsuleCollider() = default;

    bool CapsuleCollider::DetectCollision(Vec3f const& bottom1, Vec3f const& top1, 
                                          Vec3f const& bottom2, Vec3f const& top2,
                                          float r1, float r2)
    {

        Vec3f a0 = bottom1;
        Vec3f b0 = top1;
        Vec3f a1 = bottom2;
        Vec3f b1 = top2;
        float r = r1 + r2;
        Vec3f w0 = a0 - a1;
        Vec3f u = b0 - a0;
        Vec3f v = b1 - a1;
        float a = DotProduct(u, u);
        float b = DotProduct(u, v);
        float c = DotProduct(v, v);
        float d = DotProduct(u, w0);
        float e = DotProduct(v, w0);
        float s = (b * e - c * d) / (a * c - b * b);
        float t = (a * e - b * d) / (a * c - b * b);

        if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
        {
            //distance between lines
            float distance = Distance(a0 + s * u, a1 + t * v);
            return distance < r;
        }
        else
        {
            //distance between end points
            float distance = std::min({ Distance(a0, a1), Distance(a0, b1), Distance(b0, a1), Distance(b0, b1) });
            return distance < r;
        }
    }
 
}