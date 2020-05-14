#version 330
#extension GL_ARB_explicit_attrib_location:enable

in vec3 Position;
in vec3 Normal;

struct SpotLightInfo{
	vec4 Position;  
	vec3 Intensity;
	vec3 direction; // normalized direction of the spotlight
	float exponent; // angular attenuation exponent
	float cutoff;   // cutoff angle (btw 0 and 90)
};
uniform SpotLightInfo Spot;

struct MaterialInfo{
	vec3 Ka;
	vec3 Kd;
	vec3 Ks;
	float Shininess;
};
uniform MaterialInfo Material;

out vec4 color;

vec3 phongSpot()
{
	vec3 S = normalize(vec3(Spot.Position) - Position);
	vec3 spotDir = normalize(Spot.direction);
	float angle = acos(dot (-S, spotDir));
	float cutoff = radians (clamp(Spot.cutoff, 0.0, 90.0));
	vec3 ambient = Spot.Intensity * Material.Ka;

	if (angle < cutoff) {
		float spotFactor = pow(dot(-S, spotDir), Spot.exponent);
		vec3 V = normalize(vec3(-Position));
		vec3 H = normalize(V+S);

		return ambient + spotFactor * Spot.Intensity * (
			Material.Kd * max(dot(S, Normal), 0.0) +
			 Material.Ks * pow(max(dot(H, Normal), 0.0), Material.Shininess));	
	}
	else {return ambient;}
}

void main(void)
{
	color = vec4(phongSpot(), 1.0);
}