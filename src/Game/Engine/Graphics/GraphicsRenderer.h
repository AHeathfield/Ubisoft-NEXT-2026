#pragma once

#include "Mesh.h"
#include "Color.h"
#include "../Math/Vector2.h"
#include "../Math/Mat4x4.h"
#include "../Math/Triangle.h"
#include "../Camera/Camera.h"
#include <vector>

// This website helps understand the math going on https://www.codinglabs.net/article_world_view_projection_matrix.aspx
// This class will handle things like drawing lines, triangles, etc.
class GraphicsRenderer
{
public:
	struct RenderTriangle
	{
		Triangle triangle;
		bool isWireframe = false;
	};

	// These are implemented based on the platform, example DrawLine may come from SDL
	void DrawLine(const Vector2& p1, const Vector2& p2, Color color); // int because no such thing as float pixel

	// 3D Methods
	// Need to make these methods store the final triangles into smth and then draw all at the end, don't want to call multiple DrawTriangles/Lines
	void DrawTriangle(const RenderTriangle& tri);
	void DrawAll();
	void DrawMesh(const Mesh& mesh, const Mat4x4& matWorld);
	void ClearRenderer();

	void SetProjection(float fFovDegrees, float fAspectRatio, float fNear, float fFar);
	void SetCamera(Camera* camera);

private:
	Vector3 VectorIntersectPlane(Vector3& plane_p, Vector3& plane_n, Vector3& lineStart, Vector3& lineEnd);
	int ClipTriangleAgainstPlane(Vector3 plane_p, Vector3 plane_n, Triangle& in_tri, Triangle& out_tri1, Triangle& out_tri2);

private:

	std::vector<RenderTriangle> mTrisToRender;
	Mat4x4 mMatProj = Mat4x4::Identity();
	Mat4x4 mMatWorld = Mat4x4::Identity();

	Camera* mCamera = nullptr;
};
