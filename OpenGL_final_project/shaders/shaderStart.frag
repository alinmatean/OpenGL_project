#version 410 core

in vec3 fNormal;
in vec4 fPosEye;
in vec2 fTexCoords;
in vec4 lightPosEye;

out vec4 fColor;

//lighting
uniform	vec3 lightDir;
uniform	vec3 lightColor;

//point
uniform vec3 lightPos_p;
uniform	vec3 lightDir_p;
uniform	vec3 lightColor_p;
uniform int point;

//texture
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D shadowMap;

//fog
uniform int fog;

vec3 ambient;
float ambientStrength = 0.2f;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.5f;
float shininess = 32.0f;

float shadow;
float specCoeff;

in vec4 fragPosLightSpace;

///point light
vec3 ambient_point;
vec3 diffuse_point;
vec3 specular_point;
float ambientStrength_point = 0.1f;
float specularStrength_point = 0.0001f;
float shininess_point = 1.0f;

float constant = 1.0f;
float linear = 0.022;
float quadratic = 0.0019;

void computeLightComponents()
{		
	vec3 cameraPosEye = vec3(0.0f);//in eye coordinates, the viewer is situated at the origin
	
	//transform normal
	vec3 normalEye = normalize(fNormal);	
	
	//compute light direction
	vec3 lightDirN = normalize(lightDir);
	
	//compute view direction 
	vec3 viewDirN = normalize(cameraPosEye - fPosEye.xyz);
		
	//compute ambient light
	ambient = ambientStrength * lightColor;
	
	//compute diffuse light
	diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;
	
	//compute specular light
	vec3 reflection = reflect(-lightDirN, normalEye);
	specCoeff = pow(max(dot(viewDirN, reflection), 0.0f), shininess);
	specular = specularStrength * specCoeff * lightColor;
}

float computeShadow()
{
	// perform perspective divide
	/*vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	normalizedCoords = normalizedCoords * 0.5 + 0.5;
	if(normalizedCoords.z > 1.0f)
		return 0.0f;
	float closestDepth = texture(shadowMap, normalizedCoords.xy).r;
	float currentDepth = normalizedCoords.z;
	float bias = 0.005f;
	float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
	return shadow;*/
	vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	normalizedCoords = normalizedCoords * 0.5 + 0.5;
	float bias = 0.005f;
	float shadow = 0.0f;
	if(normalizedCoords.z > 1.0f)
		return 0.0f;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for(int x = -2; x <= 2; x++)
	{
		for(int y = -2; y <= 2; y++)
		{
			float pcfDepth = texture(shadowMap, normalizedCoords.xy + vec2(x, y)*texelSize).r;
			float currentDepth = normalizedCoords.z;
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}
	shadow /= 25.0;
	return shadow;
}

float computeFog()
{
 float fogDensity = 0.03f;
 float fragmentDistance = length(fPosEye);
 float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));

 return clamp(fogFactor, 0.0f, 1.0f);
}

void pointLight()
{
	vec3 cameraPosEye = vec3(0.0f);
    
    // transform normal
    vec3 normalEye = normalize(fNormal);
    
    // compute light direction
    vec3 p_lightDirN = normalize(lightPos_p - fPosEye.xyz);
    
    // compute distance to light
    float dist = length(lightPos_p - fPosEye.xyz);
    
    // compute attenuation
    float att = 1.0f / (constant + linear * dist + quadratic * (dist * dist));
    
    // compute view direction
    vec3 p_viewDirN = normalize(cameraPosEye - fPosEye.xyz);
    
    // compute half vector
    vec3 halfVector = normalize(p_lightDirN + p_viewDirN);
    
    // compute specular coefficient
    float p_specCoeff = pow(max(dot(normalEye, halfVector), 0.0f), shininess_point);
    
    // compute ambient light
    ambient_point = att * ambientStrength_point * lightColor_p;
    
    // compute diffuse light
    diffuse_point = att * max(dot(normalEye, p_lightDirN), 0.0f) * lightColor_p;
    
    // compute specular light
    specular_point = att * specularStrength_point * p_specCoeff * lightColor_p;
}

void main() 
{
	computeLightComponents();
	pointLight();

	vec3 baseColor = vec3(0.9f, 0.35f, 0.0f);//orange
	
	ambient *= texture(diffuseTexture, fTexCoords).rgb;
	diffuse *= texture(diffuseTexture, fTexCoords).rgb;
	specular *= texture(specularTexture, fTexCoords).rgb;

	ambient_point *= texture(diffuseTexture, fTexCoords).xyz;
	diffuse_point *= texture(diffuseTexture, fTexCoords).rgb;
	specular_point *= texture(specularTexture, fTexCoords).rgb;


	vec3 color_p = min(ambient_point + diffuse_point + specular_point, 1.0f);
	shadow = computeShadow();

	vec3 color_d = min((ambient + (1.0f - shadow)*diffuse) + (1.0f - shadow)*specular, 1.0f);
	vec3 color;
	if(point == 1){
		color = color_p + color_d;
	}
	else{
		color = color_d;
	}

	vec4 light_color = vec4(color, 1.0f);
	if (fog == 1){
		float fogFactor = computeFog();
		vec4 fogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);
		fColor = mix(fogColor, light_color, fogFactor);
	}
	else{
		fColor = vec4(color, 1.0f);
	}

}