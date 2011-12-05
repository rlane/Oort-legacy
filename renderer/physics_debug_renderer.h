// Copyright 2011 Rich Lane
#ifndef OORT_RENDERER_PHYSICS_DEBUG_RENDERER_H_
#define OORT_RENDERER_PHYSICS_DEBUG_RENDERER_H_

#include <memory>
#include <boost/scoped_ptr.hpp>

#include "glm/glm.hpp"
#include <Box2D/Box2D.h>

#include "gl/program.h"
#include "gl/buffer.h"

namespace Oort {

class PhysicsDebugRenderer : public b2Draw {
public:
	PhysicsDebugRenderer();

	void begin_render(float view_radius, float aspect_ratio, glm::vec2 view_center);
	void end_render();

	void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);
	void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);
	void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color);
	void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color);
	void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color);
	void DrawTransform(const b2Transform& xf);
	void DrawPoint(const b2Vec2& p, float32 size, const b2Color& color);
	void DrawString(int x, int y, const char* string, ...); 
	void DrawAABB(b2AABB* aabb, const b2Color& color);

private:
	boost::scoped_ptr<GL::Program> prog;
	float mRatio;
};

}

#endif

