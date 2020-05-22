// per-fragment interpolated values from the vertex shader
varying  vec3 fN;
varying  vec3 fL;
varying  vec3 fL2;
varying  vec3 fV;
varying vec4 Color; 
uniform vec4 fColor;
uniform vec4 MaterialAmbient, LightAmbient, DiffuseProduct, SpecularProduct;
uniform vec4 LightAmbient2, DiffuseProduct2, SpecularProduct2;

uniform mat4 ModelView;
uniform vec4 LightPosition,LightPosition2;
uniform float Shininess;
uniform float SourceNum,A,B,C,ShadingType;

varying  vec2 texCoord;
uniform int TextureFlag;
uniform sampler2D texture;



void main() 
{ 

 if (TextureFlag == 1){
        //sample a texture color from texture object
        //with newer versions of OpenGL you can  use texture() function instead of deprecated texture2D() 
        gl_FragColor = texture( texture, texCoord );
		
		}

else{


if(ShadingType==0.0 || ShadingType==2.0){
		float distance=length(LightPosition);
		vec3 N = normalize(fN);
        vec3 V = normalize(fV);
		vec3 L = normalize(fL);
	    vec3 H = normalize( L + V );
		vec3 L2 = normalize(fL2);
		vec3 H2 = normalize( L2 + V );
		vec4 diffuse;
		vec4 specular;
	    vec4 ambient = fColor*(LightAmbient+LightAmbient2);
	    float distanceTerm = 1.0/(A+distance*B+distance*distance*C);
	   
	   vec3 R=normalize (2*dot(fL, fN)* fN-fL);
	   vec3 R2=normalize(2*dot(fL2, fN)* fN-fL2);
	   
	   if(SourceNum==0.0){

	    float Kd = max(dot(L, N), 0.0);
		diffuse = Kd*distanceTerm*DiffuseProduct;
		
		
		float Ks = pow(max(dot(N, H), 0.0), Shininess);

		if(ShadingType==2.0){
		Ks =pow(max(dot(V, R), 0.0), Shininess);
		}
	    
		specular = Ks*distanceTerm*SpecularProduct;
		
		

			if( dot(L, N) < 0.0 ) {
            specular = vec4(0.0, 0.0, 0.0, 1.0);
        }

		gl_FragColor = ambient + diffuse + specular;
        gl_FragColor.a = 1.0;
		}
		
		else if(SourceNum==1.0){
		   float Kd2= max(dot(L2, N), 0.0);
		    diffuse = Kd2*DiffuseProduct2 ;
		   float Ks2 = pow(max(dot(N, H2), 0.0), Shininess);
		   if(ShadingType==2.0){
		Ks2 =pow(max(dot(V, R2), 0.0), Shininess);
		}
		   specular = Ks2*SpecularProduct2;
		    if( dot(L2, N) < 0.0 ) {
            specular = vec4(0.0, 0.0, 0.0, 1.0);
        }
		  gl_FragColor = ambient + diffuse + specular;
        gl_FragColor.a = 1.0;
		}

		else if(SourceNum==2.0){
		float Kd = max(dot(L, N), 0.0);
		diffuse = Kd*distanceTerm*DiffuseProduct ;
		float Ks = pow(max(dot(N, H), 0.0), Shininess);
		if(ShadingType==2.0){
		Ks =pow(max(dot(V, R), 0.0), Shininess);
		}
	    specular = Ks*distanceTerm*SpecularProduct;
		if( dot(L, N) < 0.0 ) {
            specular = vec4(0.0, 0.0, 0.0, 1.0);
        }
		float Kd2= max(dot(L2, N), 0.0);
		diffuse += Kd2*DiffuseProduct2 ;
		float Ks2 = pow(max(dot(N, H2), 0.0), Shininess);
		if(ShadingType==2.0){
		Ks2 =pow(max(dot(V, R2), 0.0), Shininess);
		}
		if( dot(L2, N) >= 0.0 ) {
            specular +=Ks2*SpecularProduct2 ;
        }
		  gl_FragColor = ambient + diffuse + specular;
        gl_FragColor.a = 1.0;
		}
	   
	  
	    else{
		gl_FragColor=fColor;
		}
       }
	  
	  else{
		gl_FragColor=Color;
		}
		}
} 



