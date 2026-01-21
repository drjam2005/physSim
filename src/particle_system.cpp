#include "particle_system.h"
#include <cstring>
#include "raylib.h"

SolidParticle* GenSolidParticle(std::string name, Color clr, float crumbleFactor) {
    return new SolidParticle(name, clr, crumbleFactor);
}

FluidParticle* GenFluidParticle(std::string name, Color clr, float density) {
    return new FluidParticle(name, clr, density);
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

void ParticleSystem::InsertParticle(const std::string& typeName, Vector2 canvas)
{
    int x = (int)canvas.x;
    int y = (int)canvas.y;

    if(x < 0 || x >= (int)width || y < 0 || y >= (int)height) 
		return;

    int index = y * width + x;
    Particle* proto = particleTypes[typeName];
    Particle* newParticle = nullptr;

    if(proto->type == SOLID){
        SolidParticle* s = static_cast<SolidParticle*>(proto);
        newParticle = new SolidParticle(s->parent, s->clr, s->crumbleFactor);
    }
    else if(proto->type == FLUID){
        FluidParticle* f = static_cast<FluidParticle*>(proto);
        newParticle = new FluidParticle(f->parent, f->clr, f->density);
    }

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

void ParticleSystem::Update()
{
    for(int y = (int)height-2; y >= 0; --y){
        for(int x = 0; x < (int)width; ++x){
            int curr = y*width + x;
            int bottom = (y+1)*width + x;
            int botLeft = (y+1)*width + x-1;
            int botRight = (y+1)*width + x+1;
            int left = y*width + x-1;
            int right = y*width + x+1;

            Particle* p = particles[curr];
            if(!p) 
				continue;

            if(p->type == SOLID){
                if(bottom < (int)width*height && (!particles[bottom] || particles[bottom]->type == FLUID))
                    std::swap(particles[curr], particles[bottom]);
                else if(x>0 && botLeft >= 0 && !particles[botLeft]) 
					std::swap(particles[curr], particles[botLeft]);
                else if(x+1<(int)width && botRight<(int)width*height && !particles[botRight])
					std::swap(particles[curr], particles[botRight]);
            }
            else if(p->type == FLUID){
                if(bottom < (int)width*height && !particles[bottom]) std::swap(particles[curr], particles[bottom]);
                else if(x>0 && botLeft >= 0 && !particles[botLeft])
					std::swap(particles[curr], particles[botLeft]);
                else if(x+1<(int)width && botRight<(int)width*height && !particles[botRight])
					std::swap(particles[curr], particles[botRight]);
                else if(x>0 && left>=0 && !particles[left] && rand()%2)
					std::swap(particles[curr], particles[left]);
                else if(x+1<(int)width && right<(int)width*height && !particles[right])
					std::swap(particles[curr], particles[right]);
            }
        }
    }
}

