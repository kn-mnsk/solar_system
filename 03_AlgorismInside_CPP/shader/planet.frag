#version 430 core

//layout(std140) uniform;

in VERT_OUT {
    vec3 ViewPos;
    vec3 ViewNormal;
    vec2 TexCoord;
	mat4 View;
} frag_in;


out vec4 outColor;

layout(std140) uniform MaterialBlock
{
	vec4 diffuseColor;
	vec4 specularColor;
	float specularShininess;
} Mtl;

struct PerLight
{
	vec3 lightDirV3InWS;
	float intraStarsDistance;
	float attenuCoefficient;
	vec4 lightIntensity;
};

const int numberOfLights = 1;

layout(std140) uniform LightBlock
{
	vec4 ambientIntensity;
	float attenuCoefficient;
	float maxIntensity;
	float gamma;
	PerLight lights[numberOfLights];
} Lgt;

uniform sampler2D shininess;
uniform sampler2D surface;


float CalcAttenuation(in float attenuCoefficient, in float intraStarsDistanceInCS)
{	
	float attenuation = 1.0 / ( 1.0 + attenuCoefficient * pow(intraStarsDistanceInCS, 2.0));
	return attenuation;
}


void main()
{// lightpos is in world space

// compute lighting
    float specularShininess = texture(shininess, frag_in.TexCoord).r;
	vec4 picture = texture(surface, frag_in.TexCoord);

	vec3 lightDirV3InCS = vec3(frag_in.View * vec4(Lgt.lights[0].lightDirV3InWS, 0.0f));;
	vec3 unitLightDirV3InCS = normalize(lightDirV3InCS);;
	float lightDistance = length(lightDirV3InCS);

   vec4 lightIntensity = Lgt.lights[0].lightIntensity * CalcAttenuation(Lgt.attenuCoefficient, lightDistance);

	// Note:: lightDirV3InWS is directed from Sun
	float cosAngIncidence = dot(frag_in.ViewNormal, -unitLightDirV3InCS);
	//cosAngIncidence = cosAngIncidence < 0.0001 ? 0.0 : cosAngIncidence;
	cosAngIncidence = clamp(cosAngIncidence, 0.0, 1.0);

	vec3 viewDirection = normalize(-frag_in.ViewPos);
	vec3 halfAngle = normalize(-unitLightDirV3InCS + viewDirection);
	float angleNormalHalf = acos(dot(halfAngle, frag_in.ViewNormal));
	float exponent = -pow(angleNormalHalf / specularShininess, 2.0);
	float gaussianTerm = exp(exponent);
	gaussianTerm = cosAngIncidence != 0.0 ? gaussianTerm : 0.0;

	vec4 lighting = Mtl.diffuseColor  * lightIntensity * cosAngIncidence+ Mtl.specularColor * gaussianTerm  * lightIntensity;
	
	vec4 pictureIncidence = cosAngIncidence != 0.0 ? vec4(1.0) : vec4(0.08);
	pictureIncidence.w = 1.0f;
	vec4 maxIntensity = vec4(Lgt.maxIntensity);
	maxIntensity.w = 1.0f;
	vec4 gamma = vec4(1.0/Lgt.gamma);
	gamma.w = 1.0;

	outColor = picture * pictureIncidence
		+ pow(lighting/maxIntensity,gamma) + Mtl.diffuseColor * Lgt.ambientIntensity;
	}
