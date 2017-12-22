#ifndef TFX_COMPONENTS_SPATIAL_H
#define TFX_COMPONENTS_SPATIAL_H

#include "../core/Util.h"
#include "../math/Vector2.h"
#include "../math/Vector3.h"
#include "../logic/EC.h"

namespace tfx {
	class Spatial : public Component {
	public:
		Spatial();

		Vector3& getPosition() { return m_position; }
		void setPosition(const Vector3& pos) { m_position = pos; }

		Vector2& getScale() { return m_scale; }
		void setScale(const Vector2& scl) { m_scale = scl; }

		float& getRotation() { return m_rotation; }
		void setRotation(float scl) { m_rotation = scl; }

		Component* copy() override;

	private:
		Vector3 m_position;
		Vector2 m_scale;
		float m_rotation;
	};
}

#endif // TFX_COMPONENTS_SPATIAL_H