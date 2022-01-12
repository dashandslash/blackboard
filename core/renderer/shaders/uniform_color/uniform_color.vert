$input a_position, a_color1, a_normal
$output v_view, v_pos, v_bc, v_normal

#include "../common/common.sh"
#include "../uniforms.sh"

void main()
{
	gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0) );
    v_pos = gl_Position;

	v_view = u_camera_position - mul(u_model[0], vec4(a_position, 1.0) ).xyz;
	v_bc = a_color1;

	vec3 normal = a_normal.xyz*2.0 - 1.0;
    v_normal = normal; //mul(u_model[0], vec4(normal, 0.0) ).xyz;
}
