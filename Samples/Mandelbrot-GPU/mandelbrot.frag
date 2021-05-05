#version 420

#define MAX_ITERATIONS 500
#define cproduct(a, b) vec2(a.x*b.x-a.y*b.y, a.x*b.y+a.y*b.x)

out vec4 FragmentColor;

uniform vec2 u_Resolution;
uniform vec2 u_RectMin;
uniform vec2 u_RectMax;

float Radius = 4.0f;
vec3 ColorWeight = vec3(2.0, 4.0, 5.0);

uint Diverge(inout vec2 c, float radius) {
	vec2 z = vec2(0, 0);
	uint iter = 0;
	while (length(z) <= radius && iter < MAX_ITERATIONS) {
		z = cproduct(z, z) + c;
		iter += 1;
	}
	c = z;
	return iter;
}

void main() {
	vec2 st = gl_FragCoord.xy / u_Resolution;
	
	vec2 z = u_RectMin + st * (u_RectMax - u_RectMin);
	uint iterations = Diverge(z, Radius);
	float luminance = ((iterations - log2(length(z) / Radius)) / MAX_ITERATIONS);
	vec3 color = ColorWeight * luminance;
	FragmentColor = vec4(color, 1);
}
