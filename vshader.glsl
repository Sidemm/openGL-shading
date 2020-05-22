attribute   vec4 vPosition;
attribute   vec3 vNormal;
attribute   vec3 vColor;
attribute  vec2 vTexCoord;
varying vec2 texCoord;

// output values that will be interpretated per-fragment
varying  vec3 fN;
varying  vec3 fV;
varying  vec3 fL;
varying  vec3 fL2;
varying vec4 Color;
uniform vec4 fColor; 

uniform vec4 MaterialAmbient, LightAmbient, DiffuseProduct, SpecularProduct;
uniform vec4 LightAmbient2, DiffuseProduct2, SpecularProduct2;

uniform mat4 ModelView;
uniform vec4 LightPosition,LightPosition2;
uniform float Shininess;
uniform float SourceNum,A,B,C;
uniform mat4 Projection;
uniform float ShadingType;
uniform int TextureFlag;

void main()
{

if(TextureFlag==1){
    texCoord    = vTexCoord;
    gl_Position = Projection * ModelView * vPosition;
	}

else{
	float distance=length(LightPosition);
	float distanceTerm = 1.0/(A+distance*B+distance*distance*C);

if(ShadingType==0.0 || ShadingType==2.0){
    fN = (ModelView*vec4(vNormal, 0.0)).xyz; // normal direction in camera coordinates

    fV = (ModelView * vPosition).xyz; //viewer direction in camera coordinates

    fL2 = LightPosition2.xyz; // directional light source
   
    fL = LightPosition.xyz - fV;  //point light source

    gl_Position = Projection * ModelView * vPosition;

    }

else if(ShadingType==1.0){
	if(SourceNum==0.0){
    vec3 pos = (ModelView * vPosition).xyz;
	
    vec3 L = normalize( LightPosition.xyz - pos ); //light direction
    vec3 V = normalize( -pos ); // viewer direction
    vec3 H = normalize( L + V ); // halfway vector

    // Transform vertex normal into camera coordinates
    vec3 N = normalize( ModelView * vec4(vNormal, 0.0) ).xyz;

    // Compute terms in the illumination equation
    vec4 ambient = fColor*LightAmbient;

    float Kd = max( dot(L, N), 0.0 ); //set diffuse to 0 if light is behind the surface point
    vec4  diffuse = Kd*distanceTerm*DiffuseProduct;

    float Ks = pow( max(dot(N, H), 0.0), Shininess );
    vec4  specular = Ks * distanceTerm* SpecularProduct;
    
    //ignore also specular component if light is behind the surface point
    if( dot(L, N) < 0.0 ) {
        specular = vec4(0.0, 0.0, 0.0, 1.0);
    } 

    gl_Position = Projection * ModelView * vPosition;

    Color = ambient + diffuse + specular;
    Color.a = 1.0;
	
	}

	else if(SourceNum==1.0){
    vec3 pos = (ModelView * vPosition).xyz;
	
	vec3 L2 = normalize( LightPosition2.xyz); // directional light
    
	vec3 V = normalize( -pos ); // viewer direction
   
	vec3 H2 = normalize( L2 + V ); // halfway vector

    // Transform vertex normal into camera coordinates
    vec3 N = normalize( ModelView * vec4(vNormal, 0.0) ).xyz;

    // Compute terms in the illumination equation
    vec4 ambient = fColor*LightAmbient2;

    float Kd2 = max( dot(L2, N), 0.0 ); //set diffuse to 0 if light is behind the surface point
    vec4  diffuse = Kd2*DiffuseProduct2;

    float Ks2 = pow( max(dot(N, H2), 0.0), Shininess );
    vec4  specular = Ks2 * SpecularProduct2;
    
    //ignore also specular component if light is behind the surface point
    if( dot(L2, N) < 0.0 ) {
        specular = vec4(0.0, 0.0, 0.0, 1.0);
    } 

    gl_Position = Projection * ModelView * vPosition;

    Color = ambient + diffuse + specular;
    Color.a = 1.0;
	
	}

	else if(SourceNum==2.0){
	vec3 pos = (ModelView * vPosition).xyz;
	
    vec3 L = normalize( LightPosition.xyz - pos ); //point light 
	vec3 L2 = normalize( LightPosition2.xyz); // directional light
    
	vec3 V = normalize( -pos ); // viewer direction
    vec3 H = normalize( L + V ); // halfway vector
	vec3 H2 = normalize( L2 + V ); // halfway vector

    // Transform vertex normal into camera coordinates
    vec3 N = normalize( ModelView * vec4(vNormal, 0.0) ).xyz;

    // Compute terms in the illumination equation
    vec4 ambient = fColor*(LightAmbient+LightAmbient2);
	float Kd = max( dot(L, N), 0.0 );
    float Kd2 = max( dot(L2, N), 0.0 ); //set diffuse to 0 if light is behind the surface point
    vec4  diffuse = Kd*distanceTerm*DiffuseProduct+Kd2*DiffuseProduct2;
	
	float Ks = pow( max(dot(N, H), 0.0), Shininess );
    float Ks2 = pow( max(dot(N, H2), 0.0), Shininess );
    vec4  specular = Ks*distanceTerm*SpecularProduct; 
    
    //ignore also specular component if light is behind the surface point
    if( dot(L2, N) >= 0.0 ) {
        specular += Ks2 * SpecularProduct2;
    } 

    gl_Position = Projection * ModelView * vPosition;

    Color = ambient + diffuse + specular;
    Color.a = 1.0;
	
	
	
	}
	
	else{
	
	 gl_Position = Projection * ModelView * vPosition;
	}
  }

  else{
	
	 gl_Position = Projection * ModelView * vPosition;
	 Color=fColor;
	}
    }
}

