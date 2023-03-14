#include "Runtime/Core/Math/Matrix.h"

NAMESPACE_START(Gear)
NAMESPACE_START(Math)

#if defined(USE_SSE)
	VECTOR_MEMBER_FUNCTIONS_TRANSFORM(2)
	VECTOR_MEMBER_FUNCTIONS_TRANSFORM(3)
	
void Vector4::Transform(const Matrix& Trans)
{
	VECTOR_STORE(4, this, VECTOR_TRANSFORM(4, VECTOR_LOAD(4, this), MATRIX_LOAD(&Trans)));
}
#endif

NAMESPACE_END(Gear)
NAMESPACE_END(Math)