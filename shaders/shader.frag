#version 120

uniform vec4 Ambient_uniform;
uniform vec4 Specular_uniform;
uniform float SpecularPower_uniform;
uniform sampler2D TextureMap_uniform;

varying vec3 ViewDirection;
varying vec3 LightDirection;
varying vec3 Normal;
varying vec2 uv;



void main()
{
    vec3 fvLightDirection = normalize(LightDirection);
    vec3 fvNormal         = normalize(Normal);
    
    float fNDotL          = dot(fvNormal, fvLightDirection); 
   
    vec3 fvReflection     = normalize( ((2.0 * fvNormal) * fNDotL) - fvLightDirection ); 
    vec3 fvViewDirection  = normalize(ViewDirection);
    float fRDotV           = max( 0.0, dot( fvReflection, fvViewDirection ) );
   
    vec4 fvTotalDiffuse   = fNDotL * texture2D(TextureMap_uniform, uv);
    vec4 fvTotalSpecular  = Specular_uniform * ( pow(fRDotV, SpecularPower_uniform) );
    
    gl_FragColor = vec4((Ambient_uniform.rgb + fvTotalDiffuse.rgb + fvTotalSpecular.rgb), 1.0);
}

