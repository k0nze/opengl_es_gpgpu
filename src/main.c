#include <EGL/egl.h>
#include <GL/glew.h>
#include <stdlib.h>
#include <stdio.h>

static const EGLint configAttribs[] = {EGL_SURFACE_TYPE,
                                       EGL_PBUFFER_BIT,
                                       EGL_BLUE_SIZE,
                                       8,
                                       EGL_GREEN_SIZE,
                                       8,
                                       EGL_RED_SIZE,
                                       8,
                                       EGL_DEPTH_SIZE,
                                       8,
                                       EGL_RENDERABLE_TYPE,
                                       EGL_OPENGL_BIT,
                                       EGL_NONE};

static const int pbufferWidth = 9;
static const int pbufferHeight = 9;

static const EGLint pbufferAttribs[] = {
    EGL_WIDTH, 9, EGL_HEIGHT, 9, EGL_NONE,
};

int main(int argc, char *argv[]) {
    // 1. Initialize EGL
    EGLDisplay eglDpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    EGLint major, minor;

    eglInitialize(eglDpy, &major, &minor);

    // 2. Select an appropriate configuration
    EGLint numConfigs;
    EGLConfig eglCfg;

    eglChooseConfig(eglDpy, configAttribs, &eglCfg, 1, &numConfigs);

    // 3. Create a surface
    EGLSurface eglSurf = eglCreatePbufferSurface(eglDpy, eglCfg, pbufferAttribs);

    // 4. Bind the API
    eglBindAPI(EGL_OPENGL_API);

    // 5. Create a context and make it current
    EGLContext eglCtx = eglCreateContext(eglDpy, eglCfg, EGL_NO_CONTEXT, NULL);

    eglMakeCurrent(eglDpy, eglSurf, eglSurf, eglCtx);

    // from now on use your OpenGL context
    int i; 
    int texSize = 2;
    float* data = (float*) malloc(4*texSize*texSize*sizeof(float));
    float* result = (float*) malloc(4*texSize*texSize*sizeof(float));

    // fill data
    for(i = 0; i < texSize*texSize*4; i++) {
        data[i] = 1.0 + i;
    }

    glewInit();

    // create FBO and bind it (that is, use offscreen render target)
    GLuint fb;
    glGenFramebuffersEXT(1,&fb); 
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,fb);

    // create texture
    GLuint tex;
    glGenTextures (1, &tex);
    glBindTexture(GL_TEXTURE_RECTANGLE_ARB,tex);

    // set texture parameters
    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, 
                    GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, 
                    GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, 
                    GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, 
                    GL_TEXTURE_WRAP_T, GL_CLAMP);

    // define texture with floating point format
    glTexImage2D(GL_TEXTURE_RECTANGLE_ARB,0,GL_RGBA32F_ARB,
                 texSize,texSize,0,GL_RGBA,GL_FLOAT,0);


    // attach texture
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, 
                              GL_COLOR_ATTACHMENT0_EXT, 
                              GL_TEXTURE_RECTANGLE_ARB,tex,0);

    // transfer data to texture
    glTexSubImage2D(GL_TEXTURE_RECTANGLE_ARB,0,0,0,texSize,texSize,
                    GL_RGBA,GL_FLOAT,data);

    // and read back
    //glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
    glReadPixels(0, 0, texSize, texSize,GL_RGBA,GL_FLOAT,result);

    // print data
    printf("data, result:\n");
    for(i = 0; i < texSize*texSize*4; i++) {
        printf("%f, %f\n", data[i], result[i]);
    }

    free(data);
    free(result);

    // 6. Terminate EGL when finished
    glDeleteFramebuffersEXT (1,&fb);
    glDeleteTextures (1,&tex);
    eglTerminate(eglDpy);

    return 0;
}
