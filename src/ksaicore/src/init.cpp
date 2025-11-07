#include <SDL3/SDL.h>
#include <ksaicore/init.h>
#include <ksaicore/memory.h>

static ksaicore_ctx ctx;
ksaicore_ctx *ksaicore_get_global_ctx() {
    return &ctx;
}

// I have pango cairo everything installed here so we can use it.
// I should be able to do this
// size_t circle1 = jkr_create_circle(, , , parameters)
// size_t rectangle1 = jkr_create_circle(, , , parameters)
// size_t buffer_id = begin_animations(); // This will start recording the command buffer
// animate(circle1, circle2);
// time_step(); // This will execute and play the animation (all the declarations before it will be
// played by this)
// animate(circle2, rectangle1);
// time_slow(); // animation after this will be slowed down
// animate(rectangle1, circle1);
// time_fast(); // animation after this will be faster
// end_animations(buffer_id);
// Note: ONLY time_step() says to actually move forward in time, all the animations BEFORE it and
// before another time_step() has to be executed
// .. Similarly a lot of animations will be here.
//
// All the properties and everything will be dictated by the objects like created above (all the
// state and resorces of the object should be internally handled)
// For each animation graph, like Object A -> Object B -> Object C, an image is to be made and all
// the position parameters and stuff like that that doesn't need image manipulation can be just done
// by moving rotating the image (use glm for that in SDL as well, mind that it should be done in
// this file only)
// Animation graph means, in the whole of command buffers, like there can be an object being another
// object, again that object being another object etc
// so for that chain-of-objects, one image should be there, so that we can transition properly from
// shapes, texts, images everything.
// Like text written in pango -> one object
// by animation it becomes circle -> Use image manipulation and nice animated transition like
// 3Blue1Brown's manim should be done from the whole text image to a circle
// To make things consistent, each of the element should have bounding box so to dictate the size of
// the image that we are making, but that too should be done like set_new_object_size(x, y, w, h);
// this way, there should be a default one.

void ksaicore_init() {
    SDL_SetMemoryFunctions(ksaicore_malloc, ksaicore_calloc, ksaicore_realloc, ksaicore_free);
    ctx.window = SDL_CreateWindow("ksaicore", 500, 500, SDL_WINDOW_RESIZABLE);

    int running = 1;
    SDL_Event ev;

    while (running) {
        while (SDL_PollEvent(&ev)) {
            switch (ev.type) {
            case SDL_EVENT_QUIT:
                running = 0;
                break;

            case SDL_EVENT_KEY_DOWN:
                if (ev.key.scancode == SDL_SCANCODE_ESCAPE) {
                    running = 0;
                }
                break;

            case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                running = 0;
                break;

            default:
                break;
            }
        }

        SDL_Delay(16);
    }
}
