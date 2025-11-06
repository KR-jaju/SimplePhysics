#pragma once

#include <variant>

struct renderer
{
	struct orthogonal_camera_attribute {
		char dummy;

		XMFLOAT4X4 computeProjectionMatrix() {
			return XMFLOAT4X4{};
		}
	};
	struct perspective_camera_attribute {
		float fov;

		XMFLOAT4X4 computeProjectionMatrix() {
			return XMFLOAT4X4{};
		}
	};
	enum class type {
		MainCamera,
		ShadowCaster
	} rendererType;
	std::variant<orthogonal_camera_attribute, perspective_camera_attribute> attribute;
	XMFLOAT4X4 viewMatrix;
	XMFLOAT4X4 projectionMatrix;

	XMFLOAT4X4	computeProjectionMatrix() {
		return std::visit([&](auto&& attrib) {
			return attrib.computeProjectionMatrix();
			}, this->attribute);
	}
};
