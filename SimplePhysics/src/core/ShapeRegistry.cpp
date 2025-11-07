#include "pch.h"
#include "core/ShapeRegistry.h"
#include <iostream>


ShapeRegistry::ShapeRegistry(const ComPtr<ID3D11Device>& device)
{
	this->device = device;

	this->registerIcosahedron();
}

ShapeRegistry::Handle ShapeRegistry::loadShape(const char* path)
{
	return -1;
}

ShapeRegistry::Handle ShapeRegistry::registerShape(std::vector<XMFLOAT3> positions, std::vector<int> indices)
{
	const Handle handle = this->shapes.size();

	this->shapes.emplace_back(device, positions, indices);
	return (handle);
}

ShapeRegistry::Handle	ShapeRegistry::icosahedron()
{
	return this->icosahedronHandle;
}


Shape& ShapeRegistry::getShape(int handle)
{
	return this->shapes[handle];
}

void	ShapeRegistry::registerIcosahedron()
{
    const float phi = (1.0f + std::sqrt(5.0f)) * 0.5f; // È²±Ýºñ
    const float a = 1.0f;
    const float b = phi;

    std::vector<XMFLOAT3> positions = {
        {-a,  b, 0}, { a,  b, 0}, {-a, -b, 0}, { a, -b, 0},
        { 0, -a,  b}, { 0,  a,  b}, { 0, -a, -b}, { 0,  a, -b},
        { b,  0, -a}, { b,  0,  a}, {-b,  0, -a}, {-b,  0,  a}
    };

    for (auto& p : positions)
    {
        XMVECTOR tmp = XMLoadFloat3(&p);
        tmp = XMVector3Normalize(tmp);
        XMStoreFloat3(&p, tmp);
    }

    std::vector<int> indices = {
        0,11,5,   0,5,1,    0,1,7,    0,7,10,   0,10,11,
        1,5,9,    5,11,4,   11,10,2,  10,7,6,   7,1,8,
        3,9,4,    3,4,2,    3,2,6,    3,6,8,    3,8,9,
        4,9,5,    2,4,11,   6,2,10,   8,6,7,    9,8,1
    };
    this->icosahedronHandle = this->registerShape(std::move(positions), std::move(indices));
    std::cout << this->icosahedronHandle;
}
