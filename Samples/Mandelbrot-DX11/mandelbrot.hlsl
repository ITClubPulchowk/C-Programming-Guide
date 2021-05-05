cbuffer constants : register(b0) {
	float2 Resolution;
	float2 RectMin;
	float2 RectMax;
	float2 Center;
	float Zoom;
	float AspectRatio;
}

float4 vs_main(float2 Position: POSITION) : SV_POSITION {
	float4 position = float4(Position, 0, 1);
	return position;
}

static const uint MaxIterations = 200;
static const float Radius = 4.0f;
static const float3 ColorWeight = float3(2.0, 4.0, 5.0);

float2 ComplexProduct(float2 a, float2 b) {
	return float2(a.x * b.x - a.y * b.y, a.x * b.y + a.y * b.x);
}

uint Diverge(inout float2 c, float radius) {
	float2 z = float2(0, 0);
	uint iter = 0;
	while (length(z) <= radius && iter < 500) {
		z = ComplexProduct(z, z) + c;
		iter += 1;
	}
	c = z;
	return iter;
}

float4 ps_main(float4 Position : SV_Position) : SV_TARGET0{
	float2 st = Position.xy / Resolution;
	
	float2 z = Center + (RectMin + st * (RectMax - RectMin)) * Zoom * float2(AspectRatio, 1);
	uint iterations = Diverge(z, Radius);
	float luminance = ((iterations - log2(length(z) / Radius)) / MaxIterations);
	float3 color = ColorWeight * luminance;
	
	return float4(color, 1);
}
