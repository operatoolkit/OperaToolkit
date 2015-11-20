#version 120

uniform vec2 resolution;
uniform float brightness;
uniform float contrast;
uniform float saturation;
uniform float offset;
uniform sampler2DRect texture;


void main( void )
{
	vec2 st = gl_FragCoord.st;

	vec2 st2 = gl_FragCoord.st / resolution.xy;

 

	st2 = offset * (1.0 - st2) + st2 * (1.0 - offset);

	st = st2 * resolution;


	vec3 texColor = texture2DRect(texture, st).rgb;

 	const vec3 LumCoeff = vec3(0.2125, 0.7154, 0.0721);
 	vec3 AvgLumin = vec3(0.5, 0.5, 0.5);
 	vec3 intensity = vec3(dot(texColor, LumCoeff));

	vec3 satColor = mix(intensity, texColor, saturation);
 	vec3 conColor = mix(AvgLumin, satColor, contrast);
	vec4 color = vec4(brightness * conColor, 1.0);
  	gl_FragColor = color;
}

