#ifndef GLHELPER_H
#define GLHELPER_H

#include <cstdio>

#ifdef LGCK_OPENGL_DEBUG
static void opengl_msg(unsigned int code, const char *file, int line);
void opengl_msg(unsigned int code, const char *file, int line){
    char tmp[128];
    tmp[0] = 0;
    switch(code) {
        case GL_NO_ERROR:
            strcpy(tmp, "GL_NO_ERROR");
        break;
        case GL_INVALID_ENUM:
            strcpy(tmp, "GL_INVALID_ENUM");
        break;
        case GL_INVALID_VALUE:
            strcpy(tmp, "GL_INVALID_VALUE");
        break;
        case GL_INVALID_OPERATION:
            strcpy(tmp, "GL_INVALID_OPERATION");
        break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            strcpy(tmp, "GL_INVALID_FRAMEBUFFER_OPERATION");
        break;
        case GL_OUT_OF_MEMORY:
            strcpy(tmp, "GL_OUT_OF_MEMORY");
        break;
        default:
            sprintf(tmp, "GL UNKNOWN:%u", code);
    }
    if (code != GL_NO_ERROR) {
        qDebug("Opengl error: %s in %s line %d",
            tmp, file, line); \
    }
}
    #define GLDEBUG() opengl_msg(glGetError(), __FILE__, __LINE__ );
#else
	#define GLDEBUG()
#endif	
	
#endif // GLHELPER_H
