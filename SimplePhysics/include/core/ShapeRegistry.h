#pragma once

#include <list>
#include "render/Shape.h"

class ShapeRegistry
{
public:
	ShapeRegistry(const ComPtr<ID3D11Device>& device);
	Shape* loadShape(const char* path);
	Shape*	registerShape(std::vector<XMFLOAT3> positions, std::vector<int> indices);
private:
	ComPtr<ID3D11Device> device;
	std::list<Shape>	shapes;
};