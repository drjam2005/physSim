#include "particle_system.h"
#include <vector>
#include <bits/stdc++.h>
#include <algorithm>
#include <cstring>
#include "raylib.h"
#include <random>

static std::mt19937 rng{ std::random_device{}() };
Particle* GenSolidParticle(std::string name, Color clr, float density) {
    return new Particle{name, SOLID, clr, density};
}

Particle* GenFluidParticle(std::string name, Color clr, float density) {
    return new Particle{name, FLUID, clr, density};
}

Particle* GenParticle(std::string name, PARTICLE_TYPE type, Color clr, float density){
    return new Particle{name, type, clr, density};
}

ParticleSystem::ParticleSystem(Rectangle screen, Vector2 scale) {
    this->particleScale = scale;
    this->width  = (size_t)(screen.width  / scale.x);
    this->height = (size_t)(screen.height / scale.y);

    particles = new Particle*[width * height];
    for(size_t i = 0; i < width * height; ++i) 
		particles[i] = nullptr;

    particleTypes.clear();
    particleBuffers.clear();
    particleTextures.clear();
    particleShaders.clear();

    background.id = 0;
}

ParticleSystem::~ParticleSystem(){
	delete[] particles;
}

void ParticleSystem::RegisterParticle(Particle* prototype)
{
    const std::string& name = prototype->parent;

    particleTypes[name] = prototype;

    Color* buf = new Color[width * height];
    for(size_t i = 0; i < width * height; ++i) 
		buf[i] = BLANK;
    particleBuffers[name] = buf;

    Image temp = GenImageColor(width, height, BLANK);
    particleTextures[name] = LoadTextureFromImage(temp);
    UnloadImage(temp);
}

void ParticleSystem::AddShaderToParticle(std::string typeName, std::string shaderFilePath)
{
    Shader s = LoadShader(0, shaderFilePath.c_str());
    particleShaders[typeName] = s;

    int loc = GetShaderLocation(s, "u_pixelScale");
    if(loc >= 0) SetShaderValue(s, loc, &particleScale, SHADER_UNIFORM_VEC2);
}

void ParticleSystem::UpdateShaderF(std::string shaderPath, std::string uniformName, float value)
{
    auto it = particleShaders.find(shaderPath);
    if(it != particleShaders.end()){
        int loc = GetShaderLocation(it->second, uniformName.c_str());
        if(loc >= 0) SetShaderValue(it->second, loc, &value, SHADER_UNIFORM_FLOAT);
    }
}

void ParticleSystem::UpdateShaderI(std::string shaderPath, std::string uniformName, int value)
{
    auto it = particleShaders.find(shaderPath);
    if(it != particleShaders.end()){
        int loc = GetShaderLocation(it->second, uniformName.c_str());
        if(loc >= 0) SetShaderValue(it->second, loc, &value, SHADER_UNIFORM_INT);
    }
}

void ParticleSystem::UpdateShaderV2(std::string shaderPath, std::string uniformName, Vector2 value)
{
    auto it = particleShaders.find(shaderPath);
    if(it != particleShaders.end()){
        int loc = GetShaderLocation(it->second, uniformName.c_str());
        if(loc >= 0) SetShaderValue(it->second, loc, &value, SHADER_UNIFORM_VEC2);
    }
}

void ParticleSystem::UpdateShaderV3(std::string shaderPath, std::string uniformName, Vector3 value)
{
    auto it = particleShaders.find(shaderPath);
    if(it != particleShaders.end()){
        int loc = GetShaderLocation(it->second, uniformName.c_str());
        if(loc >= 0) SetShaderValue(it->second, loc, &value, SHADER_UNIFORM_VEC3);
    }
}

void ParticleSystem::UpdateShaderV4(std::string shaderPath, std::string uniformName, Vector4 value)
{
    auto it = particleShaders.find(shaderPath);
    if(it != particleShaders.end()){
        int loc = GetShaderLocation(it->second, uniformName.c_str());
        if(loc >= 0) SetShaderValue(it->second, loc, &value, SHADER_UNIFORM_VEC4);
    }
}

void ParticleSystem::SetParticleInteraction(std::string particleOne, std::string particleTwo, std::string particleResult){
	if(particleTypes.count(particleOne) && particleTypes.count(particleTwo) && particleTypes.count(particleResult))
		particleInteractions[make_key(particleOne,particleTwo)] = particleResult;
}

void ParticleSystem::InsertParticle(std::string typeName, Vector2 canvas)
{
    int x = (int)canvas.x;
    int y = (int)canvas.y;

    if(x < 0 || x >= (int)width || y < 0 || y >= (int)height) 
		return;

    int index = y * width + x;
    Particle* proto = particleTypes[typeName];
    Particle* newParticle = nullptr;

	newParticle = new Particle{proto->parent, proto->type, proto->clr, proto->density};
    particles[index] = newParticle;
}

void ParticleSystem::UpdateColors() {
    for(auto& [name, buf] : particleBuffers){
        for(size_t i = 0; i < width*height; ++i)
            buf[i] = BLANK;
	}

    for(size_t y=0; y<height; ++y){
        for(size_t x=0; x<width; ++x){
            int i = y*width + x;
            Particle* p = particles[i];
            if(!p) continue;
            particleBuffers[p->parent][i] = p->clr;
        }
    }
}

void ParticleSystem::UpdateTextures() {
    for(auto& [name, buf] : particleBuffers)
        UpdateTexture(particleTextures[name], buf);
}

void ParticleSystem::Render() {
    UpdateColors();
    UpdateTextures();

    if(background.id > 0)
            DrawTexturePro(
                background,
                Rectangle{0,0,(float)background.width,(float)background.height},
                Rectangle{0,0,(float)GetScreenWidth(),(float)GetScreenHeight()},
                Vector2{0,0}, 0.0f, WHITE
            );

    for(auto& [name, tex] : particleTextures){
        auto it = particleShaders.find(name);
        if(it != particleShaders.end()){
            BeginShaderMode(it->second);
            DrawTexturePro(
                tex,
                Rectangle{0,0,(float)tex.width,(float)tex.height},
                Rectangle{0,0,(float)GetScreenWidth(),(float)GetScreenHeight()},
                Vector2{0,0}, 0.0f, WHITE
            );
            EndShaderMode();
        }
        else{
            DrawTexturePro(
                tex,
                Rectangle{0,0,(float)tex.width,(float)tex.height},
                Rectangle{0,0,(float)GetScreenWidth(),(float)GetScreenHeight()},
                Vector2{0,0}, 0.0f, WHITE
            );
        }
    }
	int y = 0;
	for(auto k : particleInteractions){
		DrawText(TextFormat("%s + %s = %s", k.first.first.c_str(), k.first.second.c_str(), k.second.c_str()), 20, 20+(30*y), 20, BLACK);
		y++;
	}
}

Vector2 ParticleSystem::ScreenToCanvas(Vector2 mousePos){
    Vector2 canvas;
    canvas.x = mousePos.x / particleScale.x;
    canvas.y = mousePos.y / particleScale.y;
    return canvas;
}

void ParticleSystem::SetBackground(Image img)
{
    if(background.id > 0) UnloadTexture(background);
    background = LoadTextureFromImage(img);
    UnloadImage(img);
}


void ParticleSystem::Update() {
	std::vector<int> xs(width);
	std::iota(xs.begin(), xs.end(), 0);
	std::shuffle(xs.begin(), xs.end(), rng);

    for(int y = (int)height-2; y >= 0; --y){
        for(int xi = 0; xi < (int)width; ++xi){
			int x = xs[xi];
            int curr = y*width + x;
			int top = (y-1)*width + x;
            int bottom = (y+1)*width + x;
            int botLeft = (y+1)*width + x-1;
            int botRight = (y+1)*width + x+1;
            int left = y*width + x-1;
            int right = y*width + x+1;

            Particle* pCurr = particles[curr];

			if(!pCurr)
				continue;

			if(pCurr->type == SOLID){
				if(y < height-1){ // bottom
					if(!particles[bottom]){
							std::swap(particles[curr], particles[bottom]); 
						continue;
					}else if(particles[bottom]){
						if(particleInteractions.count(make_key(particles[curr]->parent, particles[bottom]->parent))){
							std::string result = particleInteractions[make_key(particles[curr]->parent, particles[bottom]->parent)];
							particles[bottom] = GenParticle(result, particleTypes[result]->type, particleTypes[result]->clr, particleTypes[result]->density);
							delete particles[curr];
							particles[curr] = nullptr;
							continue;
						}else{
							if(particles[bottom]->type == FLUID){
								if ((float)rand() / RAND_MAX < particles[curr]->density - particles[bottom]->density)
									std::swap(particles[curr], particles[bottom]);
							}
						}
					}
				}if(x < width-1 && y < height-1){
					if(!particles[botRight]){
						if ((float)rand() / RAND_MAX < (particles[curr]->density))
							std::swap(particles[curr], particles[botRight]); 
						continue;
					}else if(particles[botRight]){
						if(particleInteractions.count(make_key(particles[curr]->parent, particles[botRight]->parent))){
							std::string result = particleInteractions[make_key(particles[curr]->parent, particles[botRight]->parent)];
							particles[botRight] = GenParticle(result, particleTypes[result]->type, particleTypes[result]->clr, particleTypes[result]->density);
							delete particles[curr];
							particles[curr] = nullptr;
							continue;
						}else{
							if(particles[botRight]->type == FLUID){
								if ((float)rand() / RAND_MAX < particles[curr]->density - particles[botRight]->density)
									std::swap(particles[curr], particles[botRight]);
							}
						}
					}
				}if(x > 0){
					if(!particles[botLeft]){
						if ((float)rand() / RAND_MAX < (particles[curr]->density))
							std::swap(particles[curr], particles[botLeft]);
						continue;
					}else if(particles[botLeft]){
						if(particleInteractions.count(make_key(particles[curr]->parent, particles[botLeft]->parent))){
							std::string result = particleInteractions[make_key(particles[curr]->parent, particles[botLeft]->parent)];
							particles[botLeft] = GenParticle(result, particleTypes[result]->type, particleTypes[result]->clr, particleTypes[result]->density);
							delete particles[curr];
							particles[curr] = nullptr;
							continue;
						}else{
							if(particles[botLeft]->type == FLUID){
								if ((float)rand() / RAND_MAX < particles[curr]->density - particles[botLeft]->density)
									std::swap(particles[curr], particles[botLeft]);
							}
						}
					}
				}
			}else if(pCurr->type == FLUID){
				if(y < height-1){ // bottom
					if(!particles[bottom]){
						std::swap(particles[curr], particles[bottom]); continue;
					}else if(particles[bottom]){
						if(particleInteractions.count(make_key(particles[curr]->parent, particles[bottom]->parent))){
							std::string result = particleInteractions[make_key(particles[curr]->parent, particles[bottom]->parent)];
							particles[bottom] = GenParticle(result, particleTypes[result]->type, particleTypes[result]->clr, particleTypes[result]->density);
							delete particles[curr];
							particles[curr] = nullptr;
							continue;
						}else{
							if(particles[bottom]->type == FLUID){
								if ((float)rand() / RAND_MAX < particles[curr]->density - particles[bottom]->density)
									std::swap(particles[curr], particles[bottom]);
							}
						}
					}
				}if(x < width-1 && y < height-1){
					if(!particles[right]){
						std::swap(particles[curr], particles[right]); continue;
					}else if(particles[right]){
						if(particleInteractions.count(make_key(particles[curr]->parent, particles[right]->parent))){
							std::string result = particleInteractions[make_key(particles[curr]->parent, particles[right]->parent)];
							particles[right] = GenParticle(result, particleTypes[result]->type, particleTypes[result]->clr, particleTypes[result]->density);
							delete particles[curr];
							particles[curr] = nullptr;
							continue;
						}else{
							if(particles[right]->type == FLUID){
								if ((float)rand() / RAND_MAX < particles[curr]->density - particles[right]->density)
									std::swap(particles[curr], particles[right]);
							}
						}
					}
				}if(x > 0){
					if(!particles[left]){
						std::swap(particles[curr], particles[left]); continue;
					}else if(particles[left]){
						if(particleInteractions.count(make_key(particles[curr]->parent, particles[left]->parent))){
							std::string result = particleInteractions[make_key(particles[curr]->parent, particles[left]->parent)];
							particles[left] = GenParticle(result, particleTypes[result]->type, particleTypes[result]->clr, particleTypes[result]->density);
							delete particles[curr];
							particles[curr] = nullptr;
							continue;
						}else{
							if(particles[left]->type == FLUID){
								if ((float)rand() / RAND_MAX < particles[curr]->density - particles[left]->density)
									std::swap(particles[curr], particles[left]);
							}
						}
					}
				}
			}
        }
    }
}

std::pair<std::string, std::string> make_key(const std::string& a, const std::string& b) {
    if (a < b)
        return {a, b};
    else
        return {b, a};
}
