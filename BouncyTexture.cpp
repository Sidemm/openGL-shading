//
//  Display a rotating cube 
//

#include "Angel.h"
#include "math.h"

#define N 4  // number of subdivisions
#define M 16*64*3  // number of resulting points


#define CUBE 0
#define SPHERE 1

typedef vec4  color4;
typedef vec4  point4;

const int NumVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)

point4 points[NumVertices];
color4 colors[NumVertices];

point4 points2[M];



// Array of rotation angles (in degrees) for each coordinate axis
enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
int  Axis = Yaxis;
GLfloat  Theta[NumAxes] = { 0.0, 30.0, 40.0 };


// Vertices of a unit cube centered at origin, sides aligned with axes
point4 vertices[8] = {
	point4(-0.1, -0.1,  0.1, 1.0),
	point4(-0.1,  0.1,  0.1, 1.0),
	point4(0.1,  0.1,  0.1, 1.0),
	point4(0.1, -0.1,  0.1, 1.0),
	point4(-0.1, -0.1, -0.1, 1.0),
	point4(-0.1,  0.1, -0.1, 1.0),
	point4(0.1,  0.1, -0.1, 1.0),
	point4(0.1, -0.1, -0.1, 1.0)
};




enum color { CYAN, RED, YELLOW, GREEN, BLUE, MAGENTA, WHITE, BLACK };

// RGBA olors
color4 vertex_colors[8] = {
	
	color4(0.0, 1.0, 1.0, 1.0),  // cyan
	color4(1.0, 0.0, 0.0, 1.0),  // red
	color4(1.0, 1.0, 0.0, 1.0),  // yellow
	color4(0.0, 1.0, 0.0, 1.0),  // green
	color4(0.0, 0.0, 1.0, 1.0),  // blue
	color4(1.0, 0.0, 1.0, 1.0),  // magenta
	color4(1.0, 1.0, 1.0, 1.0),  // white
	color4(0.0, 0.0, 0.0, 1.0),  // black
	
};

// Model-view and projection matrices uniform location
GLuint  ModelView, Projection;


float velocity;
float position;
float gravity;
float timer;
float lastposition;
bool flag; //moving up or moving down
GLint Color;
GLint fColor;
float SourceNum;
float ShadingType;
float a, b, c;
mat4  model_view1, model_view2;
GLuint vao[2]; //2 pointer for cube and sphere
int Object;
const vec3 viewer_pos(0.0, 0.0, 2.0);
vec2 tex_coords[M];
//----------------------------------------------------------------------------

// quad generates two triangles for each face and assigns colors
//    to the vertices

int Index = 0;

void
quad(int a, int b, int c, int d)
{
	// Initialize colors

	points[Index] = vertices[a]; Index++;
	points[Index] = vertices[b]; Index++;
	points[Index] = vertices[c]; Index++;
	points[Index] = vertices[a]; Index++;
	points[Index] = vertices[c]; Index++;
	points[Index] = vertices[d]; Index++;

}

//----------------------------------------------------------------------------

// generate 12 triangles: 36 vertices and 36 colors
void
colorcube()
{
	quad(1, 0, 3, 2);
	quad(2, 3, 7, 6);
	quad(3, 0, 4, 7);
	quad(6, 5, 1, 2);
	quad(4, 5, 6, 7);
	quad(5, 4, 0, 1);
}

//----------------------------CIRCLE-----------------------------------------
vec3   normals[M];

point4 v[4] = { vec4(0.0, 0.0, 1.0, 1.0), vec4(0.0, 0.942809, -0.333333, 1.0),
vec4(-0.816497, -0.471405, -0.333333, 1.0),
vec4(0.816497, -0.471405, -0.333333, 1.0) };


static int k = 0;


// move a point to unit circle

point4 unit(const point4 &p)
{
	point4 c;
	double d = 0.0;
	for (int i = 0; i<3; i++) d += p[i] * p[i];
	d = sqrt(d);
	if (d > 0.0) for (int i = 0; i<3; i++) c[i] = p[i] / d;
	c[3] = 1.0;
	return c;
}
void triangle(point4  a, point4 b, point4 c)
{
	points2[k] = a;
	//Computation of texture coordinates of a sphere
	float rh = sqrt(a.x*a.x + a.y*a.y);
	float lon = atan2(a.x, a.y);
	float lat = atan2(a.z, rh);
	float alt = sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
	float s = lon / (2 * M_PI) + 0.5;
	float t = lat / M_PI + 0.5;
	tex_coords[k] = vec2(s,t);
	
	//normal vectors of vertices
	normals[k] = normalize(vec3 (a.x, a.y, a.z));
	k++;
	points2[k] = b;

	 rh = sqrt(b.x*b.x + b.y*b.y);
	 lon = atan2(b.x, b.y);
	 lat = atan2(b.z, rh);
	 alt = sqrt(b.x*b.x + b.y*b.y + b.z*b.z);
	 s = lon / (2 * M_PI) + 0.5;
	 t = lat / M_PI + 0.5;
	tex_coords[k] = vec2(s, t);

	normals[k] = normalize(vec3(b.x, b.y, b.z));
	k++;
	points2[k] = c;
	
	 rh = sqrt(c.x*c.x + c.y*c.y);
	 lon = atan2(c.x, c.y);
	 lat = atan2(c.z, rh);
	 alt = sqrt(c.x*c.x + c.y*c.y + c.z*c.z);
	 s = lon / (2 * M_PI) + 0.5;
	 t = lat / M_PI + 0.5;
	tex_coords[k] = vec2(s, t);
	
	normals[k] = normalize(vec3(c.x, c.y, c.z));
	k++;
	
}


void divide_triangle(point4 a, point4 b, point4 c, int n)
{
	point4 v1, v2, v3;
	if (n>0)
	{
		v1 = unit(a + b);
		v2 = unit(a + c);
		v3 = unit(b + c);
		divide_triangle(a, v2, v1, n - 1);
		divide_triangle(c, v3, v2, n - 1);
		divide_triangle(b, v1, v3, n - 1);
		divide_triangle(v1, v2, v3, n - 1);
	}
	else triangle(a, b, c);
}

void tetrahedron(int n)
{
	divide_triangle(v[0], v[1], v[2], n);
	divide_triangle(v[3], v[2], v[1], n);
	divide_triangle(v[0], v[3], v[1], n);
	divide_triangle(v[0], v[2], v[3], n);
}
 float  material_shininess;
 GLuint program;
 GLuint textures[2];
 GLubyte *image; //ppm basketball
 GLubyte *image2; //ppm earth
 GLuint  TextureFlag;
 bool rotate;

 void ppmreader(char ls[], GLubyte** image) {
	 FILE *fd;
	 int k, n, m;
	 char c;
	 int i;
	 char b[100];
	 float s;
	 int red, green, blue;
	 sprintf(b, ls);
	 fd = fopen(b, "r"); //in first line
	 fscanf(fd, "%[^\n] ", b);
	 if (b[0] != 'P' || b[1] != '3') {

		 printf("%s is not a PPM file!\n, ", b);
		 exit(0);
	 }
	 printf("%s is a PPM file\n", b);
	 //Reading the Header(cont)
	 fscanf(fd, "%c", &c);
	 while (c == '#')
	 {
		 fscanf(fd, "%[^\n]", b);
		 printf("%s\n", b);
		 fscanf(fd, "%c", &c);
	 }
	 ungetc(c, fd);
	 //skip over comments by looking for # in first column
	 //Reading the Data
	 fscanf(fd, "%d %d %d", &n, &m, &k);
	 printf("%d rows %d columns max value= %d\n", n, m, k);
	 int nm = n*m;
	 *image = (GLubyte*)malloc(3 * sizeof(GLubyte)*nm);
	 for (i = nm; i > 0; i--)
	 {
		 fscanf(fd, "%d %d %d", &red, &green, &blue);
		 (*image)[3 * nm - 3 * i] = red;
		 (*image)[3 * nm - 3 * i + 1] = green;
		 (*image)[3 * nm - 3 * i + 2] = blue;
	 }
 }


 
 
// OpenGL initialization
void
init()
{
	rotate = true;
	velocity = 0.0;
	gravity = -9.8;
	position = 0.0;
	timer = 0.0;
	lastposition = 0.0;
	flag = true;
	Object = 1;
	colorcube();
	tetrahedron(N);
    
	program = InitShader("vshader.glsl", "fshader.glsl");
	
	
	ppmreader("basketball.ppm", &image);
	ppmreader("earth.ppm", &image2);
	// compansate elapsed time while texture coordinates computation
	velocity = 0;
	timer = glutGet(GLUT_ELAPSED_TIME);
	flag = true;

	//Texturing Code
	glGenTextures(2, textures);
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 256, 0,
		GL_RGB, GL_UNSIGNED_BYTE, image);
	
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_LINEAR); //try here different alternatives
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //try here different alternatives

	glBindTexture(GL_TEXTURE_2D, textures[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2048, 1024, 0,
		GL_RGB, GL_UNSIGNED_BYTE, image2);
	//glGenerateMipmap(GL_TEXTURE_2D);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	//glBindTexture(GL_TEXTURE_2D, textures[0]); //set current texture
	
	
	// Create a vertex array object
	glGenVertexArrays(2, &vao[0]); //two vertex arrays created
	glBindVertexArray(vao[0]);
	
	// Create and initialize a buffer object
	GLuint buffer1; //buffer for the cube
	glGenBuffers(1, &buffer1);
	glBindBuffer(GL_ARRAY_BUFFER, buffer1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);

	// Load shaders and use the resulting shader program

	// set up vertex arrays
	GLuint vPosition1 = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition1);
	glVertexAttribPointer(vPosition1, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glBindVertexArray(vao[1]);
	GLuint buffer2; //buffer for the sphere
	glGenBuffers(1, &buffer2);
	glBindBuffer(GL_ARRAY_BUFFER, buffer2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points2) + sizeof(normals) + sizeof(tex_coords), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points2), points2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points2), sizeof(normals), normals);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points2)+sizeof(normals), sizeof(tex_coords), tex_coords);



	GLuint vPosition2 = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition2);
	glVertexAttribPointer(vPosition2, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));

	GLuint vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points2)));
	
	GLuint vTexCoord = glGetAttribLocation(program, "vTexCoord");
	glEnableVertexAttribArray(vTexCoord);
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(sizeof(points2)+sizeof(normals)));

	TextureFlag = glGetUniformLocation(program, "TextureFlag");
	glUniform1i(TextureFlag, -1);

	// Retrieve transformation uniform variable locations
	ModelView = glGetUniformLocation(program, "ModelView");
	Projection = glGetUniformLocation(program, "Projection");

	// Set current program object
	glUseProgram(program);

	model_view1 = Translate(-viewer_pos);
	model_view2 = Translate(-viewer_pos);
	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view2);

	// Initialize shader lighting parameters
	point4 light_position = ( -2.0, 0.0, 2.0, 1.0 );
	color4 light_ambient(0.3, 0.3, 0.3, 1.0);
	color4 light_diffuse(1.0, 1.0, 1.0, 1.0);
	color4 light_specular(1.0, 1.0, 1.0, 1.0);
	/*
	m = identity(); // shadow projection matrix initially an identity matrix
	m[3][2] = -1.0 / light_position[2];
	m[3][3] = 0.0;
	*/
	point4 light_position2(5.0, 0.0, 2.0, 0.0);
	color4 light_ambient2(0.2, 0.2, 0.2, 1.0);
	color4 light_diffuse2(1.0, 1.0, 1.0, 1.0);
	color4 light_specular2(1.0, 1.0, 1.0, 1.0);
	
	color4 material_ambient(0.0, 1.0, 1.0, 1.0);
	color4 material_diffuse(1.0, 0.8, 0.4, 1.0);
	color4 material_specular(1.0, 0.8, 0.0, 1.0);
	
	color4 diffuse_product = light_diffuse * material_diffuse;
	color4 specular_product = light_specular * material_specular;
	color4 diffuse_product2 = light_diffuse2 * material_diffuse;
	color4 specular_product2 = light_specular2 * material_specular;

	glUniform4fv(glGetUniformLocation(program, "MaterialAmbient"),
		1, material_ambient);
	glUniform4fv(glGetUniformLocation(program, "LightAmbient"),
		1, light_ambient);
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"),
		1, diffuse_product);
	glUniform4fv(glGetUniformLocation(program, "SpecularProduct"),
		1, specular_product);
	glUniform4fv(glGetUniformLocation(program, "LightPosition"),
		1, light_position);
	
	glUniform4fv(glGetUniformLocation(program, "LightAmbient2"),
		1, light_ambient2);
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct2"),
		1, diffuse_product2);
	glUniform4fv(glGetUniformLocation(program, "SpecularProduct2"),
		1, specular_product2);
	glUniform4fv(glGetUniformLocation(program, "LightPosition2"),
		1, light_position2);
	
	// Enable hiddden surface removal
	glEnable(GL_DEPTH_TEST);
	
	//Enable Culling
	glEnable(GL_CULL_FACE);

	// Set state variable "clear color" to clear buffer with.
	glClearColor(1.0, 1.0, 1.0, 1.0);

	
	material_shininess = glGetUniformLocation(program, "Shininess");
	glUniform1f(material_shininess, 100.0);
	
	//For shading
	Color = glGetUniformLocation(program, "Color");
	glUniform4f(Color, 0.0, 1.0, 1.0, 1.0);

	//For changing objects Color while shading
	fColor = glGetUniformLocation(program, "fColor");
	glUniform4f(fColor, 1.0, 0.0, 0.0, 1.0);
	

	
	//User selection of number of light sources passed to fshader and vshader 
	SourceNum = glGetUniformLocation(program, "SourceNum");
	glUniform1f(SourceNum, 0.0);

	ShadingType = glGetUniformLocation(program, "ShadingType");
	glUniform1f(ShadingType, 0.0);
	
	//three coefficients of distance term that are multiplied with the point source in fshader
	a = glGetUniformLocation(program, "A"); 
	glUniform1f(a, 1);
	b = glGetUniformLocation(program, "B");
	glUniform1f(a, 1);
	c = glGetUniformLocation(program, "C");
	glUniform1f(a, 1);

}

//----------------------------------------------------------------------------

void
display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// V=V.t(time elapsed between ground and max height)-g*t^2
	velocity = sqrt(lastposition * 2 / gravity)*gravity - gravity*(glutGet(GLUT_ELAPSED_TIME) - timer)*0.001;

	if (flag == true) //moving down
		position = 0.5*gravity*(glutGet(GLUT_ELAPSED_TIME) - timer)*(glutGet(GLUT_ELAPSED_TIME) - timer)*0.000001;

	if (flag == false)  //moving up
		position = -((velocity*((glutGet(GLUT_ELAPSED_TIME) - timer)*0.001)) + (0.5*gravity*((glutGet(GLUT_ELAPSED_TIME) - timer)*0.001)*((glutGet(GLUT_ELAPSED_TIME) - timer)*0.001))) + lastposition;
	if (Object == CUBE) {
		if (position < -1.7) { //when object is on the ground 
			timer = glutGet(GLUT_ELAPSED_TIME);
			lastposition = position;
			flag = false;
		}
	}

	if (Object == SPHERE) {
		if (position < -1.65) { //when object is on the ground 
			timer = glutGet(GLUT_ELAPSED_TIME);
			lastposition = position;
			flag = false;
		}
	}

	if (position == 0.0) { // when object is at max height 
		timer = glutGet(GLUT_ELAPSED_TIME);
		flag = true;
	}

	vec3 displacement(0.0, position + 0.85, -2.3); //positions arranged with respect to the opengl window
	vec3 displacementsphere(0.0, (position + 0.8) * 5, -12.5);

	// Scale(), Translate(), RotateX(), RotateY(), RotateZ(): user-defined functions in mat.h


	if (Object == CUBE) {
		glBindVertexArray(vao[0]); //display cube
		model_view1 = (Scale(1.0, 1.0, 1.0) * Translate(displacement)*
			RotateX(Theta[Xaxis]) *
			RotateY(Theta[Yaxis]) *
			RotateZ(Theta[Zaxis]));
		glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view1);//  Generate the model-view matrix of cube
		glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	}
	else if (Object == SPHERE) {
		glBindVertexArray(vao[1]); //display sphere
		model_view2 = (Scale(0.2, 0.2, 0.2) * Translate(displacementsphere)*
			RotateX(Theta[Xaxis]) *
			RotateY(Theta[Yaxis]) *
			RotateZ(Theta[Zaxis])
			);
		glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view2);//  Generate the model-view matrix of sphere
		glDrawArrays(GL_TRIANGLES, 0, M);
	}

	glutSwapBuffers();

}

//----------------------------------------------------------------------------

void
idle(void)
{
	Theta[Yaxis] += 0.02;
	Theta[Zaxis] = 45.0;


	if (Theta[Xaxis] > 360.0) {
		Theta[Xaxis] -= 360.0;
	}

	glutPostRedisplay();
}

//----------------------------------------------------------------------------

void
keyboard(unsigned char key, int x, int y)
{
	//Up and down arrow inputs are handled in ArrowInput() function

	if (key == 'Q' | key == 'q')
		exit(0);
	if (key == 'i' | key == 'I')
	{
		velocity = 0;
		timer = glutGet(GLUT_ELAPSED_TIME);
		flag = true;

	}
	if (key == 'h' | key == 'H')
	{
		std::cout << "Available input commands:" << std::endl;
		std::cout << "Q: Exit program" << std::endl;
		std::cout << "Up Key(W): Speeds up movement" << std::endl;
		std::cout << "Down Key(S): Slows down movement" << std::endl;
		std::cout << "I: Return initial position" << std::endl;
	}
}

//----------------------------------------------------------------------------
void reshape(int w, int h)
{
	glViewport(0, 0, w, h);


	GLfloat aspect = GLfloat(w) / h;
	mat4  projection = Perspective(45.0, 1.0, 0.5, 3.0);

	glUniformMatrix4fv(Projection, 1, GL_TRUE, projection);

}

//----------------------------------------------------------------------------
void selectColor(int color)
{
	glUniform4f(fColor, vertex_colors[color].x, vertex_colors[color].y, vertex_colors[color].z, vertex_colors[color].w);
}
void selectDisplay(int draw) {
	if (draw == 0) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glUniform1f(ShadingType, -1.0);
		glUniform1i(TextureFlag, 0);
	}
	else if (draw == 1) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glUniform1f(ShadingType, 0.0);
		glUniform1i(TextureFlag, 0);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glUniform1i(TextureFlag, 1);
		glUniform1f(ShadingType, -1.0);

	}


}
void selectObject(int selection) {
	Object = selection;
}

void selectLights(int num) {
	
	glUniform1f(SourceNum, (float)num);
}

void selectShading(int selection) {

	glUniform1f(ShadingType, (float)selection);
}

void selectMaterial(int selection) {
	if(selection==0)
	glUniform1f(material_shininess, 100.0);
	else
    glUniform1f(material_shininess, 5.0);

}

void selectTexture(int selection) {
	if (selection == 0)
		glBindTexture(GL_TEXTURE_2D, textures[0]);

	else
		glBindTexture(GL_TEXTURE_2D, textures[1]);


}

//For detecting keyboard inputs up and down arrow 
void ArrowInput(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:
		gravity = gravity*1.5;

		break;
	case GLUT_KEY_DOWN:
		gravity = gravity / 1.5;

		break;
	}
}


int
main(int argc, char **argv)
{

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(512, 512);
	glutCreateWindow("SphereTexture");
	glutSpecialFunc(ArrowInput);

	glewExperimental = GL_TRUE;
	glewInit();

	init();

	glutDisplayFunc(display); // set display callback function
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);

	int colorMenu = glutCreateMenu(selectColor);
	glutAddMenuEntry("cyan", CYAN);
	glutAddMenuEntry("red", RED);
	glutAddMenuEntry("yellow", YELLOW);
	glutAddMenuEntry("green", GREEN);
	glutAddMenuEntry("blue", BLUE);
	glutAddMenuEntry("magenta", MAGENTA);
	glutAddMenuEntry("black", BLACK);

	int DisplayMenu = glutCreateMenu(selectDisplay);
	glutAddMenuEntry("Wireframe", 0);
	glutAddMenuEntry("Shading", 1);
	glutAddMenuEntry("Texture", 2);



	int objectMenu = glutCreateMenu(selectObject);
	glutAddMenuEntry("cube", 0);
	glutAddMenuEntry("sphere", 1);

	int LightSourcesMenu = glutCreateMenu(selectLights);
	glutAddMenuEntry("point light on", 0);
	glutAddMenuEntry("direcitonal light on", 1);
	glutAddMenuEntry("both lights on", 2);
	glutAddMenuEntry("both lights off", 3);

	int ShadingTypeMenu = glutCreateMenu(selectShading);
	glutAddMenuEntry("Modified Phong", 0);
	glutAddMenuEntry("Gouraud", 1);
	glutAddMenuEntry("Phong", 2);

	
	int ObjectMaterialMenu = glutCreateMenu(selectMaterial);
	glutAddMenuEntry("Metallic", 0);
	glutAddMenuEntry("Plastic", 1);
	
	int TextureMenu = glutCreateMenu(selectTexture);
	glutAddMenuEntry("Basketball", 0);
	glutAddMenuEntry("Earth", 1);



	glutCreateMenu(0);
	glutAddSubMenu("Colors", colorMenu);
	glutAddSubMenu("Display", DisplayMenu);
	glutAddSubMenu("Light sources", LightSourcesMenu);
	glutAddSubMenu("Shading types", ShadingTypeMenu);
	glutAddSubMenu("Object material", ObjectMaterialMenu);
	glutAddSubMenu("Texture",TextureMenu);




	glutAttachMenu(GLUT_LEFT_BUTTON);

	glutMainLoop();

	return 0;
}
