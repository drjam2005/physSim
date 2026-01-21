#include <raylib.h>
#include <cmath>
#include <unordered_map>
#include <particle_system.h>

#define WIDTH 800
#define HEIGHT 600

#define PIXEL_WIDTH 10
#define PIXEL_HEIGHT 10

#define GRID_WIDTH WIDTH/PIXEL_WIDTH
#define GRID_HEIGHT HEIGHT/PIXEL_HEIGHT

void UpdateBuffers(int width, int height, PARTICLE_TYPE* particles, std::unordered_map<PARTICLE_TYPE, ParticleInfo>& bufferMaps);
void UpdatePhysics(PARTICLE_TYPE* particles, PARTICLE_TYPE* buffer1, PARTICLE_TYPE* buffer);
Vector2 ScreenToCanvas(Vector2 mousePos);

int main(){
	InitWindow(WIDTH, HEIGHT, "PhysSim");
	SetTargetFPS(60);
	Image img = GenImageColor(GRID_WIDTH, GRID_HEIGHT, BLANK);
	Image noise = GenImagePerlinNoise(GRID_WIDTH, GRID_WIDTH, 0, 0, 500);
	Texture noiseT = LoadTextureFromImage(noise);
	Texture canvas = LoadTextureFromImage(img);
	UnloadImage(img);

	PARTICLE_TYPE particles[GRID_WIDTH*GRID_HEIGHT];
	PARTICLE_TYPE physBuffer1[GRID_WIDTH*GRID_HEIGHT];
	PARTICLE_TYPE physBuffer2[GRID_WIDTH*GRID_HEIGHT];
	for(int i = 0; i < GRID_WIDTH*GRID_HEIGHT; ++i){
		particles[i] = NONE;
		physBuffer1[i] = NONE;
		physBuffer1[i] = NONE;
	}

	Color waterBuffer[GRID_WIDTH*GRID_HEIGHT];
	Color staticBuffer[GRID_WIDTH*GRID_HEIGHT];
	Color sandBuffer[GRID_WIDTH*GRID_HEIGHT];

	Image blank = GenImageColor(GRID_WIDTH, GRID_HEIGHT, BLANK);
	Texture waterTex = LoadTextureFromImage(blank);
	Texture staticTex = LoadTextureFromImage(blank);
	Texture sandTex = LoadTextureFromImage(blank);

	std::unordered_map<PARTICLE_TYPE, ParticleInfo> bufferMaps;
	bufferMaps.emplace( FLUID, ParticleInfo{waterBuffer, &waterTex, BLUE});
	bufferMaps.emplace( SOLID, ParticleInfo{sandBuffer, &sandTex, YELLOW});
	bufferMaps.emplace( STATIC, ParticleInfo{staticBuffer, &staticTex, WHITE});
	UpdateBuffers((int)(GRID_WIDTH), (int)(GRID_HEIGHT), particles, bufferMaps);

	while(!WindowShouldClose()){
		BeginDrawing();
		ClearBackground(BLACK);
		
		if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
			Vector2 mouseCanvasPos = ScreenToCanvas(GetMousePosition());
			int bufferPos = ((int)mouseCanvasPos.y * GRID_WIDTH)+ (int)mouseCanvasPos.x;
			particles[bufferPos] = SOLID;
		}
		if(IsMouseButtonDown(MOUSE_BUTTON_RIGHT)){
			Vector2 mouseCanvasPos = ScreenToCanvas(GetMousePosition());
			int bufferPos = ((int)mouseCanvasPos.y * GRID_WIDTH)+ (int)mouseCanvasPos.x;
			particles[bufferPos] = FLUID;
		}
		if(IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)){
			Vector2 mouseCanvasPos = ScreenToCanvas(GetMousePosition());
			int bufferPos = ((int)mouseCanvasPos.y * GRID_WIDTH)+ (int)mouseCanvasPos.x;
			particles[bufferPos] = STATIC;
		}

		UpdateBuffers((int)(GRID_WIDTH), (int)(GRID_HEIGHT), particles, bufferMaps);
		UpdatePhysics(particles, physBuffer1, physBuffer2);
		for(auto& p : bufferMaps)
			DrawTexturePro(*p.second.tex, {0, 0, (float)(*p.second.tex).width, (float)(*p.second.tex).height}, {0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()}, {0,0}, 0.0f, WHITE);

		EndDrawing();
	}
	CloseWindow();
	return 0;
}

void UpdateBuffers(int width, int height, PARTICLE_TYPE* particles, std::unordered_map<PARTICLE_TYPE, ParticleInfo>& mapBuffers){
	for(int i = 0; i < width*height; ++i){
		for(auto& p : mapBuffers)
			p.second.buffer[i] = BLANK;
	}
	for(int y = 0; y < height; ++y){
		for(int x = 0; x < width; ++x){
			int curr = y*width+x;
			PARTICLE_TYPE partType = particles[curr];
			for (auto& p : mapBuffers) {
				if(p.first == partType){
					p.second.buffer[curr] = p.second.clr;
				}
			}
		}
	}

	for(auto& p : mapBuffers){
		UpdateTexture(*p.second.tex, p.second.buffer);
	}
}

void UpdatePhysics(PARTICLE_TYPE* particles, PARTICLE_TYPE* buffer1, PARTICLE_TYPE* buffer){
	for(int y = GRID_HEIGHT-1; y >= 0; --y){
		for(int x = 0; x < GRID_WIDTH; ++x){
			int curr     = (y)*GRID_WIDTH + x;
			int bottom   = (y+1)*GRID_WIDTH + x;
			int botLeft  = (y+1)*GRID_WIDTH + x-1;
			int botRight = (y+1)*GRID_WIDTH + x+1;
			int right    = (y)*GRID_WIDTH + x+1;
			int left     = (y)*GRID_WIDTH + x-1;

			if(particles[curr] == SOLID){
				if((particles[bottom] == NONE) && y < GRID_HEIGHT-1){
					std::swap(particles[bottom], particles[curr]);
				}else if((particles[bottom] == FLUID) && y < GRID_HEIGHT-1){
					if(particles[botRight] == NONE)
						std::swap(particles[bottom], particles[botRight]);
					else if(particles[botLeft] == NONE)
						std::swap(particles[bottom], particles[botLeft]);
					std::swap(particles[bottom], particles[curr]);
				}else if(particles[botRight] == NONE && y < GRID_HEIGHT-1){
					std::swap(particles[curr], particles[botRight]);
				}else if(particles[botLeft] == NONE && y < GRID_HEIGHT-1){
					std::swap(particles[curr], particles[botLeft]);
				}
			}else if(particles[curr] == FLUID){
				if((particles[bottom] == NONE) && y < GRID_HEIGHT-1){
					std::swap(particles[bottom], particles[curr]);
				}else if((particles[botRight] == NONE) && y < GRID_HEIGHT-1){
					std::swap(particles[botRight], particles[curr]);
				}else if((particles[botLeft] == NONE) && y < GRID_HEIGHT-1){
					std::swap(particles[botLeft], particles[curr]);
				}else if((particles[right] == NONE) && y < GRID_HEIGHT-1){
					std::swap(particles[right], particles[curr]);
				}else if((particles[left] == NONE) && y < GRID_HEIGHT-1){
					std::swap(particles[left], particles[curr]);
				}
			}
		}
	}
}

Vector2 ScreenToCanvas(Vector2 mousePos){
	Vector2 screen;
	screen.x = fmin((float)GRID_WIDTH-1, fmaxf(0.0f,(mousePos.x/PIXEL_WIDTH)));
	screen.y = fmin((float)GRID_HEIGHT-1, fmaxf(0.0f,(mousePos.y/PIXEL_HEIGHT)));
	return screen;
}
