/*
    An example of using bootleg3d that draws some spinning cubes using SDL2.
    This can be used as a crude benchmark, as it will add more cubes to the
    scene until the framerate is around 60fps.
*/
#include <stdio.h>
#include "SDL.h"
#include "../ral.h"

#ifdef __SWITCH__
    #include <switch.h>
    #include <switch/services/hid.h>
    #define FILE_LOC "romfs:/"
#else
    #define FILE_LOC "../"
#endif

#define SCREEN_SIZE 2
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define SCREEN_FINALE_WIDTH (SCREEN_WIDTH * SCREEN_SIZE)
#define SCREEN_FINALE_HEIGHT (SCREEN_HEIGHT * SCREEN_SIZE)

#ifdef __SWITCH__
    #define WINDOW_WIDTH 1280
    #define WINDOW_HEIGHT 720
    const SDL_Rect ScreenSpace = {(WINDOW_WIDTH/2)-(SCREEN_FINALE_WIDTH/2), (WINDOW_HEIGHT/2)-(SCREEN_FINALE_HEIGHT/2), SCREEN_FINALE_WIDTH, SCREEN_FINALE_HEIGHT};
#else
    #define WINDOW_WIDTH (SCREEN_WIDTH * SCREEN_SIZE)
    #define WINDOW_HEIGHT (SCREEN_HEIGHT * SCREEN_SIZE)
    const SDL_Rect ScreenSpace = {0, 0, SCREEN_FINALE_WIDTH, SCREEN_FINALE_HEIGHT};
#endif

RAL_TEXTURE load_texture_from_tga(const char *filename) {
    FILE *fp = fopen(filename, "rb");

    uint8_t header[18];
    fread(header, 1, 18, fp);

    RAL_TEXTURE result;
    result.width = header[12] | (header[13] << 8);
    result.height = header[14] | (header[15] << 8);

    int length = result.width * result.height * sizeof(uint32_t);
    result.pixels = malloc(length);
    fread(result.pixels, 1, length, fp);
    fclose(fp);

    return result;
}

int main(int argument_count, char ** arguments) {

    #ifdef __SWITCH__
        // // Initialize the default gamepad (which reads handheld mode inputs as well as the first connected controller)
        // padInitializeDefault(&game->pad);
        // // Configure our supported input layout: a single player with standard controller styles
        // padConfigureInput(1, HidNpadStyleSet_NpadStandard);
        romfsInit();
    #endif

    // Set up SDL2 stuff, including whats needed to display a buffer of pixels.
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window * window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, 0 /*SDL_RENDERER_PRESENTVSYNC*/);
    SDL_Texture * texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
    RAL_CONTEXT context = {0};
    context.far = RAL_ONE * 20;
	  context.near = RAL_ONE / 10;

    uint32_t * pixels = malloc(SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(pixels[0]));
    RAL_F * depth = malloc(SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(depth[0]));
    uint8_t * objp = malloc(SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(objp[0]));
    RAL_TEXTURE tex = load_texture_from_tga(FILE_LOC "texture.tga");
    RAL_INIT(&context, pixels, depth, objp, SCREEN_WIDTH, SCREEN_HEIGHT, RAL_I2F(60));


    // For framerate counting.
    double freq = SDL_GetPerformanceFrequency();
    uint32_t next_update = 0;
    const int samples = 100;
    float average_fps[samples];
    int average_index = 0;
    int have_enough_samples = 0;

    int cube_count = 100;

    RAL_SET_CAMERA(&context, 0, 0, -2 * RAL_ONE, 0, 0, 0);

    while (1) {
        #ifdef __SWITCH__
            // Initialize the default gamepad (which reads handheld mode inputs as well as the first connected controller)
            // padInitializeDefault(&game->pad);
            // Configure our supported input layout: a single player with standard controller styles
            // padConfigureInput(1, HidNpadStyleSet_NpadStandard);
            romfsInit();
        #endif

        uint64_t time_stamp = SDL_GetPerformanceCounter();

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT ||
               (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)) {
                exit(0);
            }
        }

        // Current time in milliseconds.
        RAL_F t = SDL_GetTicks() * (RAL_ONE / 1000);

        RAL_CLEAR(&context);

        for (int i = 0; i < cube_count; ++i) {
            // Reset transformations back to the origin.
            RAL_RESET(&context);

            RAL_ROTZ(&context, t);
            RAL_ROTY(&context,t);
            RAL_ROTX(&context,t);

            RAL_ROTY(&context,i*(RAL_ONE/10));
            RAL_TRANSLATE(&context,RAL_ONE,RAL_ONE, (i*(RAL_ONE / 10)));
            RAL_ROTZ(&context,RAL_ONE * i + t);
            //For flat color test.
            /*RAL_TRIANGLE(&context,-RAL_HALF,-RAL_HALF,-RAL_HALF, -RAL_HALF, RAL_HALF,-RAL_HALF,  RAL_HALF, RAL_HALF,-RAL_HALF, 0xfcd0a1);
            RAL_TRIANGLE(&context,-RAL_HALF,-RAL_HALF,-RAL_HALF,  RAL_HALF, RAL_HALF,-RAL_HALF,  RAL_HALF,-RAL_HALF,-RAL_HALF, 0xb1b695);
            RAL_TRIANGLE(&context, RAL_HALF,-RAL_HALF,-RAL_HALF,  RAL_HALF, RAL_HALF,-RAL_HALF,  RAL_HALF, RAL_HALF, RAL_HALF, 0x53917e);
            RAL_TRIANGLE(&context, RAL_HALF,-RAL_HALF,-RAL_HALF,  RAL_HALF, RAL_HALF, RAL_HALF,  RAL_HALF,-RAL_HALF, RAL_HALF, 0x63535b);
            RAL_TRIANGLE(&context, RAL_HALF,-RAL_HALF, RAL_HALF,  RAL_HALF, RAL_HALF, RAL_HALF, -RAL_HALF, RAL_HALF, RAL_HALF, 0x6d1a36);
            RAL_TRIANGLE(&context, RAL_HALF,-RAL_HALF, RAL_HALF, -RAL_HALF, RAL_HALF, RAL_HALF, -RAL_HALF,-RAL_HALF, RAL_HALF, 0xd4e09b);
            RAL_TRIANGLE(&context,-RAL_HALF,-RAL_HALF, RAL_HALF, -RAL_HALF, RAL_HALF, RAL_HALF, -RAL_HALF, RAL_HALF,-RAL_HALF, 0xf6f4d2);
            RAL_TRIANGLE(&context,-RAL_HALF,-RAL_HALF, RAL_HALF, -RAL_HALF, RAL_HALF,-RAL_HALF, -RAL_HALF,-RAL_HALF,-RAL_HALF, 0xcbdfbd);
            RAL_TRIANGLE(&context,-RAL_HALF, RAL_HALF,-RAL_HALF, -RAL_HALF, RAL_HALF, RAL_HALF,  RAL_HALF, RAL_HALF, RAL_HALF, 0xf19c79);
            RAL_TRIANGLE(&context,-RAL_HALF, RAL_HALF,-RAL_HALF,  RAL_HALF, RAL_HALF, RAL_HALF,  RAL_HALF, RAL_HALF,-RAL_HALF, 0xa44a3f);
            RAL_TRIANGLE(&context, RAL_HALF,-RAL_HALF, RAL_HALF, -RAL_HALF,-RAL_HALF, RAL_HALF, -RAL_HALF,-RAL_HALF,-RAL_HALF, 0x5465ff);
            RAL_TRIANGLE(&context, RAL_HALF,-RAL_HALF, RAL_HALF, -RAL_HALF,-RAL_HALF,-RAL_HALF,  RAL_HALF,-RAL_HALF,-RAL_HALF, 0x788bff);
            */
            RAL_TRIANGLE_TEX(&context, -RAL_HALF,-RAL_HALF,-RAL_HALF, -RAL_HALF, RAL_HALF,-RAL_HALF,  RAL_HALF, RAL_HALF,-RAL_HALF, 0,0, 0,RAL_ONE, RAL_ONE,RAL_ONE, &tex);
            RAL_TRIANGLE_TEX(&context, -RAL_HALF,-RAL_HALF,-RAL_HALF,  RAL_HALF, RAL_HALF,-RAL_HALF,  RAL_HALF,-RAL_HALF,-RAL_HALF, 0,0, RAL_ONE,RAL_ONE, RAL_ONE,0, &tex);
            RAL_TRIANGLE_TEX(&context,  RAL_HALF,-RAL_HALF,-RAL_HALF,  RAL_HALF, RAL_HALF,-RAL_HALF,  RAL_HALF, RAL_HALF, RAL_HALF, 0,0, RAL_ONE,0, RAL_ONE,RAL_ONE, &tex);
            RAL_TRIANGLE_TEX(&context,  RAL_HALF,-RAL_HALF,-RAL_HALF,  RAL_HALF, RAL_HALF, RAL_HALF,  RAL_HALF,-RAL_HALF, RAL_HALF, 0,0, RAL_ONE,RAL_ONE, 0,RAL_ONE, &tex);
            RAL_TRIANGLE_TEX(&context,  RAL_HALF,-RAL_HALF, RAL_HALF,  RAL_HALF, RAL_HALF, RAL_HALF, -RAL_HALF, RAL_HALF, RAL_HALF, 0,0, RAL_ONE,0, RAL_ONE,RAL_ONE, &tex);
            RAL_TRIANGLE_TEX(&context,  RAL_HALF,-RAL_HALF, RAL_HALF, -RAL_HALF, RAL_HALF, RAL_HALF, -RAL_HALF,-RAL_HALF, RAL_HALF, 0,0, RAL_ONE,RAL_ONE, RAL_ONE,0, &tex);
            RAL_TRIANGLE_TEX(&context, -RAL_HALF,-RAL_HALF, RAL_HALF, -RAL_HALF, RAL_HALF, RAL_HALF, -RAL_HALF, RAL_HALF,-RAL_HALF, 0,RAL_ONE, RAL_ONE,RAL_ONE, RAL_ONE,0, &tex);
            RAL_TRIANGLE_TEX(&context, -RAL_HALF,-RAL_HALF, RAL_HALF, -RAL_HALF, RAL_HALF,-RAL_HALF, -RAL_HALF,-RAL_HALF,-RAL_HALF, 0,RAL_ONE, RAL_ONE,0, 0,0, &tex);
            RAL_TRIANGLE_TEX(&context, -RAL_HALF, RAL_HALF,-RAL_HALF, -RAL_HALF, RAL_HALF, RAL_HALF,  RAL_HALF, RAL_HALF, RAL_HALF, 0,0, 0,RAL_ONE, RAL_ONE,RAL_ONE, &tex);
            RAL_TRIANGLE_TEX(&context, -RAL_HALF, RAL_HALF,-RAL_HALF,  RAL_HALF, RAL_HALF, RAL_HALF,  RAL_HALF, RAL_HALF,-RAL_HALF, 0,0, RAL_ONE,RAL_ONE, RAL_ONE,0, &tex);
            RAL_TRIANGLE_TEX(&context,  RAL_HALF,-RAL_HALF, RAL_HALF, -RAL_HALF,-RAL_HALF, RAL_HALF, -RAL_HALF,-RAL_HALF,-RAL_HALF, 0,0, RAL_ONE,0, RAL_ONE,RAL_ONE, &tex);
            RAL_TRIANGLE_TEX(&context,  RAL_HALF,-RAL_HALF, RAL_HALF, -RAL_HALF,-RAL_HALF,-RAL_HALF,  RAL_HALF,-RAL_HALF,-RAL_HALF, 0,0, RAL_ONE,RAL_ONE, RAL_ONE,0, &tex);
        }

        // Display the pixel buffer on screen (using a streaming texture).
        SDL_RenderClear(renderer);
        SDL_UpdateTexture(texture, NULL, pixels, SCREEN_WIDTH * sizeof(uint32_t));
        SDL_RenderCopy(renderer, texture, NULL, &ScreenSpace);
        SDL_RenderPresent(renderer);

        // Display the average framerate in the window title.
        if (SDL_GetTicks() > next_update && have_enough_samples) {
            char title[64];
            float fps = 0;
            for (int i = 0; i < samples; ++i) fps += average_fps[i];
            fps /= samples;
            cube_count += (fps > 30) ? 50 : -50;
            snprintf(title, 32, "%d tris, %.1f fps", cube_count * 12, fps);
            SDL_SetWindowTitle(window, title);
            next_update = SDL_GetTicks() + 250;
        }
        average_fps[average_index++] = 1.0 / ((SDL_GetPerformanceCounter() - time_stamp) / freq);
        if (average_index > samples) {
            average_index = 0;
            have_enough_samples = 1;
        }
    }

    free(pixels);
    free(depth);
    free(objp);
}
