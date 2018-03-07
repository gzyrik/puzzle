#include <Precompiled_headers.h>
#include "FilterSprite.h"
bool FilterSprite::initWithTexture(Texture2D* pTexture, const Rect& tRect)
{
    do{
        CC_BREAK_IF(!Sprite::initWithTexture(pTexture, tRect));
        auto glprogram = GLProgramCache::getInstance()->getGLProgram("FilterSprite");
        if (!glprogram){
            const char* pszFragSource = 
            "#ifdef GL_ES \n \
            precision mediump float; \n \
            #endif \n \
            uniform sampler2D u_texture; \n \
            varying vec2 v_texCoord; \n \
            varying vec4 v_fragmentColor; \n \
            uniform mat4 fiterMat; \n \
            void main(void) \n \
            { \n \
                vec4 value = v_fragmentColor*texture2D(u_texture, v_texCoord); \n \
                gl_FragColor = fiterMat*value; \n \
            }";
            glprogram = GLProgram::createWithByteArrays(ccPositionTextureColor_vert, pszFragSource);
            GLProgramCache::getInstance()->addGLProgram(glprogram, "FilterSprite");
        }
        setGLProgramState(GLProgramState::getOrCreateWithGLProgram(glprogram));
        CHECK_GL_ERROR_DEBUG();
        return true;
    } while (0);
    return false;
}

void FilterSprite::draw(Renderer *renderer, const Mat4 &transform, uint32_t flags)
{
    _customCommand.init(_globalZOrder);
    _customCommand.func = CC_CALLBACK_0(FilterSprite::onDraw, this, transform, flags);
    renderer->addCommand(&_customCommand);
}

void FilterSprite::onDraw(const Mat4 &transform, uint32_t flags)
{
    auto glProgramState = getGLProgramState();
    glProgramState->setUniformMat4("fiterMat",_colorMatrix);
    glProgramState->apply(transform);

    GL::blendFunc( _blendFunc.src, _blendFunc.dst );

    GL::bindTexture2D( _texture->getName() );
    GL::enableVertexAttribs( GL::VERTEX_ATTRIB_FLAG_POS_COLOR_TEX );

#define kQuadSize sizeof(_quad.bl)
    size_t offset = (size_t)&_quad;

    // vertex
    int diff = offsetof( V3F_C4B_T2F, vertices);
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, kQuadSize, (void*) (offset + diff));

    // texCoods
    diff = offsetof( V3F_C4B_T2F, texCoords);
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_TEX_COORD, 2, GL_FLOAT, GL_FALSE, kQuadSize, (void*)(offset + diff));

    // color
    diff = offsetof( V3F_C4B_T2F, colors);
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, kQuadSize, (void*)(offset + diff));

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    CHECK_GL_ERROR_DEBUG();
    CC_INCREMENT_GL_DRAWN_BATCHES_AND_VERTICES(1,4);
}
