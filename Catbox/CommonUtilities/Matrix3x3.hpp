#pragma once
#include "Vector3.hpp"
#include <array>
#include <cassert>
#include "Matrix4x4.hpp"

namespace Catbox
{
	template<typename T>
	class Matrix3x3
	{
	public:
		Matrix3x3<T>();
		Matrix3x3<T>(const Matrix3x3<T>& aMatrix1);
		Matrix3x3<T>(const Matrix4x4<T>& aMatrix1);
		T& operator()(const int aRow, const int aColumn);
		const T& operator()(const int aRow, const int aColumn) const;

		static Matrix3x3<T> CreateRotationAroundX(T aAngleInRadians);
		static Matrix3x3<T> CreateRotationAroundY(T aAngleInRadians);
		static Matrix3x3<T> CreateRotationAroundZ(T aAngleInRadians);
		static Matrix3x3<T> Transpose(const Matrix3x3<T>& aMatrix1ToTranspose);

	private:
		std::array<std::array<T, 3>, 3> myMatrix;
	};

	template<typename T>
	inline T& Matrix3x3<T>::operator()(const int aRow, const int aColumn)
	{
		assert((aRow > 0 && aRow < 4 && aColumn > 0 && aColumn < 4) && "Row or column index was out of range");
		return myMatrix[aRow - 1][aColumn - 1];
	}

	template<typename T>
	inline const T& Matrix3x3<T>::operator()(const int aRow, const int aColumn) const
	{
		assert((aRow > 0 && aRow < 4 && aColumn > 0 && aColumn < 4) && "Row or column index was out of range");
		return myMatrix[aRow - 1][aColumn - 1];
	}

	template<typename T>
	Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundX(T aAngleInRadians)
	{
		Matrix3x3<T> matrix;
		matrix.myMatrix[1][1] = cos(aAngleInRadians);
		matrix.myMatrix[1][2] = sin(aAngleInRadians);
		matrix.myMatrix[2][1] = -sin(aAngleInRadians);
		matrix.myMatrix[2][2] = cos(aAngleInRadians);
		return matrix;
	}

	template<typename T>
	Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundY(T aAngleInRadians)
	{
		Matrix3x3<T> matrix;
		matrix.myMatrix[0][0] = cos(aAngleInRadians);
		matrix.myMatrix[0][2] = -sin(aAngleInRadians);
		matrix.myMatrix[2][0] = sin(aAngleInRadians);
		matrix.myMatrix[2][2] = cos(aAngleInRadians);
		return matrix;
	}

	template<typename T>
	Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundZ(T aAngleInRadians)
	{
		Matrix3x3<T> matrix;
		matrix.myMatrix[0][0] = cos(aAngleInRadians);
		matrix.myMatrix[0][1] = sin(aAngleInRadians);
		matrix.myMatrix[1][0] = -sin(aAngleInRadians);
		matrix.myMatrix[1][1] = cos(aAngleInRadians);
		return matrix;
	}

	template<typename T>
	Matrix3x3<T> Matrix3x3<T>::Transpose(const Matrix3x3<T>& aMatrix1ToTranspose)
	{
		Matrix3x3<T> transposedMatrix;

		for (int row = 1; row < 4; row++)
		{
			for (int column = 1; column < 4; column++)
			{
				transposedMatrix(row, column) = aMatrix1ToTranspose(column, row);
			}
		}

		return transposedMatrix;
	}

	template<typename T>
	Matrix3x3<T>::Matrix3x3()
	{
		for (int row = 1; row < 4; row++)
		{
			for (int column = 1; column < 4; column++)
			{
				myMatrix[row - 1][column - 1] = (row == column) ? 1 : 0;
			}
		}
	}

	template<typename T>
	Matrix3x3<T>::Matrix3x3(const Matrix3x3<T>& aMatrix1)
	{
		myMatrix = aMatrix1.myMatrix;
	}

	template<typename T>
	Matrix3x3<T>::Matrix3x3(const Matrix4x4<T>& aMatrix1)
	{
		for (int row = 0; row < 3; row++)
		{
			for (int column = 0; column < 3; column++)
			{
				myMatrix[row][column] = aMatrix1(row + 1, column + 1);
			}
		}
	}

	template<typename T>
	Matrix3x3<T> operator+(const Matrix3x3<T>& aMatrix0, const Matrix3x3<T>& aMatrix1)
	{
		Matrix3x3<T> matrixSum(aMatrix0);
		matrixSum += aMatrix1;
		return matrixSum;
	}

	template<typename T>
	void operator+=(Matrix3x3<T>& aMatrix0, const Matrix3x3<T>& aMatrix1)
	{
		for (int row = 1; row < 4; row++)
		{
			for (int column = 1; column < 4; column++)
			{
				aMatrix0(row, column) += aMatrix1(row, column);
			}
		}
	}

	template<typename T>
	Matrix3x3<T> operator-(const Matrix3x3<T>& aMatrix0, const Matrix3x3<T>& aMatrix1)
	{
		Matrix3x3<T> matrixSum(aMatrix0);

		for (int row = 1; row < 4; row++)
		{
			for (int column = 1; column < 4; column++)
			{
				matrixSum(row, column) -= aMatrix1(row, column);
			}
		}
		return matrixSum;
	}

	template<typename T>
	void operator-=(Matrix3x3<T>& aMatrix0, const Matrix3x3<T>& aMatrix1)
	{
		for (int row = 1; row < 4; row++)
		{
			for (int column = 1; column < 4; column++)
			{
				aMatrix0(row, column) -= aMatrix1(row, column);
			}
		}
	}

	template<typename T>
	Matrix3x3<T> operator*(const Matrix3x3<T>& aMatrix0, const Matrix3x3<T>& aMatrix1)
	{
		Matrix3x3<T> tempMatrix;

		for (int row = 1; row < 4; row++)
		{
			for (int column = 1; column < 4; column++)
			{
				if (row == column) tempMatrix(row, column) = 0;
				for (int i = 1; i < 4; i++)
				{
					tempMatrix(row, column) += aMatrix0(row, i) * aMatrix1(i, column);
				}
			}
		}
		return tempMatrix;
	}

	template<typename T>
	void operator*=(Matrix3x3<T>& aMatrix0, const Matrix3x3<T>& aMatrix1)
	{
		aMatrix0 = aMatrix0 * aMatrix1;
	}

	template<typename T>
	Matrix3x3<T> operator*(const Matrix3x3<T>& aMatrix, const Vector3<T>& aVector)
	{
		for (int row = 1; row < 4; row++)
		{
			for (int column = 1; column < 4; column++)
			{
				int sum = 0;
				sum += aMatrix(row, column) * aVector[column - 1];
				aMatrix(row, column) = sum;
			}
		}
		return aMatrix;
	}

	template<typename T>
	Vector3<T> operator*(const Vector3<T>& aVector, const Matrix3x3<T>& aMatrix)
	{
		Vector3<T> multiplied;

		multiplied.x = aVector.x * aMatrix(1, 1) + aVector.y * aMatrix(2, 1) + aVector.z * aMatrix(3, 1);
		multiplied.y = aVector.x * aMatrix(1, 2) + aVector.y * aMatrix(2, 2) + aVector.z * aMatrix(3, 2);
		multiplied.z = aVector.x * aMatrix(1, 3) + aVector.y * aMatrix(2, 3) + aVector.z * aMatrix(3, 3);

		return multiplied;
	}

	template<typename T>
	bool operator==(const Matrix3x3<T>& aMatrix0, const Matrix3x3<T>& aMatrix1)
	{
		for (int row = 1; row < 4; row++)
		{
			for (int column = 1; column < 4; column++)
			{
				if (aMatrix0(row, column) != aMatrix1(row, column)) return false;
			}
		}
		return true;
	}
}