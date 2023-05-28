- ```cpp
    Texture2D ColorImage : register(t1);
    SamplerState LinearSampler : register(s1);

    struct UBO
    {
        float RadialBlurScale;
        float RadialBlueStrength;
        float2 RadialOrigin;
    };

    cbuffer Constants : register(b0)
    {
        UBO Constants;
    }

    float4 main([[vk::location(0)]] float2 InUV : TEXCOORD0) : SV_TARGET
    {
        int2 ImageDim;
        ColorImage.GetDimensions(ImageDim.x, ImageDim.y);
        float2 RadialSize = float2(1.0f / ImageDim.x, 1.0f / ImageDim.y);

        float2 UV = InUV;

        float4 Color = float4(0.0f, 0.0f, 0.0f, 0.0f);
        UV += RadialSize * 0.5f - Constants.RadialOrigin;

        #define SAMPLES 32

        for (int i = 0; i < SAMPLES; ++i)
        {
            float Scale = 1.0f - Constants.RadialBlueScale * (float(i) / float(SAMPLES - 1));
            Color += ColorImage.Sample(LinearSampler, UV * Scale + Constants.RadialOrigin);
        }

        return (Color / SAMPLES) * Constants.RadialBlurStrength;
    }
  ```