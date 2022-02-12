$input a_position, a_color0
$output v_view, v_bc

#include "common/common.sh"

void main()
{
	gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0) );
 
 	v_view = vec3(0.0, 0.0, -20.0) - mul(u_model[0], vec4(a_position, 1.0) ).xyz;
	v_bc = a_color1;
}
