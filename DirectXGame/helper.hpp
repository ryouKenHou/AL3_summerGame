#pragma once
#include <KamataEngine.h>
#include <algorithm>
#include <random>
#include <assert.h>
enum class LRDirection {
	kRight,
	kLeft,
};

inline KamataEngine::Matrix4x4 operator*(const KamataEngine::Matrix4x4& m1, const KamataEngine::Matrix4x4& m2) {
	KamataEngine::Matrix4x4 result = {};
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			for (int k = 0; k < 4; ++k) {
				result.m[i][j] += m1.m[i][k] * m2.m[k][j];
			}
		}
	}
	return result;
}

inline KamataEngine::Vector3 operator+(const KamataEngine::Vector3& v1, const KamataEngine::Vector3& v2) {
	return {v1.x + v2.x, v1.y + v2.y, v1.z + v2.z}; }

inline KamataEngine::Vector3 operator+=(KamataEngine::Vector3& v1, const KamataEngine::Vector3& v2) {
	v1.x += v2.x;
	v1.y += v2.y;
	v1.z += v2.z;
	return v1;
}

inline KamataEngine::Vector3 operator*(const KamataEngine::Vector3& v, float scalar) {
	return {v.x * scalar, v.y * scalar, v.z * scalar}; }

inline KamataEngine::Vector3 operator/(const KamataEngine::Vector3& v, float scalar) { 
	return {v.x / scalar, v.y / scalar, v.z / scalar}; }

inline KamataEngine::Vector3 Lerp(const KamataEngine::Vector3& start, const KamataEngine::Vector3& end, float t) {
	return {
		start.x + (end.x - start.x) * t,
		start.y + (end.y - start.y) * t,
		start.z + (end.z - start.z) * t
	};
}

inline KamataEngine::Matrix4x4 CreateAffineMatrix(const KamataEngine::Vector3& scale, const KamataEngine::Vector3& rotation, const KamataEngine::Vector3& translation) {
	KamataEngine::Matrix4x4 result;
	// スケーリング行列
	KamataEngine::Matrix4x4 scaleMatrix = KamataEngine::MathUtility::MakeScaleMatrix(scale);

	// 回転行列（オイラー角から）
	KamataEngine::Matrix4x4 rotationMatrixX = KamataEngine::MathUtility::MakeRotateXMatrix(rotation.x);
	KamataEngine::Matrix4x4 rotationMatrixY = KamataEngine::MathUtility::MakeRotateYMatrix(rotation.y);
	KamataEngine::Matrix4x4 rotationMatrixZ = KamataEngine::MathUtility::MakeRotateZMatrix(rotation.z);

	// 平行移動行列
	KamataEngine::Matrix4x4 translationMatrix = KamataEngine::MathUtility::MakeTranslateMatrix(translation);

	// アフィン変換行列の合成
	result = scaleMatrix * rotationMatrixZ * rotationMatrixX * rotationMatrixY * translationMatrix;

	return result;
}

struct AABB {
	KamataEngine::Vector3 min;
	KamataEngine::Vector3 max;
	
	inline static bool CheckAABBCollision(const AABB& a, const AABB& b) {
		return (a.min.x <= b.max.x && a.max.x >= b.min.x) && (a.min.y <= b.max.y && a.max.y >= b.min.y) && (a.min.z <= b.max.z && a.max.z >= b.min.z);
	}
};

inline float EaseOut(float start, float end, float t) {
	t = std::clamp(t, 0.0f, 1.0f);
	return start + (end - start) * (1.0f - static_cast<float>(std::pow(1.0f - t, 3)));
}

inline float EaseIn(float start, float end, float t) {
	t = std::clamp(t, 0.0f, 1.0f);
	return start + (end - start) * static_cast<float>(std::pow(t, 3));
}

inline float EaseInOut(float start, float end, float t) {
	t = std::clamp(t, 0.0f, 1.0f);
	if (t < 0.5f) {
		return EaseIn(start, (start + end) / 2.0f, t * 2.0f);
	} else {
		return EaseOut((start + end) / 2.0f, end, (t - 0.5f) * 2.0f);
	}
}

class Random {
	std::mt19937_64 engine_;
	std::random_device seedGenerator;

	public:
	void Initialize() {
		engine_.seed(seedGenerator()); 
	}

	float GetRandomFloat(float min, float max) {
		std::uniform_real_distribution<float> distribution(min, max);
		return distribution(engine_);
	}

};

