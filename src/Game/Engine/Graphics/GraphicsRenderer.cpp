#include "GraphicsRenderer.h"
#include "../Core/Utility.h"
#include "../../../ContestAPI/app.h"
#include <list>
#include <iostream>
#include <string>


void GraphicsRenderer::DrawLine(const Vector2& p1, const Vector2& p2, Color color)
{
    // Using how Ubisoft API draws lines
    App::DrawLine(p1.x, p1.y, p2.x, p2.y, color.r, color.g, color.b);
}


// Usually I would implement this using DrawLine and own rasterizing methods but the API provides a DrawTriangle
// If it gets weird glitches use my bresenham solution
// NEW: API HAS 3D TRIANGLES WITH DEPTH BUFFERING!!!!! WOOHOOOO!!
void GraphicsRenderer::DrawTriangle(const RenderTriangle& tri)
{
    // utility::log("Drawing Triangle: " + tri.PrintPoints());

    // Having the w values messes it up so I just set them to 1
    App::DrawTriangle(tri.triangle.points[0].x, tri.triangle.points[0].y, tri.triangle.points[0].z, 1, tri.triangle.points[1].x, tri.triangle.points[1].y, tri.triangle.points[1].z, 1, tri.triangle.points[2].x, tri.triangle.points[2].y, tri.triangle.points[2].z, 1, tri.triangle.color[0].GetPercentageRed(), tri.triangle.color[0].GetPercentageGreen(), tri.triangle.color[0].GetPercentageBlue(), tri.triangle.color[1].GetPercentageRed(), tri.triangle.color[1].GetPercentageGreen(), tri.triangle.color[1].GetPercentageBlue(), tri.triangle.color[2].GetPercentageRed(), tri.triangle.color[2].GetPercentageGreen(), tri.triangle.color[2].GetPercentageBlue(), tri.isWireframe);
}


// NOTE: Previously used painter algoirthm but API has depth buffering
void GraphicsRenderer::DrawAll()
{
    // Rendering the triangles
    for (int i = 0; i < mTrisToRender.size(); i++)
    {
        DrawTriangle(mTrisToRender[i]);
    }
}


void GraphicsRenderer::DrawMesh(const Mesh& mesh, const Mat4x4& matWorld)
{
    if (mCamera != nullptr)
    {
        // ============= Set up "World Transform"
        // NOW Handled by the Transform component

        // Setting up camera transforms
        Vector3 vTarget = mCamera->position + mCamera->GetLookDirection();
        Mat4x4 matCamera;
        matCamera.SetPointAt(mCamera->position, vTarget);

        // Make view matrix from camera (which is the transformation from World Space to View Space
        // This basically gets the World position relative to the camera
        Mat4x4 matView = matCamera.GetQuickInverse();   // NOTE CAMERA IN World Space

        std::vector<Triangle> vecTrianglesToRaster;

        // Draw Triangles
        for (auto tri : mesh.triangles)
        {
            // First need to create the projected triangle (all 3 points)
            Triangle triProjected;
            Triangle triTransformed;
            Triangle triViewed;
            triTransformed.points[0] = matWorld.GetMultiplyVector3(tri.points[0]);
            triTransformed.points[1] = matWorld.GetMultiplyVector3(tri.points[1]);
            triTransformed.points[2] = matWorld.GetMultiplyVector3(tri.points[2]);

            // Getting the Normal Vector
            Vector3 normal, line1, line2;

            // Get lines either side of triangle
            line1 = triTransformed.points[1] - triTransformed.points[0];
            line2 = triTransformed.points[2] - triTransformed.points[0];

            // Take cross product of lines to get normal to triangle surface
            normal = line1.GetCrossProduct(line2);
            normal.Normalize();

            // Get Ray from triangle to camera
            Vector3 vCameraRay = triTransformed.points[0] - mCamera->position;

            // We get the dot product to see how similar the normal vector is
            // to the projected triangle based on the camera pos
            if (normal.GetDotProduct(vCameraRay) < 0.0f) // If normal faces away this = false since -z is towards the cam
            {
                // NEW: Can add better lighting with the color vertex in API (not sure if it makes a diff)
                Vector3 light_pos = {5.f, -50.f, -50.f};
                for (int i = 0; i < 3; i++)
                {
                    Vector3 light_dir = (light_pos - triProjected.points[i]).GetNormal();
                    // How "aligned" are light direction and triangle surface normal?
                    float dp = std::max(0.1f, light_dir.GetDotProduct(normal));
                    Color color = mesh.color;
                    color.SetColorScale(dp);
                    triProjected.color[i] = color;
                }


                // Convert World Space --> View Space
                triViewed.points[0] = matView.GetMultiplyVector3(triTransformed.points[0]);
                triViewed.points[1] = matView.GetMultiplyVector3(triTransformed.points[1]);
                triViewed.points[2] = matView.GetMultiplyVector3(triTransformed.points[2]);

                // Clip Viewed Triangle against near plane, this could form 2 additional triangles
                int nClippedTriangles = 0;
                Triangle clipped[2];
                // First Vector is a point on the near plane just in front of the camera on z axis, and the normal is straight along z-axis
                nClippedTriangles = ClipTriangleAgainstPlane({ 0.0f, 0.0f, 0.1f }, { 0.0f, 0.0f, 1.0f }, triViewed, clipped[0], clipped[1]);

                for (int n = 0; n < nClippedTriangles; n++)
                {
                    // TODO: add the color set here (where the comment is)
                    // Project triangles from 3D --> 2D
                    triProjected.points[0] = mMatProj.GetMultiplyVector3(clipped[n].points[0]);
                    triProjected.points[1] = mMatProj.GetMultiplyVector3(clipped[n].points[1]);
                    triProjected.points[2] = mMatProj.GetMultiplyVector3(clipped[n].points[2]);
                    // triProjected.color = clipped[n].color;

                    // Scale into view, we need to normailze the vector matrix
                    triProjected.points[0] /= triProjected.points[0].w;
                    triProjected.points[1] /= triProjected.points[1].w;
                    triProjected.points[2] /= triProjected.points[2].w;

                    // X/Y are inverted so put them back
                    triProjected.points[0].x *= -1.0f;
                    triProjected.points[1].x *= -1.0f;
                    triProjected.points[2].x *= -1.0f;
                    triProjected.points[0].y *= -1.0f;
                    triProjected.points[1].y *= -1.0f;
                    triProjected.points[2].y *= -1.0f;

                    // Scale into view (since its between -1 and 1 currently)
                    Vector3 vOffsetView = { 1, 1, 0 };
                    triProjected.points[0] += vOffsetView;
                    triProjected.points[1] += vOffsetView;
                    triProjected.points[2] += vOffsetView;
                    triProjected.points[0].x *= 0.5f * APP_VIRTUAL_WIDTH;
                    triProjected.points[0].y *= 0.5f * APP_VIRTUAL_HEIGHT;
                    triProjected.points[1].x *= 0.5f * APP_VIRTUAL_WIDTH;
                    triProjected.points[1].y *= 0.5f * APP_VIRTUAL_HEIGHT;
                    triProjected.points[2].x *= 0.5f * APP_VIRTUAL_WIDTH;
                    triProjected.points[2].y *= 0.5f * APP_VIRTUAL_HEIGHT;

                    // Store trianlge for sorting
                    vecTrianglesToRaster.push_back(triProjected);
                }
            }
        }

       
        // Credit to javidx9 youtube series for this part
        for (auto& triToRaster : vecTrianglesToRaster)
        {
            // Clip triangles against all 4 screen edges, this could yield
            // a bunch of triangles
            Triangle clipped[2];
            std::list<RenderTriangle> listTriangles;

            // Add initial triangle
            listTriangles.push_back(RenderTriangle{triToRaster, mesh.isWireframe});
            int nNewTriangles = 1;

            for (int p = 0; p < 4; p++)
            {
                int nTrisToAdd = 0;
                while (nNewTriangles > 0)
                {
                    // Take triangle from front of queue
                    Triangle test = listTriangles.front().triangle;
                    listTriangles.pop_front();
                    nNewTriangles--;

                    // Clip it against a plane. We only need to test each subsequent plane,
                    // against subsequent new triangles as all triangles after a plane
                    // clip are guaranteed to lie on the inside of the plane.
                    switch (p)
                    {
                    case 0:
                        nTrisToAdd = ClipTriangleAgainstPlane({ 0.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }, test, clipped[0], clipped[1]);
                        break;
                    case 1:
                        nTrisToAdd = ClipTriangleAgainstPlane({ 0.f, APP_VIRTUAL_HEIGHT, 0.f }, { 0.f, -1.f, 0.f }, test, clipped[0], clipped[1]);
                        break;
                    case 2:
                        nTrisToAdd = ClipTriangleAgainstPlane({ 0.f, 0.f, 0.f }, { 1.f, 0.f, 0.f }, test, clipped[0], clipped[1]);
                        break;
                    case 3:
                        nTrisToAdd = ClipTriangleAgainstPlane({ APP_VIRTUAL_WIDTH, 0.f, 0.f }, { -1.f, 0.f, 0.f }, test, clipped[0], clipped[1]);
                        break;
                    }

                    // Clipping may yield a variable number of triangles, so add these new ones
                    // to the back of the queue for subsequent clipping against next planes
                    for (int w = 0; w < nTrisToAdd; w++)
                    {
                        listTriangles.push_back(RenderTriangle{clipped[w], mesh.isWireframe});
                    }
                }
                nNewTriangles = listTriangles.size();
            }

            // Adding the list of triangles to be rendered
            mTrisToRender.insert(mTrisToRender.end(), listTriangles.begin(), listTriangles.end());
        }
    }
}

void GraphicsRenderer::ClearRenderer()
{
    mTrisToRender.clear();
}


void GraphicsRenderer::SetProjection(float fFovDegrees, float fAspectRatio, float fNear, float fFar)
{
    float fFovRad = 1.f / std::tanf(fFovDegrees * 0.5f / 180.f * 3.14159f);
    mMatProj.m[0][0] = fAspectRatio * fFovRad;
    mMatProj.m[1][1] = fFovRad;
    mMatProj.m[2][2] = fFar / (fFar - fNear);
    mMatProj.m[3][2] = (-fFar * fNear) / (fFar - fNear);
    mMatProj.m[2][3] = 1.0f;
    mMatProj.m[3][3] = 0.0f;
}


void GraphicsRenderer::SetCamera(Camera* camera)
{
    mCamera = camera;
}


// Private methods, credit to javidx9 tutorial series on youtube on 3D Graphics
Vector3 GraphicsRenderer::VectorIntersectPlane(Vector3& plane_p, Vector3& plane_n, Vector3& lineStart, Vector3& lineEnd)
{
    plane_n.Normalize();
    float plane_d = -1 * plane_n.GetDotProduct(plane_p);
    float ad = lineStart.GetDotProduct(plane_n);
    float bd = lineEnd.GetDotProduct(plane_n);
    float t = (-1 * plane_d - ad) / (bd - ad);
    Vector3 lineStartToEnd = lineEnd - lineStart;
    Vector3 lineToIntersect = lineStartToEnd * t;
    return (lineStart + lineToIntersect);
}


// Should probably add an ifdef for debug mode or smth
int GraphicsRenderer::ClipTriangleAgainstPlane(Vector3 plane_p, Vector3 plane_n, Triangle& in_tri, Triangle& out_tri1, Triangle& out_tri2)
{
    // Make sure plane normal is indeed normal
    plane_n.Normalize();

    // This is an "auto function"
    // Return signed shortest distance from point to plane, plane normal must be normalized
    auto dist = [&](Vector3& p)
        {
            // Vector3 n = p.GetNormal();
            // p.Normalize();
            return (plane_n.x * p.x + plane_n.y * p.y + plane_n.z * p.z - plane_n.GetDotProduct(plane_p));
        };

    // Create 2 temporary storage arrays to classify points either side of plane
    // If distance sign is positive, point lies on "inside" of plane
    Vector3* inside_points[3];  int nInsidePointCount = 0;
    Vector3* outside_points[3]; int nOutsidePointCount = 0;

    // Get signed distance of each point in triangle to plane
    float d0 = dist(in_tri.points[0]);
    float d1 = dist(in_tri.points[1]);
    float d2 = dist(in_tri.points[2]);

    if (d0 >= 0)
    {
        inside_points[nInsidePointCount++] = &in_tri.points[0];
    }
    else
    {
        outside_points[nOutsidePointCount++] = &in_tri.points[0];
    }
    if (d1 >= 0)
    {
        inside_points[nInsidePointCount++] = &in_tri.points[1];
    }
    else
    {
        outside_points[nOutsidePointCount++] = &in_tri.points[1];
    }
    if (d2 >= 0)
    {
        inside_points[nInsidePointCount++] = &in_tri.points[2];
    }
    else
    {
        outside_points[nOutsidePointCount++] = &in_tri.points[2];
    }

    // Now classify triangle points, and break the input triangle into
    // smaller output triangles if required. There are four possible
    // outcomes...
    if (nInsidePointCount == 0)
    {
        // All points lie on the outside of plane, so clip whole triangle
        // It ceases to exist
        return 0; // No returned triangles are valid
    }
    if (nInsidePointCount == 3)
    {
        // All points lie on the inside of plane, so do nothing
        // and allow the triangle to simply pass through
        out_tri1 = in_tri;
        out_tri1.SetWindingCCW();

        return 1;
    }
    if (nInsidePointCount == 1 && nOutsidePointCount == 2)
    {
        // Triangle should be clipped. As two points lie outside
        // the plane, the triangle simply becomes a smaller triangle

        // Copy appearance info to new triangle
        out_tri1.color[0] = in_tri.color[0];
        out_tri1.color[1] = in_tri.color[1];
        out_tri1.color[2] = in_tri.color[2];
        // out_tri1.color = Color{0, 0, 255}; // Blue for testing

        // The inside point is valid, so keep that...
        out_tri1.points[0] = *inside_points[0];

        // but the two new points are at the locations where the
        // original sides of the triangle (lines) intersect with the plane
        out_tri1.points[1] = VectorIntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0]);
        out_tri1.points[2] = VectorIntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[1]);

        out_tri1.SetWindingCCW();

        return 1; // Return the newly formed single triangle
    }
    if (nInsidePointCount == 2 && nOutsidePointCount == 1)
    {
        // Triangle should be clipped. As 2 points lie inside the plane,
        // the clipped triangle becomes a "quad". Fortunately, we can
        // represent a quad with two new triangles

        // Copy appearance info to new triangles
        out_tri1.color[0] = in_tri.color[0];
        out_tri1.color[1] = in_tri.color[1];
        out_tri1.color[2] = in_tri.color[2];
        out_tri2.color[0] = in_tri.color[0];
        out_tri2.color[1] = in_tri.color[1];
        out_tri2.color[2] = in_tri.color[2];

        // Debugging
        // out_tri1.color = Color{0, 255, 0}; // Green
        // out_tri2.color = Color{255, 0, 0}; // Red


        // The first triangle consists of the two inside points and a new
        // point determined by the location where one side of the triangle
        // intersects with the plane
        out_tri1.points[0] = *inside_points[0];
        out_tri1.points[1] = *inside_points[1];
        out_tri1.points[2] = VectorIntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0]);

        // The second triangle is composed of one of he inside points, a
        // new point determined by the intersection of the other side of the
        // triangle and the plane, and the newly created point above
        out_tri2.points[0] = *inside_points[1];
        out_tri2.points[1] = out_tri1.points[2];
        out_tri2.points[2] = VectorIntersectPlane(plane_p, plane_n, *inside_points[1], *outside_points[0]);


        // Setting the winding
        out_tri1.SetWindingCCW();
        out_tri2.SetWindingCCW();

        return 2; // Return two newly formed triangles which form a quad
    }

    return 0;
}
