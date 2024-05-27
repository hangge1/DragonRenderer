#ifndef _RENDER_RENDERER_H
#define _RENDER_RENDERER_H_

#include <Windows.h>

#include "frame_buffer.h"
#include "pixel.h"
#include "image.h"

enum TextureUvWrap
{
    WrapRepeat,
    WrapMirror
};

class Renderer
{
public:
    Renderer() = default;
    ~Renderer();
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    void InitFrameBuffer(LONG frame_width, LONG frame_height, void* buffer = nullptr);

    void ClearFrameBuffer();

    void DrawPixel(LONG x_pox, LONG y_pox, Color& pixel_color);

    void DrawPixel(Pixel& point);

    void DrawLine(Pixel& start, Pixel& end);

    void DrawTriangle(Pixel& p1, Pixel& p2, Pixel& p3);

    void DrawPicture(const Image& image);

    void DrawPictureOnBlend(const Image& image, unsigned char src_alpha);

    inline void SetColorBlend(bool start_color_blend) { start_color_blend_ = start_color_blend; }

    inline void SetBilinearSample(bool start_bilinear_sample) { start_bilinear_sample_ = start_bilinear_sample; }

    inline void SetTextureUvWrap(TextureUvWrap texture_uv_wrap) { texture_uv_wrap_ = texture_uv_wrap; }

    void SetTexture(Image* texture) { texture_ = texture; }
private:
    Color BlendColor(LONG x, LONG y, const Color& src_color);

    Color NearestUvSample(glm::vec2& uv);
    Color BilinearUvSample(glm::vec2& uv);

    void CheckUv(float &ux, float &uy);
    float Fraction(float num);
private:
    FrameBuffer* current_frame_buffer_ { nullptr };
    bool start_color_blend_  { false };
    bool start_bilinear_sample_  { false };
    TextureUvWrap texture_uv_wrap_ { WrapRepeat };
    Image* texture_  { nullptr };
};


#endif