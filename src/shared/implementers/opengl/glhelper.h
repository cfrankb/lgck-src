#ifndef GLHELPER_H
#define GLHELPER_H

#include <cstdio>

static const char *opengl_msg(unsigned int code);

const char *opengl_msg(unsigned int code){
    static char tmp[128];
    switch(code) {
        case GL_NO_ERROR:
            return "GL_NO_ERROR";
        case GL_INVALID_ENUM:
            return "GL_INVALID_ENUM";
        case GL_INVALID_VALUE:
            return "GL_INVALID_VALUE";
        case GL_INVALID_OPERATION:
            return "GL_INVALID_OPERATION";
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            return "GL_INVALID_FRAMEBUFFER_OPERATION";
        case GL_OUT_OF_MEMORY:
            return "GL_OUT_OF_MEMORY";
        default:
            sprintf(tmp, "GL UNKNOWN:%u", code);
            return (const char*)tmp;
    }
}

static unsigned int __ERR__;
/*
#define GLDEBUG() __ERR__ = glGetError(); if (__ERR__ != GL_NO_ERROR) { \
    qDebug("Opengl error: %s in %s line %d", \
        opengl_msg(__ERR__), __FILE__, __LINE__); \
    }
*/
#define GLDEBUG() (1==1);
#endif // GLHELPER_H
