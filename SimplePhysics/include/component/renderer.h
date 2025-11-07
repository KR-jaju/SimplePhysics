#pragma once

#include <variant>

struct renderer
{
	struct orthogonal_camera_attribute { // Not Implemented yet
		char dummy;

		XMMATRIX computeProjectionMatrix(float aspectRatio) {
			return XMMATRIX{};
		}
	};
	struct perspective_camera_attribute {
		float fov = 3.141592f / 2.0f;
		float nearZ = 0.3f;
		float farZ = 100.0f;

		XMMATRIX computeProjectionMatrix(float aspectRatio) {
			return XMMatrixPerspectiveFovLH(this->fov, aspectRatio, this->nearZ, this->farZ);
		}
	};
	enum class type {
		MainCamera,
		ShadowCaster
	} rendererType;
	std::variant<perspective_camera_attribute, orthogonal_camera_attribute> attribute;
	XMFLOAT4X4 p;
	XMFLOAT4X4	vp;

	XMMATRIX	computeProjectionMatrix(float aspectRatio) {
		return std::visit([&](auto&& attrib) {
			return attrib.computeProjectionMatrix(aspectRatio);
			}, this->attribute);
	}
};
