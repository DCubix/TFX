#include "Spatial.h"

namespace tfx {
	Spatial::Spatial()
		:	m_position(Vector3(0.0f)),
			m_scale(Vector2(1.0f)),
			m_rotation(0.0f)
	{
	}

	Component* Spatial::copy() {
		Spatial* sp = new Spatial();
		sp->m_position = m_position;
		sp->m_rotation = m_rotation;
		sp->m_scale = m_scale;
		return sp;
	}
}
