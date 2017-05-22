
#define S static

S const char *vertex_shader_source =
"#version 150\n"
"precision highp float;"
	"in vec4 q;"
	"in vec4 p;"
	"in vec4 u;"
	"uniform sampler2D s;"
	"uniform vec2 wh;"
 	"uniform float t;"
	"out vec2 uv;"
"void main() {"
	"vec2 qq = p.xy + q.xy*p.zw;"
	"qq = 2*qq/wh;"
	"qq += vec2(1,1)/wh;"
	"qq += vec2(-1,-1);"
	"gl_Position.xy = qq;"
	"gl_Position.zw = vec2(0,1);"
	"uv = (u.xy + u.zw * q.zw) / 2048;"
"}";


S const char *fragment_shader_source =
"#version 150\n"
"precision highp float;"
	"in vec2 uv;"
	"out vec4 color;"
	"uniform sampler2D s;"
  	"uniform float t;"
"void main() {"
 	"float idx = texture(s,uv).r*255;"
	"color = texture(s,vec2((0.5+idx)/2048, 0.5/2048));"
"}";

#include "gl.h"
#include <stdio.h>
#define GL3_PROTOTYPES
#include <OpenGL/gl3.h>
#define __gl_h_
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <sys/time.h>

#if 0
S double gl_profile_t = 0;
S void gl_profile(const char *file, int line, const char *func) {
        struct timeval tv;
        gettimeofday(&tv, 0);
        double now = tv.tv_sec*1e6 + tv.tv_usec;
        printf("PF: %0.2fus at %s %s:%u\n", now-gl_profile_t, func, file, line);
        gl_profile_t=now;
}
#endif
S void gl_error(const char *file, int line, const char *func) {
        GLenum err = glGetError();
        if(err == GL_NO_ERROR) return;
        printf("GL error %x in %s at %s:%u\n", err, func, file, line);
        abort();
}
#define GE gl_error(__FILE__, __LINE__, __FUNCTION__);



#define PK __attribute__((__packed__))
#define GDS(x,y) glGenBuffers(1,&x);GE;glBindBuffer(GL_ARRAY_BUFFER,x);GE;glBufferData(GL_ARRAY_BUFFER,sizeof(y),y,GL_STATIC_DRAW);GE;
#define GDD(x,y) glGenBuffers(1,&x);GE;glBindBuffer(GL_ARRAY_BUFFER,x);GE;glBufferData(GL_ARRAY_BUFFER,sizeof(y),y,GL_DYNAMIC_DRAW);GE;
#define E(x) glEnable(GL_##x);GE;
#define BT glBindTexture(GL_TEXTURE_2D, texture);GE;
#define TPE(x) glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_##x,GL_CLAMP_TO_EDGE);GE;
#define TPN(x) glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_##x##_FILTER,GL_NEAREST);GE;
#define LT(x) glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,2048,2048,0,GL_RGBA,GL_UNSIGNED_BYTE,x);GE;

S const char *vertex_shader;S const char *fragment_shader;
S struct{uint16_t x,y,w,h;}PK spritesheet_coords[131072];S struct{int16_t x,y,w,h;}PK screen_coords[131072];
S GLuint program;S GLuint texture;S GLuint quad_vbo;S GLuint screen_vbo;S GLuint spritesheet_vbo;
S const GLfloat quad_vertices[]={0,0,0,0,0,1,0,1,1,0,1,0,1,1,1,1,};
S GLuint vao;

S void gl_setup_shaders(void) {
	vertex_shader = vertex_shader_source;
	fragment_shader = fragment_shader_source;
	GLuint v, f, p;
	char log[10240];
	v = glCreateShader(GL_VERTEX_SHADER); GE
	glShaderSource(v, 1, &vertex_shader, 0); GE
	glCompileShader(v); GE
	glGetShaderInfoLog(v, 10240, 0, log); GE
	printf("vertex shader: %s\n", log);
	log[0] = 0;
	f = glCreateShader(GL_FRAGMENT_SHADER); GE
	glShaderSource(f, 1, &fragment_shader, 0); GE
	glCompileShader(f); GE
	glGetShaderInfoLog(f, 10240, 0, log); GE
	printf("fragment shader: %s\n", log);
	log[0] = 0;
	p = glCreateProgram(); GE
	glAttachShader(p,v); GE
	glAttachShader(p,f); GE
	glBindFragDataLocation(p, 0, "color"); GE
	glLinkProgram(p); GE
	glGetProgramInfoLog(p, 10240, 0, log); GE
	printf("program: %s\n", log);
	program = p;
}

S GLint shader_q;S GLint shader_p;S GLint shader_u;S GLint shader_s;S GLint shader_wh;S GLint shader_t;
#define BA(x) shader_##x=glGetAttribLocation(program,#x);GE
#define BU(x) shader_##x=glGetUniformLocation(program,#x);GE

S void gl_setup_shader_bindings(void) { BA(q); BA(p); BA(u); BU(s); BU(wh); BU(t); }
S void gl_setup_texture(void){glGenTextures(1,&texture);GE;BT;TPE(S);TPE(T);TPN(MIN);TPN(MAG);LT(NULL);}
void gl_load_texture(void *texture_data) {BT;LT(texture_data);}
S void gl_setup_context(void) {E(BLEND);E(CULL_FACE);
	glBlendFuncSeparate(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA,GL_ONE,GL_ONE);GE;glCullFace(GL_FRONT);
	glClearColor(0.45,0.45,0.45,0);GE;glViewport(0,0,2*400,2*224);GE;
	GDS(quad_vbo,quad_vertices);GDS(screen_vbo,screen_coords);GDS(spritesheet_vbo,spritesheet_coords);
	glBindBuffer(GL_ARRAY_BUFFER,0);GE;glGenVertexArrays(1,&vao);glBindVertexArray(vao);}
void gl_init(void){gl_setup_context();gl_setup_texture();gl_setup_shaders();gl_setup_shader_bindings();}

