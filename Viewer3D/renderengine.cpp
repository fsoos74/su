#include "renderengine.h"

#include <QVector2D>
#include <QVector3D>
#include <GL/glu.h>

#include <renderscene.h>

RenderEngine::RenderEngine()
{
    initializeOpenGLFunctions();
    initShaders();
}

RenderEngine::~RenderEngine()
{

}


#include<iostream>
#include<iomanip>

void RenderEngine::draw(RenderScene* scene, QMatrix4x4 matrix){

    m_VITProgram.bind();
    m_VITProgram.setUniformValue("mvp_matrix", matrix);
    m_VITProgram.setUniformValue("texture", 0);
    foreach( RenderItem* item, scene->items() ){

        if( VIT* vit=dynamic_cast<VIT*>(item)){
            //std::cout<<"drawing vit "<<std::hex<<vit<<" #index="<<vit->indexCount()<<std::endl<<std::flush;
            draw( &m_VITProgram, vit);
        }
    }

    m_VICProgram.bind();
    m_VICProgram.setUniformValue("mvp_matrix", matrix);
    foreach( RenderItem* item, scene->items() ){

        if( VIC* vic=dynamic_cast<VIC*>(item)){
            //std::cout<<"drawing vic "<<std::hex<<vic<<std::dec<<" #index="<<vic->indexCount()<<std::endl<<std::flush;
            draw( &m_VICProgram, vic);
        }
    }
}

void RenderEngine::draw(QOpenGLShaderProgram *program, VIT* vit)
{

    // Tell OpenGL which VBOs to use
    vit->arrayBuffer().bind();
    vit->indexBuffer().bind();
    vit->texture()->bind();

    // Offset for position
    quintptr offset = 0;

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, sizeof(VIT::Vertex));

    // Offset for texture coordinate
    offset += sizeof(QVector3D);

    // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
    int texcoordLocation = program->attributeLocation("a_texcoord");
    program->enableAttributeArray(texcoordLocation);
    program->setAttributeBuffer(texcoordLocation, GL_FLOAT, offset, 2, sizeof(VIT::Vertex));



    glDrawElements(vit->mode(), vit->indexCount(), GL_UNSIGNED_SHORT, 0);

    vit->texture()->release();
    vit->indexBuffer().release();
    vit->arrayBuffer().release();
}

void RenderEngine::draw(QOpenGLShaderProgram *program, VIC* vic)
{

    // Tell OpenGL which VBOs to use
    vic->arrayBuffer().bind();
    vic->indexBuffer().bind();

    // Offset for position
    quintptr offset = 0;

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, sizeof(VIC::Vertex));

    // Offset for color
    offset += sizeof(QVector3D);

    // Tell OpenGL programmable pipeline how to locate vertex color data
    int colorLocation = program->attributeLocation("a_color");
    program->enableAttributeArray(colorLocation);
    program->setAttributeBuffer(colorLocation, GL_FLOAT, offset, 4, sizeof(VIC::Vertex));       // was 3 instead 4


    glDrawElements( vic->mode(), vic->indexCount(), GL_UNSIGNED_SHORT, 0);

    vic->indexBuffer().release();
    vic->arrayBuffer().release();
}


static const char* textureVertexShaderSource =
    #ifdef GL_ES
    "precision mediump int;\n"
    "precision mediump float;\n"
    #endif
    "uniform mat4 mvp_matrix;\n"
    "attribute vec4 a_position;\n"
    "attribute vec2 a_texcoord;\n"
    "varying vec2 v_texcoord;\n"
    "void main() {\n"
    "gl_Position = mvp_matrix * a_position;\n"
    "v_texcoord = a_texcoord;\n"
    "}\n";

static const char* textureFragmentShaderSource =
    #ifdef GL_ES
    "precision mediump int;\n"
    "precision mediump float;\n"
    #endif
    "uniform sampler2D texture;\n"
    "varying vec2 v_texcoord;\n"
    "void main(){\n"
//    "gl_FragColor = texture2D(texture, v_texcoord);\n"
// the next 4 lines replace above line because of transparency handling with depth test
    "vec4 texel = texture2D(texture, v_texcoord);\n"
    "if(texel.a < 0.5)\n"
    "       discard;\n"
    "gl_FragColor = texel;\n"
    "}\n";

static const char *colorVertexShaderSource =
    #ifdef GL_ES
    "precision mediump int;\n"
    "precision mediump float;\n"
    #endif
     "uniform mat4 mvp_matrix;\n"
     "attribute vec4 a_position;\n"
     "attribute vec4 a_color;\n"
     "varying vec4 v_color;\n"
     "void main() {\n"
     "gl_Position = mvp_matrix * a_position;\n"
     "v_color = a_color;\n"
     "}\n";

static const char *colorFragmentShaderSource =
    #ifdef GL_ES
    "precision mediump int;\n"
    "precision mediump float;\n"
    #endif
    "varying vec4 v_color;\n"
    "void main(){\n"
    "gl_FragColor = v_color;\n"
    "}\n";

void RenderEngine::initShaders()
{
 /*
    // Compile vertex shader
    if (!textureProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vshader.glsl"))
        close();

    // Compile fragment shader
    if (!textureProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/fshader.glsl"))
        close();
*/
    // Compile vertex shader
    if (!m_VITProgram.addShaderFromSourceCode(QOpenGLShader::Vertex, textureVertexShaderSource))
        qFatal("Compiling VIT Vertex Shader failed: %s", m_VITProgram.log().toStdString().c_str() );

    // Compile fragment shader
    if (!m_VITProgram.addShaderFromSourceCode(QOpenGLShader::Fragment, textureFragmentShaderSource))
        qFatal("Compiling VIT Fragment Shader failed: %s", m_VITProgram.log().toStdString().c_str() );

    // Link shader pipeline
    if (!m_VITProgram.link())
        qFatal("Linking VIT Shader failed: %s", m_VITProgram.log().toStdString().c_str() );

    // Bind shader pipeline for use
    if (!m_VITProgram.bind())
        qFatal("Binding VIT Shader failed: %s", m_VITProgram.log().toStdString().c_str() );


    // Compile vertex shader
    if (!m_VICProgram.addShaderFromSourceCode(QOpenGLShader::Vertex, colorVertexShaderSource))
        qFatal("Compiling VIC Vertex Shader failed: %s", m_VITProgram.log().toStdString().c_str() );

    // Compile fragment shader
    if (!m_VICProgram.addShaderFromSourceCode(QOpenGLShader::Fragment, colorFragmentShaderSource))
        qFatal("Compiling VIC Fragment Shader failed: %s", m_VITProgram.log().toStdString().c_str() );

    // Link shader pipeline
    if (!m_VICProgram.link())
        qFatal("Linking VIC Shader failed: %s", m_VITProgram.log().toStdString().c_str() );

    // Bind shader pipeline for use
    if (!m_VICProgram.bind())
        qFatal("Binding VIC Shader failed: %s", m_VITProgram.log().toStdString().c_str() );

}


