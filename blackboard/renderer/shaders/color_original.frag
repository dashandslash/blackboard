$input v_view, v_bc

#include "common/common.sh"

uniform vec4 u_color;

void main()
{


	vec3  color   = vec3(1.0, 0.0, 0.0);
	float opacity = 1.0;
	float thickness = 1.5;

	if (gl_FrontFacing) { opacity *= 0.5; }

	vec3 fw = abs(dFdx(v_bc)) + abs(dFdy(v_bc));
	vec3 val = smoothstep(vec3_splat(0.0), fw*thickness, v_bc);
	float edge = min(min(val.x, val.y), val.z); // Gets to 0.0 around the edges.

	vec4 rgba = vec4(color, (1.0-edge)*opacity);
	gl_FragColor = rgba;

	gl_FragColor = u_color;
}
