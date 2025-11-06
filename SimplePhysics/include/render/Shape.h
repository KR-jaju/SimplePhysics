#pragma once

#include <functional>

class Shape
{
public:
	Shape(ComPtr<ID3D11Device> device, std::vector<XMFLOAT3> positions, std::vector<int> indices);
	void	draw(ComPtr<ID3D11DeviceContext> ctx);
	const std::vector<XMFLOAT3>&	getPositions() const;
	const std::vector<int>&	getIndices() const;
private:
	std::vector<XMFLOAT3> positions;
	std::vector<int> indices;
	ComPtr<ID3D11Buffer> positionBuffer;
	ComPtr<ID3D11Buffer> indexBuffer;
};
