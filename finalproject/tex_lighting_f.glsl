#version 430

#extension GL_ARB_explicit_attrib_location:enable

in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;

uniform sampler2D Tex1;

struct LightInfo{
   vec4 Position;  
   vec3 Intensity;
};

struct SpotLightInfo{
   vec4 Position;  
   vec3 Intensity;
   vec3 direction; // normalized direction of the spotlight
   float exponent; // angular attenuation exponent
   float cutoff;   // cutoff angle (btw 0 and 90)
};

uniform SpotLightInfo Spot;
uniform LightInfo Light;

struct MaterialInfo{
   vec3 Ka;
   vec3 Kd;
   vec3 Ks;
   float Shininess;
};
uniform MaterialInfo Material;

out vec4 color;

vec3 phongModel(vec4 P, vec3 N, out vec3 ambAndDiff, out vec3 spec)
{
   vec3 L; 
   if (Light.Position.w == 0.0) 
      L = normalize(Light.Position.xyz);
   else 
      L = normalize((Light.Position - P).xyz);
   vec3 V = normalize(-P.xyz);
   vec3 R = reflect(-L, N);
   float lDotN = max (dot (L, N), 0.0);
   vec3 ambient = Light.Intensity * Material.Ka;
   vec3 diffuse = Light.Intensity * Material.Kd * lDotN;
   ambAndDiff = ambient + diffuse;
   spec = vec3(0.0);
   if (lDotN >0.0)
      spec = Light.Intensity * Material.Ks * pow(max (dot(R,V), 0.0), Material.Shininess);

   return ambient+diffuse+spec;
}

vec3 phongSpot()
{
   vec3 S = normalize(vec3(Spot.Position) - Position);
   vec3 spotDir = normalize(Spot.direction);
   float angle = acos(dot (-S, spotDir));
   float cutoff = radians (clamp(Spot.cutoff, 0.0, 90.0));
   vec3 ambient = Light.Intensity * Material.Ka;

   if (angle < cutoff) {
      float spotFactor = pow(dot(-S, spotDir), Spot.exponent);
      vec3 V = normalize(vec3(-Position));
      vec3 H = normalize(V+S);
      return Spot.Intensity * Material.Ka + spotFactor * Spot.Intensity * (
         Material.Kd * max(dot(S, Normal), 0.0) +
          Material.Ks * pow(max(dot(H, Normal), 0.0), Material.Shininess));   
   }
   else {   
   return ambient;
   }
}

void main(void)
{
   vec3 LightIntensity, ambAndDiff, spec;
   vec4 temp;
   vec4 texColor = texture(Tex1, TexCoord);

   LightIntensity = phongModel(vec4(Position, 1.0), Normal, ambAndDiff, spec);
   LightIntensity = vec3(0.0, 0.0, 0.0);
   temp = vec4(ambAndDiff, 1.0)*texColor + vec4(spec, 1.0);
   
   LightIntensity += phongSpot();

   color = temp + vec4(LightIntensity, 1.0);

}