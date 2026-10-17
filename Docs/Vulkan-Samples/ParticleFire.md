- ParticleFire (CPU based particle system)
  ```cpp
  #define PARTICLE_COUNT 512
  #define PARTICLE_SIZE 10.0f
  #define FLAME_RADIUS 8.0f
  #define PARTICLE_TYPE_FLAME 0
  #define PARTICLE_TYPE_SMOKE 1

  struct Particle
  {
      vec4 Pos;
      vec4 Color;
      vec4 Vel;
      float Alpha;
      float Size;
      float Rotation;
      float RotationSpeed;
      uint32_t Type;
  };

  vec3 EmitterPos = Vec3(0.0f, -FLAME_RADIUS + 2.0f, 0.0f);
  vec3 MinVel = vec3(-3.0f, 0.5f, -3.0f);
  vec3 MaxVel = vec3(3.0f, 7.0f, 3.0f);

  std::default_random_engine RandEngine;
  RandEngine.seed((unsigned))

  float Random(float Range)
  {
      std::uniform_real_distribution<float> RandDist(0.0f, Range);
      return RandDist()
  }
  ```
  - InitParticles
  ```cpp
  void InitParticle(Particle& Particle, vec3 EmitterPos)
  {
      Particle.Vel = vec4(0.0f, MinVel.y + Random(MaxVel.y - MinVel.y), 0.0f, 0.0f);
      //Particle.Alpha = Random(0.75f);
      Particle.Size = 1.0f + Random(0.5f);
      Particle.Color = vec4(1.0f);
      Particle.Type = PARTICLE_TYPE_FLAME;
      Particle.Rotation = Random(2.0f * PI);
      Particle.RotationSpeed = Random(2.0f) - Random(2.0f);

      float Theta = Random(2.0f * PI);
      float Phi = Random(PI) - PI / 2.0f;
      float R = Random(FLAME_RADIUS);

      Particle.Pos.x = R * cos(Theta) * cos(Phi);
      Particle.Pos.y = R * sin(Theta);
      Particle.Pos.z = R * sin(Theta) * cos(Phi);
      Particle.Pos += vec4(EmitterPos, 0.0f);
  }
  ```
  - PrepareParticles
  ```cpp
  ParticleBuffers.Resize(PARTICLE_COUNT);
  for (auto& Particle : ParticleBuffers)
  {
      InitParticle(Particle, EmitterPos);
      Particle.Alpha = 1.0f - (abs(Particle.Pos.y) / FLAME_RADIUS * 2.0f);
  }
  // Create Vertex Buffer
  ```
  - UpdateParticles
  ```cpp
  float ParticleTimer = FrameTimer * 0.45f;
  for (auto& Particle : ParticleBuffer)
  {
      switch (Particle.Type)
      {
          case PARTICLE_TYPE_FLAME:
            Particle.Pos.y -= Particle.Vel.y * ParticleTimer * 3.5f;
            Particle.Alpha += ParticleTimer * 2.5f;
            Particle.Size -= ParticleTimer * 0.5f;
            break;
          case PARTICLE_TYPE_SMOKE:
            Particle.Pos -= Particle.Vel * FrameTimer;
            Particle.Alpha += ParticleTimer * 0.25f;
            Particle.Size += ParticleTimer * 0.125f;
            Particle.Color -= ParticleTimer * 0.05f;
            break;
      }
      Particle.Rotation += ParticleTimer * Particle.RotationSpeed;
      if (Particle.Alpha > 2.0f)
      {
          {
              switch (Particle.Type)
              {
                  case PARTICLE_TYPE_FLAME:
                    if (Random(1.0f) < 0.05f)
                    {
                        Particle.Alpha = 0.0f;
                        Particle.Color = vec4(0.25f + Random(0.25f));
                        Particle.Pos.x *= 0.5f;
                        Particle.Pos.z *= 0.5f;
                        Particle.Vel = vec4(Random(1.0f) - Random(1.0f), (MinVel.y * 2) + Random(MaxVel.y - MinVel.y), Random(1.0f) - Random(1.0f), 0.0f);
                        Particle.Size = 1.0f + Random(0.5f);
                        Particle.RotationSpeed = Random(1.0f) - Random(1.0f);
                        Particle.Type = PARTICLE_TYPE_SMOKE;
                    }
                    else
                    {
                        InitParticles(Particle, EmitterPos);
                    }
                    break;
                  case PARTICLE_TYPE_SMOKE:
                    InitParticles(Particle, EmitterPos);
                    break;
              }
          }
      }
  }
  ```
  - DrawParticles
    - vkCmdDraw(DrawCmdBuffer, PARTICLE_COUNT, 1, 0, 0);
    - VertexShader
    ```cpp
    struct VSOutput
    {
        float4 Pos : SV_POSITION;
        [[vk::builtin("PointSize")]] float PSize : PSIZE;
        [[vk::location(0)]] float4 Color : COLOR0;
        [[vk::location(1)]] float Alpha : TEXCOORD0;
        [[vk::location(2)]] int Type : TEXCOORD1;
        [[vk::location(3)]] float Rotation : TEXCOORD2;
        [[vk::location(4)]] float2 CenterPos : POSITION1;
        [[vk::location(5)]] float PointSize : TEXCOORD3;
    };

    struct UBO
    {
        float4x4 Projection;
        float4x4 ModelView;
        float2 ViewportDim;
        float PointSize;
    };
    cbuffer Ubo : register(b0)  
    {
        UBO Ubo;
    };

    VSOutput main(VSInput Input)
    {
        /// ...
        Output.Pos = mul(Ubo.Projection, mul(Ubo.ModelView, float4(Input.Pos.xyz, 1.0f)));

        float SpriteSize = Input.Size * 8.0f;
        float4 EyePos = mul(Ubo.ModelView, float4(Input.Pos.xyz, 1.0f));
        float4 ProjectedCorner = mul(Ubo.Projection, float4(0.5f * SpriteSize, 0.5f * SpriteSize, EyePos.z, EyePos.w));
        Output.PointSize = Output.PSize = Ubo.ViewportDim.x * ProjectedCorner.x / ProjectedCorner.w;
        Output.CenterPos = ((Output.Pos.xy / Output.Pos.w) + 1.0f) * 0.5f * Ubo.ViewportDim;
        return Output;
    }
    ```
    - PixelShader
    ```cpp
    float4 main(VSOutput Input) : SV_TARGET
    {
        float4 Color;
        float Alpha = Input.Alpha <= 1.0f ? Input.Alpha : (2.0f - Input.Alpha);

        float RotCenter = 0.5f;
        float RotCos = cos(Input.Rotation);
        float RotSin = sin(Input.Rotation);

        float2 PointCoord = (Input.Pos.xy - Input.CenterPos.xy) / Input.PointSize + 0.5f;

        float2 RotUV = float2(
          RotCos * (PointCoord.x - RotCenter) + RotSin * (PointCoord.x - RotCenter) + RotCenter,
          RotCos * (PointCoord.y - RotCenter) - RotSin * (PointCoord.y - RotCenter) + RotCenter);

        float4 OutColor;
        if (Input.Type == 0)
        {
            Color = TextureFire.Sample(SamplerFire, RotUV);
            OutColor.a = 0.0f;
        } 
        else
        {
            Color = TextureSmoke.Sample(SamplerSmoke, RotUV);
            OutColor.a = Color.a * Alpha;
        }

        OutColor.rgb = Color.rgb * Input.Color.rgb * Alpha;
        return OutColor;
    }
    ```