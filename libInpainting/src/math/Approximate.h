#pragma once

#include <stdint.h>

namespace ettention
{

	class Approximate
	{
	public:
		static inline float fastpow2(float p)
		{
			float offset = (p < 0) ? 1.0f : 0.0f;
			float clipp = (p < -126) ? -126.0f : p;
			int w = (int) clipp;
			float z = clipp - w + offset;
			union { uint32_t i; float f; } v = { static_cast<uint32_t> ((1 << 23) * (clipp + 121.2740575f + 27.7280233f / (4.84252568f - z) - 1.49012907f * z)) };
			return v.f;
		}

		static inline float fastexp(float p)
		{
			return fastpow2(1.442695040f * p);
		}

		static inline float fasterpow2(float p)
		{
			float clipp = (p < -126) ? -126.0f : p;
			union { uint32_t i; float f; } v = { static_cast<uint32_t> ((1 << 23) * (clipp + 126.94269504f)) };
			return v.f;
		}

		static inline float fasterexp(float p)
		{
			return fasterpow2(1.442695040f * p);
		}

		static inline float fastlog2(float x)
		{
			union { float f; uint32_t i; } vx = { x };
			union { uint32_t i; float f; } mx = { (vx.i & 0x007FFFFF) | 0x3f000000 };
			float y = (float) vx.i;
			y *= 1.1920928955078125e-7f;

			return y - 124.22551499f
				- 1.498030302f * mx.f
				- 1.72587999f / (0.3520887068f + mx.f);
		}

		static inline float fastlog(float x)
		{
			return 0.69314718f * fastlog2(x);
		}

		static inline float fasterlog2(float x)
		{
			union { float f; uint32_t i; } vx = { x };
			float y = (float) vx.i;
			y *= 1.1920928955078125e-7f;
			return y - 126.94269504f;
		}

		static inline float fasterlog(float x)
		{
			union { float f; uint32_t i; } vx = { x };
			float y = (float) vx.i;
			y *= 8.2629582881927490e-8f;
			return y - 87.989971088f;
		}

		static inline float fastpow(float x, float p)
		{
			return fastpow2(p * fastlog2(x));
		}

		static inline float fasterpow(float x, float p)
		{
			return fasterpow2(p * fasterlog2(x));
		}
	};
} // namespace