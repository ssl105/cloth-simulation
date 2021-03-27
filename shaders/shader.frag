#version 330 core

// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.

in vec3 fragNormal;

// uniforms used for lighting
// light1
uniform vec3 AmbientColor = vec3(0.2);
uniform vec3 LightDirection = normalize(vec3(0, 5, 0));
uniform vec3 LightColor = vec3(1);

// light2

uniform vec3 AmbientColor2 = vec3(0.2);
uniform vec3 LightDirection2 = normalize(vec3(0, -5, 0));
uniform vec3 LightColor2 = vec3(0.5, 0.2, 0.9);

uniform vec3 DiffuseColor;	// passed in from c++ side NOTE: you can also set the value here and then remove 
							// color from the c++ side

// You can output many things. The first vec4 type output determines the color of the fragment
out vec4 fragColor;

void main()
{
	
	// light1
	// Compute irradiance (sum of ambient & direct lighting)
	vec3 irradiance = AmbientColor + LightColor * max(0, dot(LightDirection, fragNormal));

	// Diffuse reflectance
	vec3 reflectance = irradiance * DiffuseColor;

	
	//light 2
	// Compute irradiance (sum of ambient & direct lighting)
	vec3 irradiance2 = AmbientColor2 + LightColor2 * max(0, dot(LightDirection2, fragNormal));

	// Diffuse reflectance
	vec3 reflectance2 = irradiance2 * DiffuseColor;

	// add both lights
	reflectance = reflectance + reflectance2;

	// Gamma correction
	fragColor = vec4(sqrt(reflectance), 1);

}