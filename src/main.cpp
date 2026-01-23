#include "raylib.h"
#include "particle_system.h"

#define WIDTH 800
#define HEIGHT 600
#define PIXEL_SIZE 15

Vector2 ScreenToCanvas(Vector2 mousePos, Vector2 particleScale);

int main()
{
    InitWindow(WIDTH, HEIGHT, "particle physics sim thing");
	SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor())/2.0f);
	

    ParticleSystem system({0,0,(float)WIDTH,(float)HEIGHT}, {PIXEL_SIZE, PIXEL_SIZE});
    system.RegisterParticle(GenSolidParticle("STONE", GRAY, 0.6f));
    system.RegisterParticle(GenSolidParticle("OBSIDIAN", BLACK, 0.9f));
    system.RegisterParticle(GenFluidParticle("WATER", BLUE, 0.1));
    system.RegisterParticle(GenFluidParticle("LAVA", RED, 0.4));
    system.RegisterParticle(GenSolidParticle("SAND", BEIGE, 0.2));
    system.RegisterParticle(GenSolidParticle("MUD", BROWN, 0.3f));
	system.SetParticleInteraction("SAND", "WATER", "MUD");
	system.SetParticleInteraction("WATER", "LAVA", "OBSIDIAN");

	system.AddShaderToParticle("SAND", "../src/noise.fs");
	system.AddShaderToParticle("STONE", "../src/noise.fs");
	system.AddShaderToParticle("MUD", "../src/noise.fs");
	system.SetBackground(GenImageColor(WIDTH/PIXEL_SIZE, HEIGHT/PIXEL_SIZE, DARKBLUE));

	while(!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(BLACK);

		if(IsMouseButtonDown(MOUSE_BUTTON_LEFT))
			system.InsertParticle("SAND", ScreenToCanvas(GetMousePosition(), (Vector2){PIXEL_SIZE, PIXEL_SIZE}));
		if(IsMouseButtonDown(MOUSE_BUTTON_RIGHT) && IsKeyDown(KEY_LEFT_SHIFT))
			system.InsertParticle("LAVA", ScreenToCanvas(GetMousePosition(), (Vector2){PIXEL_SIZE, PIXEL_SIZE}));
		else if(IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
			system.InsertParticle("WATER", ScreenToCanvas(GetMousePosition(), (Vector2){PIXEL_SIZE, PIXEL_SIZE}));
		if(IsMouseButtonDown(MOUSE_BUTTON_MIDDLE))
			system.InsertParticle("STONE", ScreenToCanvas(GetMousePosition(), (Vector2){PIXEL_SIZE, PIXEL_SIZE}));

		system.Update();
		system.Render();

		EndDrawing();
	}


    CloseWindow();
    return 0;
}

Vector2 ScreenToCanvas(Vector2 mousePos, Vector2 particleScale){
	Vector2 screen;
	screen.x = mousePos.x / particleScale.x;
	screen.y = mousePos.y / particleScale.y;
	return screen;
}

