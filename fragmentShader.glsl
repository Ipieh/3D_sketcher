#version 330 core	     
// Minimal GL version support expected from the GPU

out vec4 color;	  // Shader output: the color response attached to this fragment

in vec2 fTexCoord;
in vec3 fNormal;
in vec3 fPosition; 

uniform vec3 camPos;
uniform vec3 objColor;
uniform float shininess;
uniform bool isSun;
uniform sampler2D ourTexture;

void main() {
	vec3 texColor = texture(ourTexture, fTexCoord).rgb;
	if (!isSun){
		
		vec3 lightColor = vec3(1.0);
		vec3 n = normalize(fNormal);
		vec3 lightDirection = -normalize(vec3(1,1,0)); 
		//light direction vector 

		vec3 viewVector = normalize(camPos-fPosition);
		vec3 reflection = normalize(reflect(-lightDirection,n));
		float ambient = 0.5f;
	
		vec3 specular = pow(max(dot(viewVector,reflection),0.0),shininess)*lightColor;
		float diffuse = max(dot(n, lightDirection),0.0);

		color = vec4((ambient + diffuse)*objColor + specular, 1.0);
	 }

	else{
		color = vec4(texColor,1.0);
	
	}

	// Building RGBA from RGB. 
}
	

