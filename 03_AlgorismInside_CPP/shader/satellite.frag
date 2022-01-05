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

//
//in vec3 _vertexV3InCS;
//in vec3 _vertexNormalV3InCS;
//in vec2 _textureCoordInMS;
//in mat4 _Mat4OfWStoCS;
//
//out vec4 outputColor;
//
//layout(std140) uniform;
//
//uniform Material
//{
//	vec4 diffuseColor;
//	vec4 specularColor;
//	float specularShininess;
//} Mtl;
//
//struct PerLight
//{
//	vec3 lightDirV3InWS;
//	float intraStarsDistance;
//	float lightAttenuation;
//	vec4 lightIntensity;
//};
//
//const int numberOfLights = 1;
//
//uniform Light
//{
//	vec4 ambientIntensity;
//	//float lightAttenuation;
//	PerLight lights[numberOfLights];
//} Lgt;
//
//uniform sampler2D shininessTexture;
//uniform sampler2D surfaceTexture;
//
//
////float CalcAttenuation(in float intraStarsDistance)
//{	
	//float attenuation = 1.0 / ( 1.0 + Lgt.lightAttenuation * pow(intraStarsDistance, 2.0));
	//return attenuation;
//}

vec4 ComputeLighting(in vec3 lightDirV3InWS, in float intraStarsDistance, in float attenuation, in vec4 lightIntensity, in vec3 vertexV3InCS,	in vec3 vertexNormalV3InCS, in float specularShininess)
{
	vec4 _lightIntensity = lightIntensity * attenuation;

	// Note:: lightDirV3InWS is directed from Sun
	float cosAngIncidence = dot(vertexNormalV3InCS, lightDirV3InWS);
	//cosAngIncidence = cosAngIncidence < 0.0001 ? 0.0 : cosAngIncidence;
	cosAngIncidence = cosAngIncidence > -0.0001 ? 0.0 : -cosAngIncidence;

	vec3 viewDirection = normalize(-vertexV3InCS);
	//vec3 viewDirection = normalize(vertexPosition);
	vec3 halfAngle = normalize(lightDirV3InWS + viewDirection);
	float angleNormalHalf = acos(dot(halfAngle, vertexNormalV3InCS));
	float exponent = -pow(angleNormalHalf / specularShininess, 2.0);
	float gaussianTerm = exp(exponent);

	gaussianTerm = cosAngIncidence != 0.0 ? gaussianTerm : 0.0;
	
	vec4 lighting = (Mtl.diffuseColor * cosAngIncidence + Mtl.specularColor * gaussianTerm)  * lightIntensity;
	
	return lighting;
}


void main()
{
// lightpos is in world space
    float specularShininess = texture(shininess,frag_in.TexCoord).r;

	vec4 accumLighting = vec4(0.0, 0.0, 0.0, 1.0);//Mtl.diffuseColor * Lgt.ambientIntensity;//
	vec3 lightDirV3InCS;

	for(int light = 0; light < numberOfLights; light++)
	{
		lightDirV3InCS = vec3(frag_in.View * vec4(Lgt.lights[light].lightDirV3InWS, 0.0f));

		accumLighting += ComputeLighting(lightDirV3InCS, Lgt.lights[light].intraStarsDistance, Lgt.lights[light].attenuCoefficient, Lgt.lights[light].lightIntensity, frag_in.ViewPos,frag_in.ViewNormal, specularShininess);
	}
	
    vec4 picture = texture(surface, frag_in.TexCoord);

	//outputColor = picture + sqrt(accumLighting); 
	//outputColor = (picture + 1.0) *  sqrt(accumLighting);
	//outputColor = picture  *  sqrt(accumLighting);

	outColor = picture *  sqrt(sqrt(accumLighting) )+ sqrt(Mtl.diffuseColor * Lgt.ambientIntensity);

}
