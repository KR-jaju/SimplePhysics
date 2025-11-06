#include "pch.h"
#include "core/ShapeRegistry.h"



ShapeRegistry::ShapeRegistry(const ComPtr<ID3D11Device>& device)
{
	this->device = device;
}

Shape* ShapeRegistry::loadShape(const char* path)
{
	return nullptr;
}

Shape* ShapeRegistry::registerShape(std::vector<XMFLOAT3> positions, std::vector<int> indices)
{
	return &this->shapes.emplace_back(device, positions, indices);
}