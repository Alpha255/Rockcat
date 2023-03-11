#pragma once

#include "Colorful/IRenderer/IImage.h"
#include "Colorful/IRenderer/IBuffer.h"

NAMESPACE_START(RHI)

using FieldID = Handle<class Field, uint16_t>;
using RenderPassID = DirectedAcyclicGraph::NodeID;

class Field
{
public:
	enum class EVisibility
	{
		None = 0x0,
		Input = 0x1,
		Output = 0x2,
		Internal = 0x4
	};

	enum class EType
	{
		Image1D,
		Image2D,
		Image3D,
		ImageCube,
		Buffer
	};

	Field(const char8_t* Name, EVisibility Visibility, EType Type)
		: m_Name(Name)
		, m_Visibility(Visibility)
		, m_Type(Type)
		, m_Attributes({})
		, m_ID(FieldID::Alloc())
	{
	}

	Field& SetName(const char8_t* Name)
	{
		m_Name = Name;
		return *this;
	}

	const char8_t* Name() const
	{
		return m_Name.c_str();
	}

	EVisibility Visibility() const
	{
		return m_Visibility;
	}

	Field SetVisibility(EVisibility Visibility)
	{
		m_Visibility = Visibility;
	}

	EType Type() const
	{
		return m_Type;
	}

	FieldID ID() const
	{
		return m_ID;
	}

	const BufferDesc& GetBufferAttributes() const
	{
		assert(m_Type == EType::Buffer);
		return m_Attributes.BufferAttributes;
	}

	bool8_t operator==(const Field& Other) const
	{
		assert(m_Type == Other.m_Type);
		return m_ID == Other.m_ID;
	}

	bool8_t operator!=(const Field& Other) const
	{
		assert(m_Type == Other.m_Type);
		return m_ID != Other.m_ID;
	}

	static std::shared_ptr<Field> CreateBuffer(size_t Size);
	//static std::shared_ptr<Field> CreateRenderTargetView(const ImageAttributes& Attributes);
	//static std::shared_ptr<Field> CreateShaderResourceView(const ImageAttributes& Attributes);
	//static std::shared_ptr<Field> CreateDepthStencilView(const ImageAttributes& Attributes);
	//static std::shared_ptr<Field> CreateUnorderedAccessView(const ImageAttributes& Attributes);

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(m_ID),
			CEREAL_NVP(m_Visibility),
			CEREAL_NVP(m_Type),
			CEREAL_NVP(m_Attributes),
			CEREAL_NVP(m_Name)
		);
	}
protected:
private:
	FieldID m_ID;
	EVisibility m_Visibility;
	EType m_Type;
	union
	{
		//ImageAttributes ImagesAttributes;
		BufferDesc BufferAttributes;
	}m_Attributes;
	std::string m_Name;
};
ENUM_FLAG_OPERATORS(Field::EVisibility)

class IRenderPass : public DirectedAcyclicGraph::Node
{
public:
	IRenderPass(const char8_t* Name)
		: DirectedAcyclicGraph::Node(RenderPassID::Alloc())
		, m_Name(Name)
	{
	}

	void AddField(const char8_t* Name, Field::EVisibility Visiblity, Field::EType Type)
	{
		m_Fields.emplace_back(Field(Name, Visibility, Type));
	}

	const char8_t* Name() const
	{
		return m_Name.c_str();
	}

	RenderPassID ID() const
	{
		return Node::ID();
	}

	virtual void Execute() = 0;

	virtual void OnResize(uint32_t Width, uint32_t Height) { (void)Width; (void)m_Height; }

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(m_Name),
			CEREAL_NVP(m_Fields)
		);
	}
protected:
private:
	std::string m_Name;
	std::vector<Field> m_Fields;
};

NAMESPACE_END(RHI)
