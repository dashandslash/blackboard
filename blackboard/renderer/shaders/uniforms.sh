/*
 * Copyright 2016 Dario Manesku. All rights reserved.
 * License: https://github.com/bkaradzic/bgfx#license-bsd-2-clause
 */

uniform vec4 u_uniform[4];
#define u_camera_position u_uniform[0].xyz
#define u_time            u_uniform[0].w
#define u_color           u_uniform[1]
#define u_edge_color      u_uniform[2]
#define u_edge_thickness  u_uniform[3].x
