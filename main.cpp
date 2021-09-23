#pragma warning(disable : 4996)

#include <irrlicht.h>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

#define AGG_BGRA32
#include <agg-src/include/agg_basics.h>
#include <agg-src/include/agg_rendering_buffer.h>
#include <agg-src/include/agg_rasterizer_scanline_aa.h>
#include <agg-src/include/agg_scanline_p.h>
#include <agg-src/include/agg_renderer_scanline.h>

// To save time picking a color
#define AGG_ARGB32
#include <agg-src/examples/pixel_formats.h>

#include "agg_scanline_p.h"
#include "agg_basics.h"
#include "agg_rendering_buffer.h"
#include "agg_rasterizer_scanline_aa.h"
#include "agg_renderer_scanline.h"
#include "agg_pixfmt_rgba.h"
#include "agg-src/examples/svg_viewer/agg_svg_parser.h"
#include "platform/agg_platform_support.h"
#include "ctrl/agg_slider_ctrl.h"

//
#include <irrlicht.h>

//
#include <string>

enum { flip_y = false };

typedef agg::row_accessor<irr::u32> rendering_buffer_u32;
typedef agg::pixfmt_alpha_blend_rgba<agg::blender_bgra32, rendering_buffer_u32> agg_pixel_type;

class the_application : public agg::platform_support
{
    agg::slider_ctrl<color_type> m_expand;
    agg::slider_ctrl<color_type> m_gamma;
    agg::slider_ctrl<color_type> m_scale;
    agg::slider_ctrl<color_type> m_rotate;

    double m_min_x;
    double m_min_y;
    double m_max_x;
    double m_max_y;

    double m_x;
    double m_y;
    double m_dx;
    double m_dy;
    bool   m_drag_flag;

public:
    agg::svg::path_renderer m_path;

    the_application(agg::pix_format_e format, bool flip_y) :
        agg::platform_support(format, flip_y),
        m_path(),
        m_expand(1, 1, 10, 10, !flip_y),
        m_gamma(0, 0, 0, 0, !flip_y),
        m_scale(0, 0, 0, 0, !flip_y),
        m_rotate(256 + 5, 5 + 15, 512 - 5, 11 + 15, !flip_y),
        m_min_x(0.0),
        m_min_y(0.0),
        m_max_x(0.0),
        m_max_y(0.0),
        m_x(0.0),
        m_y(0.0),
        m_dx(0.0),
        m_dy(0.0),
        m_drag_flag(false)
    {
        add_ctrl(m_expand);
        add_ctrl(m_gamma);
        add_ctrl(m_scale);
        add_ctrl(m_rotate);

        m_expand.label("Expand=%3.2f");
        m_expand.range(-1, 1.2);
        m_expand.value(0.0);

        m_gamma.label("Gamma=%3.2f");
        m_gamma.range(0.0, 3.0);
        m_gamma.value(1.0);

        m_scale.label("Scale=%3.2f");
        m_scale.range(0.2, 10.0);
        m_scale.value(1.0);

        m_rotate.label("Rotate=%3.2f");
        m_rotate.range(-180.0, 180.0);
        m_rotate.value(0.0);
    }

    void parse_svg(const char* fname)
    {
        agg::svg::parser p(m_path);
        p.parse(fname);
        m_path.bounding_rect(&m_min_x, &m_min_y, &m_max_x, &m_max_y);
    }

    ITexture* agg_svg_ITexture(IVideoDriver* driver, fschar_t* texture_name = (fschar_t*)"", double scale_value = 0.0, int stride_value = 4, double rotate_value = 0.0, double expand_value = 0.1, video::ECOLOR_FORMAT color_format = ECF_A8R8G8B8, u32 alpha_value = 0)
    {
        typedef agg::pixfmt_bgra32 pixfmt;
        typedef agg::renderer_base<pixfmt> renderer_base;
        typedef agg::renderer_scanline_aa_solid<renderer_base> renderer_solid;

        /// PROBLEM IS HERE? image_size cannot be anything other than around 2048,2048
        dimension2du image_size = driver->getScreenSize();
        IImage* image = driver->createImage(color_format, image_size);
        agg::rendering_buffer rbuf((agg::int8u*)image->lock(), image_size.Width, image_size.Height, image_size.Width * stride_value);

        pixfmt pixf(rbuf);
        renderer_base rb(pixf);
        renderer_solid ren(rb);

        rb.clear(agg::rgba(1, 1, 1, 0));

        agg::rasterizer_scanline_aa<> ras;
        agg::scanline_p8 sl;
        agg::trans_affine mtx;

        double scaleX = (double)driver->getScreenSize().Width / 600;
        double scaleY = (double)driver->getScreenSize().Height / 600;
        mtx *= agg::trans_affine_scaling(m_scale.value() * scaleX, m_scale.value() * scaleY);

        m_path.render(ras, sl, ren, mtx, rb.clip_box(), 1.0);

        agg::render_scanlines(ras, sl, ren);

        image->unlock();
        ITexture* texture = driver->addTexture(texture_name, image);
        image->drop();
        return texture;
    }

    ~the_application() {

    }

};

agg::svg::path_renderer* agg_svg_path(char* file_name)
{
    agg::svg::path_renderer* m_path = new agg::svg::path_renderer();
    agg::svg::parser p(*m_path);
    p.parse(file_name);
    return m_path;
}

IGUIImage* img = 0;
void createImageTest(the_application* app, IrrlichtDevice* device)
{
    if (img)
        img->remove();

    const char* fname = "tiger.svg";
    app->parse_svg(app->full_file_name(fname));

    ITexture* tex = app->agg_svg_ITexture(device->getVideoDriver(), (fschar_t*)fname);
    img = device->getGUIEnvironment()->addImage(tex, vector2di(0, 0));
    // img->setSourceRect(rect<s32>(0,0,tex->getOriginalSize().Width/4,tex->getOriginalSize().Height/4));
}

int main()
{
    irr::core::dimension2du screenSize(600, 600);
    irr::video::SColor white(0xffffffff);

    irr::IrrlichtDevice* device = irr::createDevice(irr::video::EDT_OPENGL, screenSize);

    if (!device) return 1;

    irr::video::IVideoDriver* videoDriver = device->getVideoDriver();

    the_application app(agg::pix_format_bgra32, flip_y);
    createImageTest(&app, device);

    while (device->run()) {
        videoDriver->beginScene(true, false, white);

        if (img && videoDriver->getScreenSize() != screenSize)
        {
            screenSize = videoDriver->getScreenSize();
            createImageTest(&app, device);
            img->setRelativePosition(irr::core::rect<int>(0, 0, screenSize.Width, screenSize.Height));
            printf("WINDOW RESIZE EVENT\n");
        }

        device->getGUIEnvironment()->drawAll();

        videoDriver->endScene();
    }

    device->drop();
    return 0;
}