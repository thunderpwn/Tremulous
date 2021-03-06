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
uniform sampler2D u_NormalMap; // Blur'ed bloom'ed VBO image...

varying vec2	var_TexCoords;

void main(void)
{
  vec3 col1 = texture2D(u_DiffuseMap, var_TexCoords.st).rgb;
  vec3 col2 = texture2D(u_NormalMap, var_TexCoords.st).rgb;
  gl_FragColor.rgb = col1 + col2;
  gl_FragColor.a	= 1.0;
}


