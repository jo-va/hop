#include "hop.h"
#include "lua/lua.h"
#include "lua/stack.h"
#include "lua/environment.h"
#include "util/log.h"

#include "loaders/obj.h"
#include "math/vec2.h"
#include "math/vec3.h"
#include "math/transform.h"
#include "geometry/world.h"
#include "geometry/shape_manager.h"
#include "camera/perspective_camera.h"
#include "render/renderer.h"
#include "options.h"

#include <sstream>
#include <memory>
#include <cstdio>

namespace hop { namespace lua {

static Environment* g_environment;

// This is here because GCC was optmizing away the destructor calls
// causing a segfault when during lua exit
template <typename T>
void NOOPTIMIZE destroy(T& obj)
{
    obj.~T();
}

static int load_obj(lua_State* L)
{
    Stack s(L);
    const char* file = s.get_string(1);
    ShapeID id = obj::load(file);
    s.push<int>(id);
    return 1;
}

static int vec3_ctor(lua_State* L)
{
    Stack s(L);
    Vec3r v(s.get_double(1), s.get_double(2), s.get_double(3));
    s.push_vec3(v);
    return 1;
}

static int vec3_tostring(lua_State* L)
{
    Stack s(L);
    Vec3r v = s.get_vec3(1);
    std::ostringstream oss;
    oss << v;
    s.push<const char*>(oss.str().c_str());
    return 1;
}

static int vec3_add(lua_State* L)
{
    Stack s(L);
    Vec3r v1 = s.get_vec3(1);
    Vec3r v2 = s.get_vec3(2);
    s.push_vec3(v1 + v2);
    return 1;
}

static int vec3_sub(lua_State* L)
{
    Stack s(L);
    Vec3r v1 = s.get_vec3(1);
    Vec3r v2 = s.get_vec3(2);
    s.push_vec3(v1 - v2);
    return 1;
}

static int vec3_mul(lua_State* L)
{
    Stack s(L);
    Vec3r v = s.get_vec3(1);
    double x = s.get_double(2);
    s.push_vec3(v * x);
    return 1;
}

static int vec3_length(lua_State* L)
{
    Stack s(L);
    Vec3r v = s.get_vec3(1);
    s.push<double>(v.length());
    return 1;
}

static int vec3_normalize(lua_State* L)
{
    Stack s(L);
    Vec3r v = s.get_vec3(1);
    s.push_vec3(v.normalize());
    return 1;
}

static int vec3_cross(lua_State* L)
{
    Stack s(L);
    Vec3r v1 = s.get_vec3(1);
    Vec3r v2 = s.get_vec3(2);
    s.push_vec3(cross(v1, v2));
    return 1;
}

static int transform_ctor(lua_State* L)
{
    Stack s(L);
    Transformr xfm;
    s.push_transform(xfm);
    return 1;
}

static int transform_tostring(lua_State* L)
{
    Stack s(L);
    Transformr xfm = s.get_transform(1);
    std::ostringstream oss;
    oss << '\n' << xfm.m;
    s.push<const char*>(oss.str().c_str());
    return 1;
}

static int transform_inverse(lua_State* L)
{
    Stack s(L);
    Transformr xfm = s.get_transform(1);
    Transformr inv(xfm.inv, xfm.m);
    s.push_transform(inv);
    return 1;
}

static int transform_mul(lua_State* L)
{
    Stack s(L);
    Transformr t1 = s.get_transform(1);
    Transformr t2 = s.get_transform(2);
    s.push_transform(t1 * t2);
    return 1;
}

static int make_translation_transform(lua_State* L)
{
    Stack s(L);
    Transformr xfm = make_translation(Vec3r(s.get_double(1), s.get_double(2), s.get_double(3)));
    s.push_transform(xfm);
    return 1;
}

static int make_lookat_transform(lua_State* L)
{
    Stack s(L);
    Vec3r eye = s.get_vec3(1);
    Vec3r target = s.get_vec3(2);
    Vec3r up = s.get_vec3(3);
    Transformr xfm = make_lookat(eye, target, up);
    s.push_transform(xfm);
    return 1;
}

static int world_ctor(lua_State* L)
{
    Stack s(L);
    auto world = std::make_shared<World>();
    s.push_world(world);
    return 1;
}

static int make_scale_transform(lua_State* L)
{
    Stack s(L);
    Transformr xfm = make_scale(Vec3r(s.get_double(1), s.get_double(2), s.get_double(3)));
    s.push_transform(xfm);
    return 1;
}

static int world_dtor(lua_State* L)
{
    Stack s(L);
    auto world = s.get_world(1);
    destroy(world);
    return 0;
}

static int world_add_shape(lua_State* L)
{
    Stack s(L);
    auto world = s.get_world(1);
    ShapeID shape = (ShapeID)s.get_int(2);
    world->add_shape(shape);
    return 0;
}

static int world_preprocess(lua_State* L)
{
    Stack s(L);
    auto world = s.get_world(1);
    world->preprocess();
    return 0;
}

static int make_instance(lua_State* L)
{
    Stack s(L);
    ShapeID id = (ShapeID)s.get_int(1);
    Transformr xfm = s.get_transform(2);
    ShapeID inst_id = ShapeManager::create<ShapeInstance>(id, xfm);
    s.push<int>(inst_id);
    return 1;
}

static int camera_make_perspective(lua_State* L)
{
    Stack s(L);
    luaL_checktype(L, 1, LUA_TTABLE);
    lua_getfield(L, 1, "transform");
    lua_getfield(L, 1, "frame_width");
    lua_getfield(L, 1, "frame_height");
    lua_getfield(L, 1, "fov");
    lua_getfield(L, 1, "lens_radius");
    lua_getfield(L, 1, "focal_distance");

    Transformr xfm = s.get_transform(-6);
    double w = s.get_double(-5);
    double h = s.get_double(-4);
    double fov = s.get_double(-3);
    double lensr = s.get_double(-2);
    double dist = s.get_double(-1);
    s.pop(6);

    std::shared_ptr<PerspectiveCamera> cam =
        std::make_shared<PerspectiveCamera>(xfm, Vec2u(w, h), fov, lensr, dist);
    s.push_camera(std::dynamic_pointer_cast<Camera>(cam));

    return 1;
}

static int camera_dtor(lua_State* L)
{
    Stack s(L);
    auto camera = s.get_camera(1);
    destroy(camera);
    return 0;
}

static int renderer_ctor(lua_State* L)
{
    Stack s(L);
    std::shared_ptr<World> world = s.get_world(1);
    std::shared_ptr<Camera> cam = s.get_camera(2);
    luaL_checktype(L, 3, LUA_TTABLE);
    lua_getfield(L, 3, "frame_width");
    lua_getfield(L, 3, "frame_height");
    lua_getfield(L, 3, "tile_width");
    lua_getfield(L, 3, "tile_height");
    lua_getfield(L, 3, "samples_per_pixel");

    int spp = s.get_int(-1);
    int th = s.get_int(-2);
    int tw = s.get_int(-3);
    int fh = s.get_int(-4);
    int fw = s.get_int(-5);
    s.pop(5);

    Options opts;
    opts.frame_size = Vec2u(fw, fh);
    opts.tile_size = Vec2u(tw, th);
    opts.tile_spp = spp;

    std::shared_ptr<Renderer> renderer = std::make_shared<Renderer>(world, cam, opts);
    renderer->set_lua_environment(g_environment);
    s.push_renderer(renderer);

    return 1;
}

static int renderer_dtor(lua_State* L)
{
    Stack s(L);
    auto renderer = s.get_renderer(1);
    destroy(renderer);
    return 0;
}

static int renderer_render_interactive(lua_State* L)
{
    Stack s(L);
    auto renderer = s.get_renderer(1);
    renderer->render(true);
    return 0;
}

static int renderer_set_camera(lua_State* L)
{
    Stack s(L);
    auto renderer = s.get_renderer(1);
    auto camera = s.get_camera(2);
    renderer->set_camera(camera);
    return 0;
}

static int renderer_reset(lua_State* L)
{
    Stack s(L);
    auto renderer = s.get_renderer(1);
    renderer->reset();
    return 0;
}

void load_api(Environment& env)
{
    g_environment = &env;

    env.register_function("load_obj", load_obj);

    const luaL_Reg vec3_funcs[] = {
        { "new",        vec3_ctor },
        { "__tostring", vec3_tostring },
        { "__add",      vec3_add },
        { "__sub",      vec3_sub },
        { "__mul",      vec3_mul },
        { "length",     vec3_length },
        { "normalize",  vec3_normalize },
        { "cross",      vec3_cross },
        { nullptr,      nullptr }
    };
    env.register_module("Vec3", vec3_funcs);

    const luaL_Reg transform_funcs[] = {
        { "new",        transform_ctor },
        { "ident",      transform_ctor },
        { "__tostring", transform_tostring },
        { "__mul",      transform_mul },
        { "inverse",    transform_inverse },
        { nullptr,      nullptr }
    };
    env.register_module("Transform", transform_funcs);

    env.register_function("make_scale", make_scale_transform);
    env.register_function("make_translation", make_translation_transform);
    env.register_function("make_lookat", make_lookat_transform);

    const luaL_Reg world_funcs[] = {
        { "new",        world_ctor },
        { "__gc",       world_dtor },
        { "add_shape",  world_add_shape },
        { "preprocess", world_preprocess },
        { nullptr,      nullptr }
    };
    env.register_module("World", world_funcs);

    env.register_function("make_instance", make_instance);

    const luaL_Reg camera_funcs[] = {
        { "make_perspective", camera_make_perspective },
        { "__gc",             camera_dtor },
        { nullptr,            nullptr }
    };
    env.register_module("Camera", camera_funcs);

    const luaL_Reg renderer_funcs[] = {
        { "new",                renderer_ctor },
        { "__gc",               renderer_dtor },
        { "render_interactive", renderer_render_interactive },
        { "set_camera",         renderer_set_camera },
        { "reset",              renderer_reset },
        { nullptr,              nullptr }
    };
    env.register_module("Renderer", renderer_funcs);
}

} } // namespace hop::lua
