#pragma once

struct transform
{
    XMFLOAT3 position = {0, 0, 0};
    XMFLOAT4 rotation = { 0, 0, 0, 1 };
    XMFLOAT3 scale = { 1, 1, 1 };
    XMFLOAT3 velocity = { 0, 0, 0 };
    XMFLOAT4X4 m;
    XMFLOAT4X4 mInv;
};