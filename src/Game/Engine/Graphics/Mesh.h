#pragma once

#include "../Math/Triangle.h"
#include "../Core/Utility.h"
#include "../Math/BVHNode.h"
#include "../Components/Transform3DComponent.h"
#include "Color.h"
#include <cstring>
#include <vector>
#include <fstream>
#include <sstream>

struct Mesh
{
    std::vector<Triangle> triangles;
    BVHNode* accelerator = nullptr;
    Color color = Color(255, 255, 255);
    Transform3DComponent prevTransform = Transform3DComponent();
    bool isWireframe = false;

    bool LoadFromObjectFile(std::string file_path)
    {
        std::ifstream file(file_path);
        if (!file.is_open())
        {
            utility::log("Error File: " + file_path + " did not open!");
            return false;
        }

        // Local cache of verts
        std::vector<Vector3> verts;

        while (!file.eof())
        {
            std::string line;
            getline(file, line);

            // Stringstream is really useful for allowing to read and write to strings like they are streams (similar to cin and cout)
            std::stringstream ss(line);

            char junk;

            // v for vertice
            if (line[0] == 'v')
            {
                Vector3 v;
                // This outputs the first word in the stream into junk, second word into v.x and so on
                // word as in text split by spaces
                ss >> junk >> v.x >> v.y >> v.z;

                verts.push_back(v);
            }

            // f is for face and in our case it's a Triangle
            if (line[0] == 'f')
            {
                int f[3];
                ss >> junk >> f[0] >> f[1] >> f[2];
                // Adds Triangles to the mesh, also obj file starts counting at index 1 instead of 0 which is why we subract 1
                Triangle temp = {
                        verts[f[0] - 1],
                        verts[f[1] - 1],
                        verts[f[2] - 1],
                        color,
                        color,
                        color
                };
                // temp.SetWindingCCW();
                triangles.push_back(temp);
            }
        }

        return true;
    }


    Mesh GetUntransformedMesh() const
    {
        Mesh result;
        const int SIZE = triangles.size();
        result.triangles.reserve(SIZE);
        Mat4x4 matTransform = prevTransform.GetWorldMatrix().GetQuickInverse();
        for (int i = 0; i < SIZE; i++)
        {
            Triangle newTri;
            newTri.points[0] = matTransform.GetMultiplyVector3(triangles[i].points[0]);
            newTri.points[1] = matTransform.GetMultiplyVector3(triangles[i].points[1]);
            newTri.points[2] = matTransform.GetMultiplyVector3(triangles[i].points[2]);

            result.triangles[i] = newTri; 
        }

        result.prevTransform = Transform3DComponent();
        return result;
    }
 };
