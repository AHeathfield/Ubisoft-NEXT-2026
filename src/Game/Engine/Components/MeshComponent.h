#pragma once

#include "../Graphics/Mesh.h"
#include "../Math/Mat4x4.h"

struct MeshComponent
{
	bool isLoaded = false;
	std::string file_path = "";
	Mesh mesh = Mesh();

	MeshComponent() = default;
	MeshComponent(const std::string& path)
		: file_path(path)
	{
		LoadMesh();
	}
	MeshComponent(const std::string& path, const Color& color)
		: file_path(path)
	{
		LoadMesh();
		mesh.color = color;
	}

	void LoadMesh()
	{
		isLoaded = mesh.LoadFromObjectFile(file_path);
	}
};
