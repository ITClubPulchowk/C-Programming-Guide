cbuffer constants : register(b0) {
	float2 Resolution;
	float2 RectMin;
	float2 RectMax;
	float2 Center;
	float Zoom;
	float AspectRatio;
	float Time;
}

float4 vs_main(float2 Position: POSITION) : SV_POSITION {
	float4 position = float4(Position, 0, 1);
	return position;
}

float2 ComplexProduct(float2 a, float2 b) {
	return float2(a.x * b.x - a.y * b.y, a.x * b.y + a.y * b.x);
}

float3 HSV2RGB(float3 c) {
    float4 K = float4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    float3 p = abs(frac(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * lerp(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

float3 Mandelbrot_SimpleColoring(float2 c) {
	const uint MaxIterations = 200;
	const float Radius = 4.0f;
	const float3 ColorWeight = float3(2.0, 4.0, 5.0);
	
	float2 z = float2(0, 0);
	uint iterations = 0;
	while (length(z) <= Radius && iterations < MaxIterations) {
		z = ComplexProduct(z, z) + c;
		iterations += 1;
	}
	
	float luminance = ((iterations - log2(length(z) / Radius)) / MaxIterations);
	float3 color = ColorWeight * luminance;
	return color;
}

float3 Mandelbrot_WaveColoring(float2 c) {
	const uint MaxIterations = 500;
	const float Radius = 2.0;
	const float Amount = 0.1;
	
	float2 z = float2(0, 0);
	uint iterations = 0;
	while (length(z) <= Radius && iterations < MaxIterations) {
		z = ComplexProduct(z, z) + c;
		iterations += 1;
	}
	
	float3 color;
	color.x = 0.5 * sin(Amount * iterations + 4.188) + 0.5;
	color.y = 0.5 * sin(Amount * iterations) + 0.5;
	color.z = 0.5 * sin(Amount * iterations + 2.094) + 0.5;
	
	return color;
}

float3 Mandelbrot_WaveColoringAnimated(float2 c) {
	const uint MaxIterations = 500;
	const float Radius = 2.0;
	const float Amount = 0.1;
	const float Speed = 1;
	
	float2 z = float2(0, 0);
	uint iterations = 0;
	while (length(z) <= Radius && iterations < MaxIterations) {
		z = ComplexProduct(z, z) + c;
		iterations += 1;
	}
	
	float3 color;
	color.x = 0.5 * sin(Time * Speed + Amount * iterations + 4.188) + 0.5;
	color.y = 0.5 * sin(Time * Speed + Amount * iterations) + 0.5;
	color.z = 0.5 * sin(Time * Speed + Amount * iterations + 2.094) + 0.5;
	
	return color;
}

float3 Mandelbrot_SmoothColoring(float2 c) {
	const uint MaxIterations = 500;
	const float Radius = 2.0;
	const float Saturation = 1.0;
	const float Value = 0.8;
	const float MinHue = 0.1;
	const float MaxHue = 0.8;
	
	float2 z = float2(0, 0);
	uint iterations = 0;
	while (length(z) <= Radius && iterations < MaxIterations) {
		z = ComplexProduct(z, z) + c;
		iterations += 1;
	}
	
	 if (iterations < MaxIterations) {
        float log_zn = log(z.x * z.x + z.y * z.y) / 2.0;
        float nu = log(log_zn / log(2)) / log(2);
        iterations = iterations + 1 - nu;
	}

	float hue1 = (float)iterations / (float)MaxIterations;
	float hue2 = (float)(iterations + 1) / (float)MaxIterations;
    float hue = lerp(hue1, hue2, iterations % 1);
	hue = MinHue + hue * (MaxHue - MinHue);
	
	float3 color = HSV2RGB(float3(hue, Saturation, Value));
	
	return color;
}

float4 ps_main(float4 Position : SV_Position) : SV_TARGET0 {
	float2 st = Position.xy / Resolution;	
	float2 z = Center + (RectMin + st * (RectMax - RectMin)) * Zoom * float2(AspectRatio, 1);
	
	float3 color = Mandelbrot_WaveColoringAnimated(z);
	//float3 color = Mandelbrot_WaveColoring(z);
	//float3 color = Mandelbrot_SmoothColoring(z);
	//float3 color = Mandelbrot_SimpleColoring(z);
	
	return float4(color, 1);
}
