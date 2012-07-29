#version 120

uniform sampler2D tex0;
uniform sampler2D tex1;

uniform float t;
uniform int type;
uniform float border;

void main()
{
	vec2 uv = gl_TexCoord[0].st;

	vec4 c0 = texture2D( tex0, uv );
	vec4 c1 = texture2D( tex1, uv );

	if ( type == 0)
	{
		// fade
		gl_FragColor = mix( c0, c1, t );
	}
	else
	if ( type == 1 )
	{
		// hard switch left to right
		gl_FragColor = mix( c0, c1, step( 1. - t, uv.x ) );
	}
	else
	{
		// smooth switch left to right
		float edgeCenter = 1. + border - ( 1. + 2. * border ) * t;
		gl_FragColor = mix( c0, c1, smoothstep( edgeCenter - border, edgeCenter + border, uv.x ) );
	}
}
