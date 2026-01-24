#pragma once
#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include <string>
#include <raylib.h>
#include <stddef.h>
#include <unordered_map>

// helper
struct PairHash {
    std::size_t operator()(const std::pair<std::string, std::string>& p) const {
        return std::hash<std::string>()(p.first) ^
               (std::hash<std::string>()(p.second) << 1);
    }
};

std::pair<std::string, std::string> make_key(const std::string& a, const std::string& b);

enum PARTICLE_TYPE {
	NONE,
	STATIC,
	SOLID,
	FLUID,
	GAS,
};

struct Particle {
	std::string parent;
    PARTICLE_TYPE type = NONE;
	Color clr;
    float density;
};
Particle* GenSolidParticle(std::string name, Color clr, float density);
Particle* GenFluidParticle(std::string name, Color clr, float density);
Particle* GenParticle(std::string name, PARTICLE_TYPE type, Color clr, float density);

class ParticleSystem {
private:
    Particle** particles;
    size_t width, height;
	Vector2 particleScale;
	Texture background;

    std::unordered_map<std::string, Particle*> particleTypes;    
    std::unordered_map<std::string, Color*> particleBuffers;     
    std::unordered_map<std::string, Texture2D> particleTextures; 
	std::unordered_map<std::string, Shader> particleShaders;
	std::unordered_map<std::pair<std::string, std::string>, std::string, PairHash> particleInteractions;
public:
    ParticleSystem(Rectangle screen, Vector2 scale);
	~ParticleSystem();

    void RegisterParticle(Particle* prototype);
    void InsertParticle(std::string typeName, Vector2 pos);
	void SetParticleInteraction(std::string particleOne, std::string particleTwo, std::string particleResult);
	void AddShaderToParticle(std::string typeName, std::string shaderFilePath);
	void SetBackground(Image img);

	//shader stuff
	void UpdateShaderF(std::string shaderPath, std::string uniformName, float value);
	void UpdateShaderI(std::string shaderPath, std::string uniformName, int value);
	void UpdateShaderV2(std::string shaderPath, std::string uniformName, Vector2 value);
	void UpdateShaderV3(std::string shaderPath, std::string uniformName, Vector3 value);
	void UpdateShaderV4(std::string shaderPath, std::string uniformName, Vector4 value);

    void Update();
    void UpdateColors();
    void UpdateTextures();
    void Render();
	Vector2 ScreenToCanvas(Vector2 mousePos);
};


#endif
