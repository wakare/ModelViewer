#version 330 core

out vec4 outputColor;
in vec3 outColor;
in vec3 outNormal;
in vec3 viewDirection;

uniform int enableLight;

void main()
{
	if (enableLight < 0.5)
		outputColor = vec4(outColor, 1.0);

	else
	{
		vec3 lightDirection = vec3(0.0, 0.0, 1.0);
		vec3 originalColor = vec3(1.0, 1.0, 1.0);
		vec3 ambient = vec3(0.7, 0.7, 0.7);
		vec3 diffuse = vec3(0.8, 0.8, 0.8);
		float specularLevel = 64.0;
		vec3 specular = vec3(1.0, 1.0, 1.0);
		vec3 normal = outNormal;

		if (!gl_FrontFacing)
		{
			normal.z = -normal.z;
			ambient = vec3(0.3, 0.3, 0.3);
			diffuse = vec3(0.4, 0.4, 0.4);
			specular = vec3(0.0, 0.0, 0.0);
		}

		float NdotL = dot(normal, lightDirection);

		vec3 ambient_color = ambient * originalColor;

		float diffuseTemp = max(NdotL, 0);
		vec3 diffuse_color = diffuseTemp * originalColor * diffuse;

		vec3 reflectDirection = ((2 * normal) * NdotL)  - lightDirection;
		float RdotV = dot(reflectDirection, viewDirection);
		float specularTemp = max(0, pow(RdotV, specularLevel));
		vec3 specular_color = specular * specularTemp;

		vec3 total_color = ambient_color + diffuse_color + specular_color;

		outputColor = vec4(total_color, 1.0);
	}
}