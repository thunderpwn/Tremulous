/*[Vertex]*/
attribute vec3 attr_Position;
attribute vec4 attr_TexCoord0;

uniform mat4   u_ModelViewProjectionMatrix;

varying vec2   var_TexCoords;

void main(void)
{
  gl_Position = u_ModelViewProjectionMatrix * vec4(attr_Position, 1.0);
  var_TexCoords = attr_TexCoord0.st;
}

/*[Fragment]*/
uniform sampler2D u_DiffuseMap;

uniform vec2	u_Dimensions;
uniform vec4	u_Local0; // scan_pixel_size_x, scan_pixel_size_y, scan_width, is_ssgi

varying vec2	var_TexCoords;

void main(void)
{
	float NUM_VALUES = 1.0;
	vec2 PIXEL_OFFSET = vec2(1.0 / u_Dimensions.x, 1.0 / u_Dimensions.y);

	vec3 col0 = texture2D(u_DiffuseMap, var_TexCoords.xy).rgb;
	//gl_FragColor.rgb = vec3(0.0, 0.0, 0.0);
	gl_FragColor.rgb = col0.rgb;

	for (float width = 1.0; width <= u_Local0.z; width += 1.0)
	{
		float dist_mult = clamp(1.0 - (width / u_Local0.z), 0.0, 1.0);
		vec3 col1 = texture2D(u_DiffuseMap, var_TexCoords.xy + ((u_Local0.xy * width) * PIXEL_OFFSET)).rgb;
		vec3 col2 = texture2D(u_DiffuseMap, var_TexCoords.xy - ((u_Local0.xy * width) * PIXEL_OFFSET)).rgb;
		vec3 add_color = ((col0 / 2.0) + ((col1 + col2) * (dist_mult * 2.0))) / 4.0;

		vec3 BLUE_SHIFT_MOD = vec3(0.333, 0.333, 3.0);
		vec3 add_color_blue = clamp(add_color * (BLUE_SHIFT_MOD * (1.0 - clamp(dist_mult*3.5, 0.0, 1.0))), 0.0, 1.0);
		add_color.rgb += clamp(((add_color + add_color + add_color_blue) * 0.37), 0.0, 1.0);

		gl_FragColor.rgb += add_color;
		NUM_VALUES += 1.0;
	}

	gl_FragColor.rgb /= NUM_VALUES;//u_Local0.z;
	gl_FragColor.a	= 1.0;
}
