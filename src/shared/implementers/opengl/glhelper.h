/*
    LGCK Builder Runtime
    Copyright (C) 1999, 2020  Francois Blanchette

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
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
