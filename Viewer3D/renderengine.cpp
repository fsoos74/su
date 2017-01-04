#include "renderengine.h"

#include <QVector2D>
#include <QVector3D>
#include <GL/glu.h>

RenderEngine::RenderEngine()
{
    initializeOpenGLFunctions();
    initShaders();
}

RenderEngine::~RenderEngine()
{

    foreach ( VIC* vic, m_vics) {
       delete vic;
    }

    foreach ( VIT* vit, m_vits) {
       delete vit;
    }
}


void RenderEngine::clear(){

    foreach ( VIC* vic, m_vics) {
       delete vic;
    }

    m_vics.clear();

    foreach ( VIT* vit, m_vits) {
       delete vit;
    }

    m_vits.clear();
}

void RenderEngine::addVIT(VIT* vit){

    if( !vit ) return;

    m_vits.append(vit);
}


void RenderEngine::addVIC( VIC* vic){

    if( !vic ) return;

    m_vics.append(vic);
}

#include<iostream>
#include<iomanip>

void RenderEngine::draw(QMatrix4x4 matrix){

    m_textureProgram.bind();
    m_textureProgram.setUniformValue("mvp_matrix", matrix);
    m_textureProgram.setUniformValue("texture", 0);
    foreach(VIT* vit, m_vits){
        //std::cout<<"drawing vit "<<std::hex<<vit<<" #index="<<vit->indexCount()<<std::endl<<std::flush;
        draw( &m_textureProgram, vit);
    }

    m_colorProgram.bind();
    m_colorProgram.setUniformValue("mvp_matrix", matrix);
    foreach (VIC* vic, m_vics) {
        //std::cout<<"drawing vic "<<std::hex<<vic<<std::dec<<" #index="<<vic->indexCount()<<std::endl<<std::flush;
        draw( &m_colorProgram, vic);
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
    program->setAttributeBuffer(colorLocation, GL_FLOAT, offset, 3, sizeof(VIC::Vertex));


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
    "gl_FragColor = texture2D(texture, v_texcoord);\n"
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
    if (!m_textureProgram.addShaderFromSourceCode(QOpenGLShader::Vertex, textureVertexShaderSource))
        qFatal("Compiling VIT Vertex Shader failed: %s", m_textureProgram.log().toStdString().c_str() );

    // Compile fragment shader
    if (!m_textureProgram.addShaderFromSourceCode(QOpenGLShader::Fragment, textureFragmentShaderSource))
        qFatal("Compiling VIT Fragment Shader failed: %s", m_textureProgram.log().toStdString().c_str() );

    // Link shader pipeline
    if (!m_textureProgram.link())
        qFatal("Linking VIT Shader failed: %s", m_textureProgram.log().toStdString().c_str() );

    // Bind shader pipeline for use
    if (!m_textureProgram.bind())
        qFatal("Binding VIT Shader failed: %s", m_textureProgram.log().toStdString().c_str() );


    // Compile vertex shader
    if (!m_colorProgram.addShaderFromSourceCode(QOpenGLShader::Vertex, colorVertexShaderSource))
        qFatal("Compiling VIC Vertex Shader failed: %s", m_textureProgram.log().toStdString().c_str() );

    // Compile fragment shader
    if (!m_colorProgram.addShaderFromSourceCode(QOpenGLShader::Fragment, colorFragmentShaderSource))
        qFatal("Compiling VIC Fragment Shader failed: %s", m_textureProgram.log().toStdString().c_str() );

    // Link shader pipeline
    if (!m_colorProgram.link())
        qFatal("Linking VIC Shader failed: %s", m_textureProgram.log().toStdString().c_str() );

    // Bind shader pipeline for use
    if (!m_colorProgram.bind())
        qFatal("Binding VIC Shader failed: %s", m_textureProgram.log().toStdString().c_str() );

}


