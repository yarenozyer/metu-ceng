#version 330 core


vec3 I = vec3(1, 1, 1);          // point light intensity
vec3 Iamb = vec3(0.9, 0.9, 0.9); // ambient light intensity
vec3 kd = vec3(0.9, 0.6, 0.0);     // diffuse reflectance coefficient
vec3 ka = vec3(0.3, 0.3, 0.3);   // ambient reflectance coefficient
vec3 ks = vec3(0.9, 0.9, 0.9);   // specular reflectance coefficient


uniform vec3 eyePos;

in vec4 fragWorldPos;
in vec3 fragWorldNor;
in vec3 lightPosTransformed;
in vec3 eyePosTransformed;

out vec4 fragColor;

void main(void)
{

	
	vec3 L = normalize(lightPosTransformed - vec3(fragWorldPos));
	vec3 V = normalize(eyePosTransformed - vec3(fragWorldPos));
	vec3 H = normalize(L + V);
	vec3 N = normalize(fragWorldNor);

	float NdotL = dot(N, L); // for diffuse component
	float NdotH = dot(N, H); // for specular component

	vec3 diffuseColor = I * kd * max(0, NdotL);
	vec3 specularColor = I * ks * pow(max(0, NdotH), 70);
	vec3 ambientColor = Iamb * ka;
	fragColor = vec4(diffuseColor+ specularColor + ambientColor, 1);
}
