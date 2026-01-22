Noita-like particle simulation
===
- quick project for study
- TODO:
	- Maybe add interaction modes:
	- example:
```cpp
system.RegisterParticle(GenSolidParticle("STONE", GRAY, 2.0f));
system.RegisterParticle(GenSolidParticle("SAND", YELLOW, 1.0f));
system.RegisterParticle(GenSolidParticle("GRAVEL", GRAY, 1.0f));

// maybe add this?
system.InteractionTypeToType(FLUID, SOLID, "SAND");
system.InteractionTypeToParticle(FLUID, "STONE", "GRAVEL");

```
