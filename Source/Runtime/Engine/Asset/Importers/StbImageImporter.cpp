#if 0

RHI::ImageDesc KTXImporter::Reimport(const uint8_t* Data, size_t Size)
{
	assert(Data && Size);

	ktxTexture* KTX = nullptr;
	VERIFY(KTX_SUCCESS == ktxTexture_CreateFromMemory(Data, Size, KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &KTX));

	RHI::ImageDesc Desc;
	Desc.Width = KTX->baseWidth;
	Desc.Height = KTX->baseHeight;
	Desc.Depth = KTX->baseDepth;
	Desc.MipLevels = KTX->numLevels;
	Desc.ArrayLayers = KTX->numLayers;
	switch (KTX->numDimensions)
	{
	case 1: Desc.Type = KTX->isArray ?
		RHI::EImageType::T_1D_Array : RHI::EImageType::T_1D;
		break;
	case 2: Desc.Type = KTX->isCubemap ?
		(KTX->isArray ? RHI::EImageType::T_Cube_Array : RHI::EImageType::T_Cube) :
		(KTX->isArray ? RHI::EImageType::T_2D_Array : RHI::EImageType::T_2D);
		break;
	case 3:
		Desc.Type = RHI::EImageType::T_3D;
		break;
	default:
		assert(0);
		break;
	}

	RHI::FormatAttribute FormatAttr;
	if (KTX->classId == ktxTexture1_c)
	{
		ktxTexture1* ktx1 = reinterpret_cast<ktxTexture1*>(KTX);
		FormatAttr = RHI::FormatAttribute::Attribute_GL(ktx1->glInternalformat);
	}
	else
	{
		ktxTexture2* ktx2 = reinterpret_cast<ktxTexture2*>(KTX);
		FormatAttr = RHI::FormatAttribute::Attribute_Vk(ktx2->vkFormat);
	}
	assert(FormatAttr.Format != RHI::EFormat::Unknown);

	Desc.Format = FormatAttr.Format;
	Desc.Subresources.resize(static_cast<size_t>(KTX->numLevels) * static_cast<size_t>(KTX->numLayers));

	uint32_t Index = 0u;
	for (uint32_t ArrayIndex = 0u; ArrayIndex < Desc.ArrayLayers; ++ArrayIndex)
	{
		uint32_t Width = Desc.Width;
		uint32_t Height = Desc.Height;
		uint32_t Depth = Desc.Depth;
		for (uint32_t MipIndex = 0u; MipIndex < Desc.MipLevels; ++MipIndex)
		{
			size_t Offset = 0u;
			VERIFY(ktxTexture_GetImageOffset(KTX, MipIndex, ArrayIndex, 0u, &Offset) == KTX_SUCCESS); /// Wrong param order...

			Desc.Subresources[Index].Width = Width;
			Desc.Subresources[Index].Height = Height;
			Desc.Subresources[Index].Depth = Depth;
			RHI::FormatAttribute::CalculateFormatBytes(Width, Height, Desc.Format, Desc.Subresources[Index].SliceBytes, Desc.Subresources[Index].RowBytes);

			Desc.Subresources[Index].Offset = static_cast<uint32_t>(Offset);
			assert(ktxTexture_GetImageSize(KTX, MipIndex) == Desc.Subresources[Index].SliceBytes);

			Width = Width >> 1;
			Height = Height >> 1;
			Depth = Depth >> 1;
			Width = Width == 0u ? 1u : Width;
			Height = Height == 0u ? 1u : Height;
			Depth = Depth == 0u ? 1u : Depth;

			++Index;
		}
	}

	auto Bytes = ktxTexture_GetDataSize(KTX);
	Desc.Asset.Data = ktxTexture_GetData(KTX);
	Desc.Asset.Size = static_cast<uint32_t>(Bytes);

	ktxTexture_Destroy(KTX);

	return Desc;
}
#endif