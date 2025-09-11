#include "cEffect.h"

void eae6320::Graphics::cEffect::BindEffect() {
	{
		BindEffect_PlatformSpecific();
	}
	// Render state
	{
		m_renderState->Bind();
	}
}

eae6320::cResult eae6320::Graphics::cEffect::Initialize(std::string vertexShader, std::string fragmentShader, 
	uint8_t u_renderStateBits) {
	auto result = eae6320::Results::Success;

	if (!(result = eae6320::Graphics::cShader::Load(vertexShader,
		m_vertexShader, eae6320::Graphics::eShaderType::Vertex)))
	{
		EAE6320_ASSERTF(false, "Can't initialize shading data without vertex shader");
		return result;
	}
	if (!(result = eae6320::Graphics::cShader::Load(fragmentShader,
		m_fragmentShader, eae6320::Graphics::eShaderType::Fragment)))
	{
		EAE6320_ASSERTF(false, "Can't initialize shading data without fragment shader");
		return result;
	}
	{
		auto renderStateBits = [u_renderStateBits]
			{
				uint8_t renderStateBits = u_renderStateBits;

				eae6320::Graphics::RenderStates::DisableAlphaTransparency(renderStateBits);
				eae6320::Graphics::RenderStates::DisableDepthTesting(renderStateBits);
				eae6320::Graphics::RenderStates::DisableDepthWriting(renderStateBits);
				eae6320::Graphics::RenderStates::DisableDrawingBothTriangleSides(renderStateBits);

				return renderStateBits;
			}();
		if (!(result = m_renderState->Initialize(renderStateBits)))
		{
			EAE6320_ASSERTF(false, "Can't initialize shading data without render state");
			return result;
		}
	}
	result = Initialize_platformSpecificExtra();
	return result;
}

eae6320::cResult eae6320::Graphics::cEffect::CleanUp() {
	auto result = eae6320::Results::Success;

	result = CleanUp_platformSpecificExtra();
	if (m_vertexShader)
	{
		m_vertexShader->DecrementReferenceCount();
		m_vertexShader = nullptr;
	}
	if (m_fragmentShader)
	{
		m_fragmentShader->DecrementReferenceCount();
		m_fragmentShader = nullptr;
	}
	m_renderState = nullptr;
	return result;
}

eae6320::Graphics::cEffect::~cEffect()
{
	const auto result = CleanUp();
	EAE6320_ASSERT(result);
}