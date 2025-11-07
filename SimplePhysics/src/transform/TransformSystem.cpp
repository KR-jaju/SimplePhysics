#include "pch.h"
#include "transform/TransformSystem.h"
#include "component/transform.h"
#include "component/renderer.h"

TransformSystem::TransformSystem(entt::registry& registry)
{

}
void	TransformSystem::update(entt::registry& registry, float aspectRatio)
{
	auto view = registry.view<transform>();
	
	for (auto [e, tf] : view.each())
	{
		XMMATRIX t = XMMatrixTranslation(tf.position.x, tf.position.y, tf.position.z);
		XMMATRIX r = XMMatrixRotationQuaternion(XMLoadFloat4(&tf.rotation));
		XMMATRIX s = XMMatrixScaling(tf.scale.x, tf.scale.y, tf.scale.z);
		XMMATRIX tInv = XMMatrixTranslation(-tf.position.x, -tf.position.y, -tf.position.z);
		XMMATRIX rInv = XMMatrixTranspose(r);
		XMMATRIX sInv = XMMatrixScaling(1.0f / tf.scale.x, 1.0f /  tf.scale.y, 1.0f / tf.scale.z);

		XMMATRIX m = XMMatrixMultiply(s, XMMatrixMultiply(r, t));
		XMMATRIX mInv = XMMatrixMultiply(tInv, XMMatrixMultiply(rInv, sInv));
		XMStoreFloat4x4(&tf.m, m);
		XMStoreFloat4x4(&tf.mInv, mInv);
		if (renderer* r = registry.try_get<renderer>(e))
		{
			XMMATRIX p = r->computeProjectionMatrix(aspectRatio);
			XMMATRIX vp = XMMatrixMultiply(mInv, p);

			XMStoreFloat4x4(&r->p, p);
			XMStoreFloat4x4(&r->vp, vp);
		}
	}
}