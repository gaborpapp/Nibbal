#version 120

uniform sampler2D tex0;
uniform sampler2D tex1;

uniform float t;

void main()
{
	vec2 uv = gl_TexCoord[0].st;

	vec4 c0 = texture2D( tex0, uv );
	vec4 c1 = texture2D( tex1, uv );

	// smooth switch left to right
	float border = .05;
	float edgeCenter = 1. + border - ( 1. + 2. * border ) * t;
	gl_FragColor = mix( c0, c1, smoothstep( edgeCenter - border, edgeCenter + border, uv.x ) );

	// hard switch left to right
    //gl_FragColor = mix( c0, c1, step( 1. - t, uv.x ) );

	// fade
	//gl_FragColor = mix( c0, c1, t );
}

