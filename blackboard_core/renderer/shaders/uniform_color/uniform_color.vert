$input a_position, a_color1, a_normal, a_color0, a_texcoord0, a_tangent, a_bitangent
$output v_view, v_position, v_bc, v_normal, v_texcoord0, v_color0, v_tangent, v_bitangent

#include "../common/common.sh"
#include "../uniforms.sh"

void main()
{
	v_color0 = a_color0;
	v_texcoord0 = a_texcoord0;

	gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0) );
    v_position = gl_Position;

	v_view = u_camera_position - mul(u_model[0], vec4(a_position, 1.0) ).xyz;
	v_bc = a_color1;

	vec3 normal = a_normal * 2.0 - 1.0;
	// v_normal = mul(u_model[0], vec4(a_normal, 0.0) ).xyz;
	v_normal = normal;

	v_tangent = a_tangent * 1.0 - 1.0;
	v_bitangent = a_bitangent * 1.0 - 1.0;
}
