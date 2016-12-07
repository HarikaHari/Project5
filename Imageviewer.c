#define GLFW_DLL 1

#define GL_GLEXT_PROTOTYPES
#include <GLES2/gl2.h>
#include <GLFW/glfw3.h>

#include "linmath.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

typedef struct pixel {
    float r, g, b;
} pixel;

/* typedef struct Image {
    int width, height;
    pixel* pixmap;
} Image; */

typedef struct PPM
{
    int width, height;
    unsigned char *data;
} PPM;

// Create the default structure for the vertex 
typedef struct
{
  float Position[2];
  float TexCoord[2];
} Vertex;


PPM* readImage(char* name) {
   	// Variables to Read the actual image into buffer
    FILE *filePath;
    char c;
    int ppmType, width, height, maxColor;
    int i, j, size, pixel;

    // start of reading the pixel data into buffer
    PPM *tempImage = (PPM *)malloc(sizeof(PPM));
	//opening the PPM file 
    filePath = fopen (name, "r");
    if(filePath == NULL)
    {
        fprintf(stderr, "\nERROR: Error in opening or reading file ");
        fclose(filePath);
        free(tempImage);
        exit(-1);
    }
    fscanf(filePath, "P%c\n", &c);
    ppmType = c -'0';
    if (ppmType != 6 && ppmType != 3 ) 
    {//if not in either p6 or p3 format then exit
        fprintf(stderr, "\nERROR: This is not in the correct ppm format!");
        free(tempImage);
        fclose(filePath);
        exit(-1);
    }
    c = getc(filePath);
    //skip the comments since they do not matter
    while(c =='#')
    {
        c = getc(filePath);
        while(c!='\n') //read to the end of the line
        {
            c = getc(filePath);
        }
    }
    ungetc(c, filePath);
    fscanf(filePath, "%d %d %d\n", &width, &height, &maxColor);
    if(maxColor > 255 || maxColor <= 0){
        fprintf(stderr,"\nERROR: Image is not 8 bits per channel!");
        fclose(filePath);
        free(tempImage);
        exit(-1);
    }
    size = width * height * 3;
    if(!tempImage)
    {
        fprintf(stderr, "\nERROR: Error in memory allocation");
        fclose(filePath);
        free(tempImage);
        exit(-1);
    }
    else{
        tempImage->width = width;
        tempImage->height = height;
        tempImage->data = (unsigned char *)malloc(width*height*3);
    }

    if(!tempImage->data){
        fprintf(stderr,"\nERROR:  Error in memory allocation for image pixels data");
        fclose(filePath);
        free(tempImage);
        exit(-1);
    }

    if(ppmType == 6)
        fread((void *) tempImage->data, 1, (size_t) size, filePath);
         
    
    else if(ppmType == 3)
    {
        for(i=0;i<height;i++)
        {
            for(j=0;j<width;j++)
            {
                fscanf(filePath, "%d ", &pixel);
                tempImage->data[i*width*3+3*j] = pixel;
                fscanf(filePath, "%d ", &pixel);
                tempImage->data[i*width*3+3*j+1] = pixel;
                fscanf(filePath, "%d ", &pixel);
                tempImage->data[i*width*3+3*j+2] = pixel;
            }
        }
    }
	//end of reading the pixel data into buffer
	fclose(filePath);
	return tempImage;
}



Vertex vertexes[] = {
  {{1, -1}, {0.99999, 0.99999}},
  {{1, 1},  {0.99999, 0}},
  {{-1, 1}, {0, 0}},
  {{-1, 1}, {0, 0}},
  {{-1, -1}, {0, 0.99999}},
  {{1, -1}, {0.99999, 0.99999}}
};


const double pi = 3.1415926535897;
float rotation = 0;
float scale = 1;
float translate_x = 0;
float translate_y = 0;
float shear_x = 0;
float shear_y = 0;


static const char* vertex_shader_text =
    "uniform mat4 MVP;\n"
    "attribute vec2 TexCoordIn;\n"
    "attribute vec2 vPos;\n"
    "varying vec2 TexCoordOut;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
    "    TexCoordOut = TexCoordIn;\n"
    "}\n";


static const char* fragment_shader_text =
    "varying lowp vec2 TexCoordOut;\n"
    "uniform sampler2D Texture;\n"
    "void main()\n"
    "{\n"
    "    gl_FragColor = texture2D(Texture, TexCoordOut);\n"
    "}\n";


static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}



static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // Hit escape to quite the ez-view program
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    // Rotate the image Left wise 90 degrees using W key
    if (key == GLFW_KEY_Q && action == GLFW_PRESS)
    	rotation += 90*pi/180;

    // Rotate the image Right wise 90 degrees using E key
    if (key == GLFW_KEY_E && action == GLFW_PRESS)
    	rotation -= 90*pi/180;

    // Zoom into the image using =	key
    if (key == GLFW_KEY_EQUAL && action == GLFW_PRESS)//

    	scale *= 2;

    // Zoom out of the image using - key
    if (key == GLFW_KEY_MINUS && action == GLFW_PRESS)
    	scale *= .5;

    // Translate the image down using down arrow
    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
    	translate_y += .1;

    // Translate the image Up using up arrow
    if (key == GLFW_KEY_UP && action == GLFW_PRESS)
    	translate_y -= .1;

    // Translate the image left using left arrow
    if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
    	translate_x += .1;

    // Translate the image right using right arrow
    if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
    	translate_x -= .1;

    // Shear image upward using W key
    if (key == GLFW_KEY_W && action == GLFW_PRESS) //Shear Up
    	shear_y += .1;

    // Shear image downward using S key
    if (key == GLFW_KEY_S && action == GLFW_PRESS) //Shear Down
    	shear_y -= .1;

    // Shear image right using D key
    if (key == GLFW_KEY_D && action == GLFW_PRESS) //Shear Right
    	shear_x += .1;

    // Shear image left using A key
    if (key == GLFW_KEY_A && action == GLFW_PRESS) //Shear Left
    	shear_x -= .1;
}


void glCompileShaderOrDie(GLuint shader)
{
    GLint compiled;
    glCompileShader(shader);
    glGetShaderiv(shader,
        GL_COMPILE_STATUS,
        &compiled);

    if (!compiled)
    {
        GLint infoLen = 0;
        glGetShaderiv(shader,
              GL_INFO_LOG_LENGTH,
              &infoLen);
        char* info = malloc(infoLen+1);
        GLint done;
        glGetShaderInfoLog(shader, infoLen, &done, info);
        printf("Unable to compile shader: %s\n", info);
        exit(-1);
    }
}




int main (int argc, char *argv[]) {

    GLFWwindow* window;
    GLuint vertex_buffer, vertex_shader, fragment_shader, program;
    GLint mvp_location, vpos_location;		
    // Check input arguments
    if (argc != 2) {
        fprintf(stderr, "Error: Please provide the file name as an argument\n");
        return 1;
    }	
    char *fileName = argv[1];
    // start of code to load ppm image file
  	PPM *image = readImage(fileName);
    if (image->width < 0) {
        fprintf(stderr, "Error: Error in  loading image file.\n");
        exit(1);
    }	
	if (!glfwInit())
        exit(EXIT_FAILURE);
    // Same hints used in the demo provided
	glfwDefaultWindowHints();
	glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    window = glfwCreateWindow(640, 480, "Image Viewer", NULL, NULL);
    // window initialization 
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    // Turn on key callback in order to take in the user inputs
    glfwSetKeyCallback(window, key_callback);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    // NOTE: OpenGL error checks have been omitted for brevity
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexes), vertexes, GL_STATIC_DRAW);
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    glCompileShaderOrDie(vertex_shader);
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    glCompileShaderOrDie(fragment_shader);
    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    mvp_location = glGetUniformLocation(program, "MVP");
    assert(mvp_location != -1);
    vpos_location = glGetAttribLocation(program, "vPos");
    assert(vpos_location != -1);
    GLint texcoord_location = glGetAttribLocation(program, "TexCoordIn");
    assert(texcoord_location != -1);
    GLint tex_location = glGetUniformLocation(program, "Texture");
    assert(tex_location != -1);
    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(Vertex),
                          (void*) 0);

    glEnableVertexAttribArray(texcoord_location);
    glVertexAttribPointer(texcoord_location,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(Vertex),
                          (void*) (sizeof(float) * 2));

    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGB,
                 image->width,
                 image->height,
                 0,
                 GL_RGB,
                 GL_UNSIGNED_BYTE,
                 image->data);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texID);
    glUniform1i(tex_location, 0);
    while (!glfwWindowShouldClose(window))
    {
        float ratio;
        int windowWidth, windowHeight;
        mat4x4 r, h, s, t, rh, rhs, mvp; 
        glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
        ratio = windowWidth / (float) windowHeight;
        glViewport(0, 0, windowWidth, windowHeight);
        glClear(GL_COLOR_BUFFER_BIT);
        mat4x4_identity(r);
        mat4x4_rotate_Z(r, r, rotation); 
        mat4x4_identity(h);
        h[0][1] = shear_x;
        h[1][0] = shear_y;
        mat4x4_identity(s); 
        s[0][0] = s[0][0]*scale;
        s[1][1] = s[1][1]*scale;
        mat4x4_identity(t);
        mat4x4_translate(t, translate_x, translate_y, 0);
        mat4x4_mul(rh, r, h); 
        mat4x4_mul(rhs, rh, s);
        mat4x4_mul(mvp, rhs, t);
        glUseProgram(program);
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    free(image);
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}