#include "MatrixFunction.h"
#include <math.h>

template<typename MatrixSize>
int ColumnSize(MatrixSize m)
{
	return sizeof(m.m[0]) / sizeof(int);
}

template<typename MatrixSize>
int RowSize(MatrixSize m)
{
	return sizeof(m.m) / sizeof(m.m[0]);
}

Matrix4x4 Add(Matrix4x4 m1, Matrix4x4 m2)
{
	Matrix4x4 result;
	int size = ColumnSize(m1);
	for (int row = 0; row < size; row++)
	{
		for (int column = 0; column < size; column++)
		{
			result.m[row][column] = m1.m[row][column] + m2.m[row][column];
		}
	}
	return result;
}


Matrix4x4 Subtract(Matrix4x4 m1, Matrix4x4 m2)
{
	Matrix4x4 result;
	int size = ColumnSize(m1);
	for (int row = 0; row < size; row++)
	{
		for (int column = 0; column < size; column++)
		{
			result.m[row][column] = m1.m[row][column] - m2.m[row][column];
		}
	}
	return result;
}


Matrix4x4 Multiply(Matrix4x4 m1, Matrix4x4 m2)
{
	Matrix4x4 result;
	int size = ColumnSize(m1);
	int m1Column = ColumnSize(m1);
	int m2Row = RowSize(m2);
	assert(m1Column == m2Row);//
	for (int row = 0; row < size; row++)
	{
		for (int column = 0; column < size; column++)
		{
			result.m[row][column] = 0;
			for (int i = 0; i < size; i++)
			{
				result.m[row][column] += m1.m[row][i] * m2.m[i][column];
			}
		}
	}
	return result;
}


Matrix4x4 Scalar(float k, Matrix4x4 m)
{
	Matrix4x4 result;
	int size = ColumnSize(m);
	for (int row = 0; row < size; row++)
	{
		for (int column = 0; column < size; column++)
		{
			result.m[row][column] = k * m.m[row][column];
		}
	}
	return result;
}
auto CofactorExpansion(Matrix2x2 m)
{
	return (m.m[0][0] * m.m[1][1] - m.m[0][1] * m.m[1][0]);
}
auto CofactorExpansion(Matrix3x3 m)
{
	int size = ColumnSize(m);
	Matrix2x2 result[4];
	//Matrix2x2 ans;
	double ans = 0.0f;
	for (int i = 0; i < size; i++)
	{
		double k = m.m[i][0];
		k *= pow(-1.0, double((i + 1) + 1));
		int row2 = 0;
		int column2 = 0;
		for (int row = 0; row < size; row++)
		{
			for (int column = 0; column < size; column++)
			{
				if (row != i && column != 0)
				{
					result[i].m[row2][column2] = m.m[row][column];
					column2++;
					if (column2 >= size - 1)
					{
						column2 = 0;
						row2++;
					}
				}
			}
		}
		//result[i] = Scalar(k, result[i]);
		ans += k * CofactorExpansion(result[i]);
	}
	/*ans = result[0];
	for (int i = 1; i < size; i++)
	{
		ans = Add(ans, result[i]);
	}

	if (size == 2)
	{
		//return ans;
	}
	for (int i = 0; i < size; i++)
	{
		ans = Add(ans, result[i]);
	}
	return CofactorExpansion(ans);
	*/
	return ans;
}


double CofactorExpansion(Matrix4x4 m)
{
	int size = ColumnSize(m);
	Matrix3x3 result[4];
	//Matrix3x3 ans;
	double ans = 0.0f;
	for (int i = 0; i < size; i++)
	{
		double k = m.m[i][0];
		k *= pow(-1.0f, double((i + 1) + 1));
		int row2 = 0;
		int column2 = 0;
		for (int row = 0; row < size; row++)
		{
			for (int column = 0; column < size; column++)
			{
				if (row != i && column != 0)
				{
					result[i].m[row2][column2] = m.m[row][column];
					column2++;
					if (column2 >= size - 1)
					{
						column2 = 0;
						row2++;
					}
				}
			}
		}
		//result[i] = Scalar(k, result[i]);
		ans += k * CofactorExpansion(result[i]);
	}
	/*ans = result[0];
	for (int i = 1; i < size; i++)
	{
		ans = Add(ans, result[i]);
	}

	if (size == 2)
	{
		//return ans;
	}
	for (int i = 0; i < size; i++)
	{
		ans = Add(ans, result[i]);
	}
	return CofactorExpansion(ans);
	*/
	return ans;
}


Matrix4x4 Transpose(Matrix4x4 m)
{
	Matrix4x4 result;
	int size = ColumnSize(m);
	for (int row = 0; row < size; row++)
	{
		for (int column = 0; column < size; column++)
		{
			result.m[row][column] = m.m[column][row];
		}
	}
	return result;
}

Matrix3x3 Transpose(Matrix3x3 m)
{
	Matrix3x3 result;
	int size = ColumnSize(m);
	for (int row = 0; row < size; row++)
	{
		for (int column = 0; column < size; column++)
		{
			result.m[row][column] = m.m[column][row];
		}
	}
	return result;
}

Matrix2x2 Transpose(Matrix2x2 m)
{
	Matrix2x2 result;
	int size = ColumnSize(m);
	for (int row = 0; row < size; row++)
	{
		for (int column = 0; column < size; column++)
		{
			result.m[row][column] = m.m[column][row];
		}
	}
	return result;
}

Matrix4x4 Adjoint(Matrix4x4 m)
{
	int size = ColumnSize(m);
	Matrix4x4 result = { 0 };
	Matrix3x3 matrix = { 0 };
	for (int rowRes = 0; rowRes < size; rowRes++)
	{
		for (int columnRes = 0; columnRes < size; columnRes++)
		{
			double k = m.m[rowRes][columnRes];
			k = pow(-1.0, double((rowRes + 1) + double(columnRes + 1)));
			int row2 = 0;
			int column2 = 0;
			for (int row = 0; row < size; row++)
			{
				for (int column = 0; column < size; column++)
				{
					if (row != rowRes && column != columnRes)
					{
						matrix.m[row2][column2] = m.m[row][column];
						column2++;
						if (column2 >= size - 1)
						{
							column2 = 0;
							row2++;
						}
					}

				}
			}
			double res = CofactorExpansion(matrix);
			result.m[rowRes][columnRes] = float(k * res);
		}

	}
	result = Transpose(result);
	return result;
}
Matrix3x3 Adjoint(Matrix3x3 m)
{
	int size = ColumnSize(m);
	Matrix3x3 result = { 0 };
	Matrix2x2 matrix = { 0 };
	for (int rowRes = 0; rowRes < size; rowRes++)
	{
		for (int columnRes = 0; columnRes < size; columnRes++)
		{
			double k = m.m[rowRes][columnRes];
			k = pow(-1.0, double((rowRes + 1) + double(columnRes + 1)));
			int row2 = 0;
			int column2 = 0;
			for (int row = 0; row < size; row++)
			{
				for (int column = 0; column < size; column++)
				{
					if (row != rowRes && column != columnRes)
					{
						matrix.m[row2][column2] = m.m[row][column];
						column2++;
						if (column2 >= size - 1)
						{
							column2 = 0;
							row2++;
						}
					}

				}
			}
			double res = CofactorExpansion(matrix);
			result.m[rowRes][columnRes] = float(k * res);
		}

	}
	result = Transpose(result);
	return result;
}
Matrix2x2 Adjoint(Matrix2x2 m)
{
	int size = ColumnSize(m);
	Matrix2x2 result = { 0 };
	float matrix = { 0 };
	for (int rowRes = 0; rowRes < size; rowRes++)
	{
		for (int columnRes = 0; columnRes < size; columnRes++)
		{
			double k = m.m[rowRes][columnRes];
			k = pow(-1.0, double((rowRes + 1) + double(columnRes + 1)));
			int row2 = 0;
			int column2 = 0;
			for (int row = 0; row < size; row++)
			{
				for (int column = 0; column < size; column++)
				{
					if (row != rowRes && column != columnRes)
					{
						matrix = m.m[row][column];
						column2++;
						if (column2 >= size - 1)
						{
							column2 = 0;
							row2++;
						}
					}

				}
			}
			//double res = CofactorExpansion(matrix);
			result.m[rowRes][columnRes] = float(k * matrix);
		}

	}
	result = Transpose(result);
	return result;
}


Matrix4x4 Inverse(Matrix4x4 m)
{
	int size = ColumnSize(m);
	Matrix4x4 result;
	Matrix2x2 matrix = { 0 };
	double determinant = CofactorExpansion(m);
	/*if (size>2)
	{
		matrix = CofactorExpansion(m);
		determinant = (matrix.m[0][0] * matrix.m[1][1] - matrix.m[0][1] * matrix.m[1][0]);
	}
	else
	{
		determinant = (m.m[0][0] * m.m[1][1] - m.m[0][1] * m.m[1][0]);
	}*/
	//double determinant = (matrix.m[0][0] * matrix.m[1][1] - matrix.m[0][1] * matrix.m[1][0]);
	Matrix4x4 adjugateMatrix = Adjoint(m);


	for (int row = 0; row < size; row++)
	{
		for (int column = 0; column < size; column++)
		{
			result.m[row][column] = float(1.0 / determinant) * adjugateMatrix.m[row][column];
		}
	}
	return result;
}

Matrix4x4 MakeIdentity4x4()
{
	Matrix4x4 result;
	int size = 4;
	for (int row = 0; row < size; row++)
	{
		for (int column = 0; column < size; column++)
		{
			if (row == column)
			{
				result.m[row][column] = 1;
			}
			else
			{
				result.m[row][column] = 0;
			}
		}
	}
	return result;
}

//アフィン変換関係

Matrix4x4 MakeTranslateMatrix(const Vector3& translate)
{
	Matrix4x4 translateMatrix = MakeIdentity4x4();
	translateMatrix.m[3][0] = translate.x;
	translateMatrix.m[3][1] = translate.y;
	translateMatrix.m[3][2] = translate.z;
	return translateMatrix;
}

Matrix4x4 MakeScaleMatrix(const Vector3& scale)
{
	Matrix4x4 scaleMatrix = MakeIdentity4x4();
	scaleMatrix.m[0][0] = scale.x;
	scaleMatrix.m[1][1] = scale.y;
	scaleMatrix.m[2][2] = scale.z;
	return scaleMatrix;
}

Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix)
{
	Vector3 transform;
	float transformMatrix[4];
	float matrix1x4[4] = { vector.x,vector.y,vector.z ,1.0f };
	for (int column = 0; column < 4; column++)
	{
		transformMatrix[column] = 0.0f;
		for (int i = 0; i < 4; i++)
		{
			transformMatrix[column] += matrix1x4[i] * matrix.m[i][column];
		}
	}
	float w = transformMatrix[3];
	assert(w != 0.0f);
	transform.x = transformMatrix[0] / w;
	transform.y = transformMatrix[1] / w;
	transform.z = transformMatrix[2] / w;
	return transform;
}

Matrix4x4 MakeRotateXMatrix(float radian)
{
	Matrix4x4 rotate = MakeIdentity4x4();
	rotate.m[1][1] = std::cos(radian);
	rotate.m[1][2] = std::sin(radian);
	rotate.m[2][1] = -std::sin(radian);
	rotate.m[2][2] = std::cos(radian);

	return rotate;
}

Matrix4x4 MakeRotateYMatrix(float radian)
{
	Matrix4x4 rotate = MakeIdentity4x4();
	rotate.m[0][0] = std::cos(radian);
	rotate.m[2][0] = std::sin(radian);
	rotate.m[0][2] = -std::sin(radian);
	rotate.m[2][2] = std::cos(radian);

	return rotate;
}
Matrix4x4 MakeRotateZMatrix(float radian)
{
	Matrix4x4 rotate = MakeIdentity4x4();
	rotate.m[0][0] = std::cos(radian);
	rotate.m[0][1] = std::sin(radian);
	rotate.m[1][0] = -std::sin(radian);
	rotate.m[1][1] = std::cos(radian);

	return rotate;
}

Matrix4x4 MakeRotateMatrix(const Vector3& rotate)
{
	Matrix4x4 rotateMatrix = MakeIdentity4x4();
	Matrix4x4 rotateXMatrix = MakeRotateXMatrix(rotate.x);
	Matrix4x4 rotateYMatrix = MakeRotateYMatrix(rotate.y);
	Matrix4x4 rotateZMatrix = MakeRotateZMatrix(rotate.z);
	rotateMatrix = Multiply(rotateXMatrix, Multiply(rotateYMatrix, rotateZMatrix));

	return rotateMatrix;
}

Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate)
{
	Matrix4x4 matrix;
	matrix = MakeScaleMatrix(scale);
	matrix = Multiply(matrix, MakeRotateMatrix(rotate));
	matrix = Multiply(matrix, MakeTranslateMatrix(translate));
	return matrix;
}

//透視投影行列
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip)
{
	Matrix4x4 matrix = MakeIdentity4x4();
	matrix.m[0][0] = (1.0f / aspectRatio) * 1.0f / (std::tan(fovY / 2.0f));
	matrix.m[1][1] = 1.0f / (std::tan(fovY / 2.0f));
	matrix.m[2][2] = farClip / (farClip - nearClip);
	matrix.m[2][3] = 1.0f;
	matrix.m[3][2] = (-nearClip * farClip) / (farClip - nearClip);
	matrix.m[3][3] = 0.0f;
	return matrix;
}

//正射影行列
Matrix4x4 MakeOrthographicMatrix(float l, float t, float r, float b, float zn, float zf)
{
	Matrix4x4 matrix = MakeIdentity4x4();
	matrix.m[0][0] = 2.0f / (r - l);
	matrix.m[1][1] = 2.0f / (t - b);
	matrix.m[2][2] = 1.0f / (zf - zn);
	matrix.m[3][0] = (l + r) / (l - r);
	matrix.m[3][1] = (t + b) / (b - t);
	matrix.m[3][2] = zn / (zn - zf);

	return matrix;
}

//ビューポート変換行列
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minD, float maxD)
{
	Matrix4x4 matrix = MakeIdentity4x4();
	matrix.m[0][0] = width / 2.0f;
	matrix.m[1][1] = -height / 2.0f;
	matrix.m[2][2] = maxD - minD;
	matrix.m[3][0] = left + width / 2.0f;
	matrix.m[3][1] = top + height / 2.0f;
	matrix.m[3][2] = minD;

	return matrix;
}

