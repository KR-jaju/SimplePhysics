#pragma once

#include <vector>
#include "render/Shape.h"

class ShapeRegistry
{
public:
	using Handle = int;
	ShapeRegistry(const ComPtr<ID3D11Device>& device);
	Handle loadShape(const char* path);
	Handle	registerShape(std::vector<XMFLOAT3> positions, std::vector<int> indices);
	Handle	icosahedron();

	Shape& getShape(int handle);
private:
	ComPtr<ID3D11Device> device;
	std::vector<Shape>	shapes;
	Handle	icosahedronHandle;

	void	registerIcosahedron();
};