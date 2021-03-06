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
		gl_FragColor = mix( c0, c1, step( t, fract( 2. * uv.x ) ) );
	}
	else
	if ( type == 2 )
	{
		// hard switch right to left
		gl_FragColor = mix( c0, c1, step( 1. - t, fract( 2. * uv.x ) ) );
	}
	else
	if ( type == 3 )
	{
		// smooth switch left to right
		float edgeCenter = - border + ( 1. + 2. * border ) * t;
		gl_FragColor = mix( c0, c1, smoothstep( edgeCenter - border, edgeCenter + border,
			fract( 2. * uv.x ) ) );
	}
	else
	{
		// smooth switch right to left
		float edgeCenter = 1. + border - ( 1. + 2. * border ) * t;
		gl_FragColor = mix( c0, c1, smoothstep( edgeCenter - border, edgeCenter + border,
			fract( 2. * uv.x ) ) );
	}
}
