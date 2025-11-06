#include "pch.h"
#include "render/Shape.h"


Shape::Shape(ComPtr<ID3D11Device> device, std::vector<XMFLOAT3> positions, std::vector<int> indices)
	: positions(positions), indices(indices),
	positionBuffer(nullptr), indexBuffer(nullptr)
{
	ComPtr<ID3D11Buffer> positionBuffer;
	ComPtr<ID3D11Buffer> indexBuffer;
    D3D11_BUFFER_DESC desc = {};
    D3D11_SUBRESOURCE_DATA initData = {};

    assert(!positions.empty());
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    desc.ByteWidth = static_cast<UINT>(sizeof(XMFLOAT3) * positions.size());
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    initData.pSysMem = positions.data();
    HRESULT hr = device->CreateBuffer(&desc, &initData, positionBuffer.GetAddressOf());
    assert(SUCCEEDED(hr));
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    desc.ByteWidth = static_cast<UINT>(sizeof(int) * indices.size());
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    initData.pSysMem = indices.data();
    hr = device->CreateBuffer(&desc, &initData, indexBuffer.GetAddressOf());
    assert(SUCCEEDED(hr));
}


void	Shape::draw(ComPtr<ID3D11DeviceContext> ctx)
{

}
