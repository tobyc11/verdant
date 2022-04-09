//
//  GPUCode.metal
//  MetalRay
//
//  Created by Toby on 4/8/22.
//

#include <metal_stdlib>
using namespace metal;

struct VertexOut {
    float4 position [[position]];
    float2 texCoord;
};

vertex VertexOut quadVertex(device const float4 *in [[buffer(0)]],
                            uint vid [[vertex_id]])
{
    VertexOut out;
    out.position = in[vid];
    out.texCoord = (in[vid].xy + 1.f) / 2.f;
    return out;
}

fragment float4 blitterFrag(VertexOut in [[stage_in]],
                            texture2d<float, access::sample> source [[texture(0)]],
                            sampler smp [[sampler(0)]])
{
    // return source.sample(smp, in.position.rg);
    // return float4(in.texCoord, 0, 0);
    return source.sample(smp, in.texCoord);
}

// Begin ray tracing solution
struct Ray {
    float3 O;
    float3 D;
};

struct Isect {
    float3 pos;
    float3 normal;
};

struct Sphere {
    float3 center;
    float radius;
    
    bool intersect(thread const Ray &ray, thread float *t, thread Isect &isect)
    {
        float t0, t1;
        float3 L = center - ray.O;
        float tca = dot(L, ray.D);
        float d2 = dot(L, L) - tca * tca;
        float radius2 = radius * radius;
        if (d2 > radius2) {
            return false;
        }
        float thc = sqrt(radius2 - d2);
        t0 = tca - thc;
        t1 = tca + thc;
        
        if (t0 > t1) {
            float tmp = t0;
            t0 = t1;
            t1 = tmp;
        }
        if (t0 < 0.f) {
            t0 = t1;
            if (t0 < 0.f) {
                return false;
            }
        }
        *t = t0;
        isect.pos = ray.O + ray.D * t0;
        isect.normal = normalize(isect.pos - center);
        return true;
    }
};

bool sceneIntersect(thread const Ray &ray, thread Isect &isect)
{
    // Let's have three spheres
    Sphere sphereList[3];
    sphereList[0].center = float3(0, 0, -10);
    sphereList[0].radius = 3.0f;
    sphereList[1].center = float3(5, 5, -6);
    sphereList[1].radius = 2.0f;
    sphereList[2].center = float3(3, 3, -8);
    sphereList[2].radius = 1.0f;
    
    float t = FLT_MAX;
    bool didIsect = false;
    for (int i = 0; i < 3; i++) {
        float tCurr;
        bool didIsectCurr;
        didIsectCurr = sphereList[i].intersect(ray, &tCurr, isect);
        if (didIsectCurr && tCurr < t) {
            t = tCurr;
            didIsect = true;
        }
    }
    return didIsect;
}

Ray rayMath(float2 xy, float fovY, float aspectRatio)
{
    // (x, y) in NDC -> (x, y, z) in camera space
    float zPlane = -1;
    float yEnd = tan(fovY / 2.0f);
    float xEnd = yEnd * aspectRatio;
    float3 viewRayDir = float3(xEnd * xy.x, yEnd * xy.y, zPlane);
    float3 origin = float3(0, 0, 0);
    
    Ray ray;
    ray.O = origin;
    ray.D = normalize(viewRayDir);
    return ray;
}

float3 displayNormal(float3 normal)
{
    return (normal + 1) / 2;
}

float3 integrator(thread const Ray &ray)
{
    Isect isect;
    bool hit = sceneIntersect(ray, isect);
    float3 Lo = float3(0, 0, 0);
    if (hit) {
        // Calculate 0 bounce lighting
        
        // Dummy
        // Lo = float3(0, 1, 0);
        Lo = displayNormal(isect.normal);
    }
    return Lo;
}

kernel void rtDraw(ushort2 tid [[thread_position_in_grid]],
                   texture2d<float, access::write> film [[texture(0)]])
{
    // Film (0, 0) is screen bottom left
    // tid contains the pixel coordinate of this sample
    float2 coord = (float2)tid / float2(1280.0f, 720.0f);
    float2 ndc = coord * 2.0f - 1.0f;
    Ray ray = rayMath(ndc, 1.2217f, 1280.0f / 720.0f);
    film.write(float4(integrator(ray), 1.0), tid);
}

kernel void dummyDrawKernel(ushort2 tid [[thread_position_in_grid]],
                            ushort2 gridSize [[threads_per_grid]],
                            texture2d<float, access::write> film [[texture(0)]])
{
    float2 coord = (float2)tid / float2(1280.0f, 720.0f);
    film.write(float4(coord, 0, 1), tid);
}
