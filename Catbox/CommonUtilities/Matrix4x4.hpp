#pragma once
#include "Vector4.hpp"
#include "Vector3.hpp"
#include <array>
#include <cassert>
#include <thread>


namespace Catbox
{
	template<typename T>
	class Matrix4x4
	{
	public:

		Matrix4x4<T>();
		Matrix4x4<T>(const Matrix4x4<T>& aMatrix1);
		Matrix4x4<T>(const std::array<T, 16> anArray);
		T& operator()(const int aRow, const int aColumn);
		const T& operator()(const int aRow, const int aColumn) const;

		void Reset();

		static Matrix4x4<T> CreateRotationAroundX(T aAngleInRadians);
		static Matrix4x4<T> CreateRotationAroundY(T aAngleInRadians);
		static Matrix4x4<T> CreateRotationAroundZ(T aAngleInRadians);
		static Matrix4x4<T> Transpose(const Matrix4x4<T>& aMatrix1ToTranspose);
		static Matrix4x4<T> ToColumnMajor(const Matrix4x4<T>& aMatrix);
		static Matrix4x4<T> GetFastInverse(const Matrix4x4<T>& aTransform);
		static std::array<T, 16> ToArray(const Matrix4x4<T>& aMatrix);
		static Catbox::Vector3<float> GetTranslationFromMatrix(const Matrix4x4<T>& aMatrix);
		static Catbox::Vector3<float> GetRotationFromMatrix(const Matrix4x4<T>& aMatrix);
		static Catbox::Vector3<float> GetScaleFromMatrix(const Matrix4x4<T>& aMatrix);

		inline const Catbox::Vector3<T> right() { return Catbox::Vector3<T>(myMatrix[0][0], myMatrix[0][1], myMatrix[0][2]).GetNormalized(); }
		inline const Catbox::Vector3<T> left() { return Catbox::Vector3<T>(-myMatrix[0][0], -myMatrix[0][1], -myMatrix[0][2]).GetNormalized(); }
		inline const Catbox::Vector3<T> up() { return Catbox::Vector3<T>(myMatrix[1][0], myMatrix[1][1], myMatrix[1][2]).GetNormalized(); }
		inline const Catbox::Vector3<T> down() { return Catbox::Vector3<T>(-myMatrix[1][0], -myMatrix[1][1], -myMatrix[1][2]).GetNormalized(); }
		inline const Catbox::Vector3<T> forward() { return Catbox::Vector3<T>(myMatrix[2][0], myMatrix[2][1], myMatrix[2][2]).GetNormalized(); }
		inline const Catbox::Vector3<T> back() { return Catbox::Vector3<T>(-myMatrix[2][0], -myMatrix[2][1], -myMatrix[2][2]).GetNormalized(); }


	private:
		std::array<std::array<T, 4>, 4> myMatrix;
	};

	template<typename T>
	inline T& Matrix4x4<T>::operator()(const int aRow, const int aColumn)
	{
		//assert((aRow > 0 && aRow < 5 && aColumn > 0 && aColumn < 5) && "Row or column index was out of range");
		return myMatrix[(aRow) - 1][(aColumn) - 1];
	}

	template<typename T>
	inline const T& Matrix4x4<T>::operator()(const int aRow, const int aColumn) const
	{
		//assert((aRow > 0 && aRow < 5 && aColumn > 0 && aColumn < 5) && "Row or column index was out of range");
		return myMatrix[(aRow) - 1][(aColumn) - 1];
	}

	template<typename T>
	inline void Matrix4x4<T>::Reset()
	{
		for (UINT64 row = 1; row < 5; row++)
		{
			for (UINT64 column = 1; column < 5; column++)
			{
				myMatrix[row - 1][column - 1] = (row == column) ? static_cast<T>(1) : static_cast<T>(0);
			}
		}
	}

	template<typename T>
	Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundX(T aAngleInRadians)
	{
		Matrix4x4<T> matrix;
		matrix.myMatrix[1][1] = cos(aAngleInRadians);
		matrix.myMatrix[1][2] = sin(aAngleInRadians);
		matrix.myMatrix[2][1] = -sin(aAngleInRadians);
		matrix.myMatrix[2][2] = cos(aAngleInRadians);
		return matrix;
	}

	template<typename T>
	Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundY(T aAngleInRadians)
	{
		Matrix4x4<T> matrix;
		matrix.myMatrix[0][0] = cos(aAngleInRadians);
		matrix.myMatrix[0][2] = -sin(aAngleInRadians);
		matrix.myMatrix[2][0] = sin(aAngleInRadians);
		matrix.myMatrix[2][2] = cos(aAngleInRadians);
		return matrix;
	}

	template<typename T>
	Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundZ(T aAngleInRadians)
	{
		Matrix4x4<T> matrix;
		matrix.myMatrix[0][0] = cos(aAngleInRadians);
		matrix.myMatrix[0][1] = sin(aAngleInRadians);
		matrix.myMatrix[1][0] = -sin(aAngleInRadians);
		matrix.myMatrix[1][1] = cos(aAngleInRadians);
		return matrix;
	}

	template<typename T>
	Matrix4x4<T> Matrix4x4<T>::Transpose(const Matrix4x4<T>& aMatrix1ToTranspose)
	{
		Matrix4x4<T> transposedMatrix;

		for (int row = 1; row < 5; row++)
		{
			for (int column = 1; column < 5; column++)
			{
				transposedMatrix(row, column) = aMatrix1ToTranspose(column, row);
			}
		}

		return transposedMatrix;
	}

	template<typename T>
	inline Matrix4x4<T> Matrix4x4<T>::ToColumnMajor(const Matrix4x4<T>& aMatrix)
	{
		Matrix4x4<T> swappedMatrix;
		for (int y = 1; y <= 4; y++)
		{
			for (int x = 1; x <= 4; x++)
			{
				swappedMatrix(y, x) = aMatrix(x, y);
			}
		}
		return swappedMatrix;
	}

	template<typename T>
	inline std::array<T, 16> Matrix4x4<T>::ToArray(const Matrix4x4<T>& aMatrix)
	{
		std::array<T, 16> result;
		for (int y = 0; y < 4; y++)
		{
			for (int x = 0; x < 4; x++)
			{
				result[(4 * y) + x] = aMatrix(y + 1, x + 1);
			}
		}
		return result;
	}

	template<typename T>
	inline Catbox::Vector3<float> Matrix4x4<T>::GetTranslationFromMatrix(const Matrix4x4<T>& aMatrix)
	{
		return { aMatrix(4, 1),aMatrix(4, 2),aMatrix(4, 3) };
	}

	template<typename T>
	inline Catbox::Vector3<float> Matrix4x4<T>::GetRotationFromMatrix(const Matrix4x4<T>& aMatrix)
	{
		Catbox::Vector3<float> scale = GetScaleFromMatrix(aMatrix);
		float yaw = std::atan2(aMatrix(1, 2) / scale.x, aMatrix(1, 1) / scale.x);
		float pitch = std::atan2(-aMatrix(1, 3)/ scale.x, std::sqrt(aMatrix(2, 3) * aMatrix(2, 3) / scale.y + aMatrix(3, 3) * aMatrix(3, 3) / scale.z));
		float roll = std::atan2(aMatrix(2, 3) / scale.y, aMatrix(3, 3) / scale.z);
		constexpr float pi = 3.14159265359f;
		roll = static_cast<float>(roll * 180.0f / pi);
		pitch = static_cast<float>(pitch * 180.0f / pi);
		yaw = static_cast<float>(yaw * 180.0f / pi);
		Vector3f rot = { roll, pitch, yaw };
		return rot;
	}

	template<typename T>
	inline Catbox::Vector3<float> Matrix4x4<T>::GetScaleFromMatrix(const Matrix4x4<T>& aMatrix)
	{
		float x = Vector3f(aMatrix(1, 1), aMatrix(1, 2), aMatrix(1, 3)).Length();
		float y = Vector3f(aMatrix(2, 1), aMatrix(2, 2), aMatrix(2, 3)).Length();
		float z = Vector3f(aMatrix(3, 1), aMatrix(3, 2), aMatrix(3, 3)).Length();
		return { x,y,z };
	}

	template<typename T>
	inline Matrix4x4<T> Matrix4x4<T>::GetFastInverse(const Matrix4x4<T>& aTransform)
	{
		const Matrix4x4<T>& m = aTransform;
		T A3434 = m(3, 3) * m(4, 4) - m(3, 4) * m(4, 3);
		T A2434 = m(3, 2) * m(4, 4) - m(3, 4) * m(4, 2);
		T A2334 = m(3, 2) * m(4, 3) - m(3, 3) * m(4, 2);
		T A1434 = m(3, 1) * m(4, 4) - m(3, 4) * m(4, 1);
		T A1334 = m(3, 1) * m(4, 3) - m(3, 3) * m(4, 1);
		T A1234 = m(3, 1) * m(4, 2) - m(3, 2) * m(4, 1);
		T A3424 = m(2, 3) * m(4, 4) - m(2, 4) * m(4, 3);
		T A2424 = m(2, 2) * m(4, 4) - m(2, 4) * m(4, 2);
		T A2324 = m(2, 2) * m(4, 3) - m(2, 3) * m(4, 2);
		T A3423 = m(2, 3) * m(3, 4) - m(2, 4) * m(3, 3);
		T A2423 = m(2, 2) * m(3, 4) - m(2, 4) * m(3, 2);
		T A2323 = m(2, 2) * m(3, 3) - m(2, 3) * m(3, 2);
		T A1424 = m(2, 1) * m(4, 4) - m(2, 4) * m(4, 1);
		T A1324 = m(2, 1) * m(4, 3) - m(2, 3) * m(4, 1);
		T A1423 = m(2, 1) * m(3, 4) - m(2, 4) * m(3, 1);
		T A1323 = m(2, 1) * m(3, 3) - m(2, 3) * m(3, 1);
		T A1224 = m(2, 1) * m(4, 2) - m(2, 2) * m(4, 1);
		T A1223 = m(2, 1) * m(3, 2) - m(2, 2) * m(3, 1);

		T det = m(1, 1) * (m(2, 2) * A3434 - m(2, 3) * A2434 + m(2, 4) * A2334)
			- m(1, 2) * (m(2, 1) * A3434 - m(2, 3) * A1434 + m(2, 4) * A1334)
			+ m(1, 3) * (m(2, 1) * A2434 - m(2, 2) * A1434 + m(2, 4) * A1234)
			- m(1, 4) * (m(2, 1) * A2334 - m(2, 2) * A1334 + m(2, 3) * A1234);
		det = 1 / det;

		Matrix4x4<T> im;
		im(1, 1) = det * (m(2, 2) * A3434 - m(2, 3) * A2434 + m(2, 4) * A2334);
		im(1, 2) = det * -(m(1, 2) * A3434 - m(1, 3) * A2434 + m(1, 4) * A2334);
		im(1, 3) = det * (m(1, 2) * A3424 - m(1, 3) * A2424 + m(1, 4) * A2324);
		im(1, 4) = det * -(m(1, 2) * A3423 - m(1, 3) * A2423 + m(1, 4) * A2323);
		im(2, 1) = det * -(m(2, 1) * A3434 - m(2, 3) * A1434 + m(2, 4) * A1334);
		im(2, 2) = det * (m(1, 1) * A3434 - m(1, 3) * A1434 + m(1, 4) * A1334);
		im(2, 3) = det * -(m(1, 1) * A3424 - m(1, 3) * A1424 + m(1, 4) * A1324);
		im(2, 4) = det * (m(1, 1) * A3423 - m(1, 3) * A1423 + m(1, 4) * A1323);
		im(3, 1) = det * (m(2, 1) * A2434 - m(2, 2) * A1434 + m(2, 4) * A1234);
		im(3, 2) = det * -(m(1, 1) * A2434 - m(1, 2) * A1434 + m(1, 4) * A1234);
		im(3, 3) = det * (m(1, 1) * A2424 - m(1, 2) * A1424 + m(1, 4) * A1224);
		im(3, 4) = det * -(m(1, 1) * A2423 - m(1, 2) * A1423 + m(1, 4) * A1223);
		im(4, 1) = det * -(m(2, 1) * A2334 - m(2, 2) * A1334 + m(2, 3) * A1234);
		im(4, 2) = det * (m(1, 1) * A2334 - m(1, 2) * A1334 + m(1, 3) * A1234);
		im(4, 3) = det * -(m(1, 1) * A2324 - m(1, 2) * A1324 + m(1, 3) * A1224);
		im(4, 4) = det * (m(1, 1) * A2323 - m(1, 2) * A1323 + m(1, 3) * A1223);
		return im;

		/*Matrix4x4<T> tempTranslation;

		for (int i = 1; i <= 3; ++i)
		{
			tempTranslation(4, i) = aTransform(4, i) * -1;
		}

		Matrix4x4<T> tempRotation;

		for (int i = 1; i <= 3; ++i)
		{
			for (int j = 1; j <= 3; ++j)
			{
				tempRotation(j, i) = aTransform(i, j);
			}
		}

		return tempTranslation * tempRotation;*/
	}

	template<typename T>
	Matrix4x4<T>::Matrix4x4()
	{
		for (UINT64 row = 1; row < 5; row++)
		{
			for (UINT64 column = 1; column < 5; column++)
			{
				myMatrix[row - 1][column - 1] = (row == column) ? static_cast<T>(1) : static_cast<T>(0);
			}
		}
	}

	template<typename T>
	Matrix4x4<T>::Matrix4x4(const Matrix4x4<T>& aMatrix1)
	{
		myMatrix = aMatrix1.myMatrix;
	}

	template<typename T>
	Matrix4x4<T>::Matrix4x4(const std::array<T, 16> anArray)
	{
		for (int y = 0; y < 4; y++)
		{
			for (int x = 0; x < 4; x++)
			{
				myMatrix[y][x] = anArray[(4 * y) + x];
			}
		}
	}

	template<typename T>
	Matrix4x4<T> operator+(const Matrix4x4<T>& aMatrix0, const Matrix4x4<T>& aMatrix1)
	{
		Matrix4x4<T> matrixSum(aMatrix0);
		matrixSum += aMatrix1;
		return matrixSum;
	}

	template<typename T>
	void operator+=(Matrix4x4<T>& aMatrix0, const Matrix4x4<T>& aMatrix1)
	{
		for (int row = 1; row < 5; row++)
		{
			for (int column = 1; column < 5; column++)
			{
				aMatrix0(row, column) += aMatrix1(row, column);
			}
		}
	}

	template<typename T>
	Matrix4x4<T> operator-(const Matrix4x4<T>& aMatrix0, const Matrix4x4<T>& aMatrix1)
	{
		Matrix4x4<T> matrixSum(aMatrix0);

		for (int row = 1; row < 5; row++)
		{
			for (int column = 1; column < 5; column++)
			{
				matrixSum(row, column) -= aMatrix1(row, column);
			}
		}
		return matrixSum;
	}

	template<typename T>
	void operator-=(Matrix4x4<T>& aMatrix0, const Matrix4x4<T>& aMatrix1)
	{
		for (int row = 1; row < 5; row++)
		{
			for (int column = 1; column < 5; column++)
			{
				aMatrix0(row, column) -= aMatrix1(row, column);
			}
		}
	}

	template<typename T>
	Matrix4x4<T> operator*(const Matrix4x4<T>& aMatrix0, const Matrix4x4<T>& aMatrix1)
	{
		Matrix4x4<T> tempMatrix;

		//UNROLLED LOOP
		/*tempMatrix(1, 1) = 0;
		tempMatrix(2, 2) = 0;
		tempMatrix(3, 3) = 0;
		tempMatrix(4, 4) = 0;

		tempMatrix(1, 1) += aMatrix0(1, 1) * aMatrix1(1, 1);
		tempMatrix(1, 1) += aMatrix0(1, 2) * aMatrix1(2, 1);
		tempMatrix(1, 1) += aMatrix0(1, 3) * aMatrix1(3, 1);
		tempMatrix(1, 1) += aMatrix0(1, 4) * aMatrix1(4, 1);
		tempMatrix(1, 2) += aMatrix0(1, 1) * aMatrix1(1, 2);
		tempMatrix(1, 2) += aMatrix0(1, 2) * aMatrix1(2, 2);
		tempMatrix(1, 2) += aMatrix0(1, 3) * aMatrix1(3, 2);
		tempMatrix(1, 2) += aMatrix0(1, 4) * aMatrix1(4, 2);
		tempMatrix(1, 3) += aMatrix0(1, 1) * aMatrix1(1, 3);
		tempMatrix(1, 3) += aMatrix0(1, 2) * aMatrix1(2, 3);
		tempMatrix(1, 3) += aMatrix0(1, 3) * aMatrix1(3, 3);
		tempMatrix(1, 3) += aMatrix0(1, 4) * aMatrix1(4, 3);
		tempMatrix(1, 4) += aMatrix0(1, 1) * aMatrix1(1, 4);
		tempMatrix(1, 4) += aMatrix0(1, 2) * aMatrix1(2, 4);
		tempMatrix(1, 4) += aMatrix0(1, 3) * aMatrix1(3, 4);
		tempMatrix(1, 4) += aMatrix0(1, 4) * aMatrix1(4, 4);


		tempMatrix(2, 1) += aMatrix0(2, 1) * aMatrix1(1, 1);
		tempMatrix(2, 1) += aMatrix0(2, 2) * aMatrix1(2, 1);
		tempMatrix(2, 1) += aMatrix0(2, 3) * aMatrix1(3, 1);
		tempMatrix(2, 1) += aMatrix0(2, 4) * aMatrix1(4, 1);
		tempMatrix(2, 2) += aMatrix0(2, 1) * aMatrix1(1, 2);
		tempMatrix(2, 2) += aMatrix0(2, 2) * aMatrix1(2, 2);
		tempMatrix(2, 2) += aMatrix0(2, 3) * aMatrix1(3, 2);
		tempMatrix(2, 2) += aMatrix0(2, 4) * aMatrix1(4, 2);
		tempMatrix(2, 3) += aMatrix0(2, 1) * aMatrix1(1, 3);
		tempMatrix(2, 3) += aMatrix0(2, 2) * aMatrix1(2, 3);
		tempMatrix(2, 3) += aMatrix0(2, 3) * aMatrix1(3, 3);
		tempMatrix(2, 3) += aMatrix0(2, 4) * aMatrix1(4, 3);
		tempMatrix(2, 4) += aMatrix0(2, 1) * aMatrix1(1, 4);
		tempMatrix(2, 4) += aMatrix0(2, 2) * aMatrix1(2, 4);
		tempMatrix(2, 4) += aMatrix0(2, 3) * aMatrix1(3, 4);
		tempMatrix(2, 4) += aMatrix0(2, 4) * aMatrix1(4, 4);

		tempMatrix(3, 1) += aMatrix0(3, 1) * aMatrix1(1, 1);
		tempMatrix(3, 1) += aMatrix0(3, 2) * aMatrix1(2, 1);
		tempMatrix(3, 1) += aMatrix0(3, 3) * aMatrix1(3, 1);
		tempMatrix(3, 1) += aMatrix0(3, 4) * aMatrix1(4, 1);
		tempMatrix(3, 2) += aMatrix0(3, 1) * aMatrix1(1, 2);
		tempMatrix(3, 2) += aMatrix0(3, 2) * aMatrix1(2, 2);
		tempMatrix(3, 2) += aMatrix0(3, 3) * aMatrix1(3, 2);
		tempMatrix(3, 2) += aMatrix0(3, 4) * aMatrix1(4, 2);
		tempMatrix(3, 3) += aMatrix0(3, 1) * aMatrix1(1, 3);
		tempMatrix(3, 3) += aMatrix0(3, 2) * aMatrix1(2, 3);
		tempMatrix(3, 3) += aMatrix0(3, 3) * aMatrix1(3, 3);
		tempMatrix(3, 3) += aMatrix0(3, 4) * aMatrix1(4, 3);
		tempMatrix(3, 4) += aMatrix0(3, 1) * aMatrix1(1, 4);
		tempMatrix(3, 4) += aMatrix0(3, 2) * aMatrix1(2, 4);
		tempMatrix(3, 4) += aMatrix0(3, 3) * aMatrix1(3, 4);
		tempMatrix(3, 4) += aMatrix0(3, 4) * aMatrix1(4, 4);

		tempMatrix(4, 1) += aMatrix0(4, 1) * aMatrix1(1, 1);
		tempMatrix(4, 1) += aMatrix0(4, 2) * aMatrix1(2, 1);
		tempMatrix(4, 1) += aMatrix0(4, 3) * aMatrix1(3, 1);
		tempMatrix(4, 1) += aMatrix0(4, 4) * aMatrix1(4, 1);
		tempMatrix(4, 2) += aMatrix0(4, 1) * aMatrix1(1, 2);
		tempMatrix(4, 2) += aMatrix0(4, 2) * aMatrix1(2, 2);
		tempMatrix(4, 2) += aMatrix0(4, 3) * aMatrix1(3, 2);
		tempMatrix(4, 2) += aMatrix0(4, 4) * aMatrix1(4, 2);
		tempMatrix(4, 3) += aMatrix0(4, 1) * aMatrix1(1, 3);
		tempMatrix(4, 3) += aMatrix0(4, 2) * aMatrix1(2, 3);
		tempMatrix(4, 3) += aMatrix0(4, 3) * aMatrix1(3, 3);
		tempMatrix(4, 3) += aMatrix0(4, 4) * aMatrix1(4, 3);
		tempMatrix(4, 4) += aMatrix0(4, 1) * aMatrix1(1, 4);
		tempMatrix(4, 4) += aMatrix0(4, 2) * aMatrix1(2, 4);
		tempMatrix(4, 4) += aMatrix0(4, 3) * aMatrix1(3, 4);
		tempMatrix(4, 4) += aMatrix0(4, 4) * aMatrix1(4, 4);*/

		for (int row = 1; row < 5; row++)
		{
			for (int column = 1; column < 5; column++)
			{
				if (row == column) tempMatrix(row, column) = 0;
				for (int i = 1; i < 5; i++)
				{
					tempMatrix(row, column) += aMatrix0(row, i) * aMatrix1(i, column);
				}
			}
		}

		return tempMatrix;
	}

	template<typename T>
	void operator*=(Matrix4x4<T>& aMatrix0, const Matrix4x4<T>& aMatrix1)
	{
		aMatrix0 = aMatrix0 * aMatrix1;
	}

	template<typename T>
	Matrix4x4<T> operator*(const Matrix4x4<T>& aMatrix, const Vector4<T>& aVector)
	{
		Matrix4x4<T> newMatrix;
		for (int row = 1; row < 5; row++)
		{
			for (int column = 1; column < 5; column++)
			{
				T sum = 0;
				sum += aMatrix(row, column) * aVector[column - 1];
				newMatrix(row, column) = sum;
			}
		}
		return newMatrix;
	}

	template<typename T>
	Vector4<T> operator*(const Vector4<T>& aVector, const Matrix4x4<T>& aMatrix)
	{
		Vector4<T> multiplied;

		multiplied.x = aVector.x * aMatrix(1, 1) + aVector.y * aMatrix(2, 1) + aVector.z * aMatrix(3, 1) + aVector.w * aMatrix(4, 1);
		multiplied.y = aVector.x * aMatrix(1, 2) + aVector.y * aMatrix(2, 2) + aVector.z * aMatrix(3, 2) + aVector.w * aMatrix(4, 2);
		multiplied.z = aVector.x * aMatrix(1, 3) + aVector.y * aMatrix(2, 3) + aVector.z * aMatrix(3, 3) + aVector.w * aMatrix(4, 3);
		multiplied.w = aVector.x * aMatrix(1, 4) + aVector.y * aMatrix(2, 4) + aVector.z * aMatrix(3, 4) + aVector.w * aMatrix(4, 4);

		return multiplied;
	}

	template<typename T>
	bool operator==(const Matrix4x4<T>& aMatrix0, const Matrix4x4<T>& aMatrix1)
	{
		for (int row = 1; row < 5; row++)
		{
			for (int column = 1; column < 5; column++)
			{
				if (aMatrix0(row, column) != aMatrix1(row, column)) return false;
			}
		}
		return true;
	}
}