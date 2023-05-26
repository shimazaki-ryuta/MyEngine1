#pragma once
#include "matrix.h"
#include "Vector3.h"
#include <math.h>
#include <assert.h>
#include <cmath>
/*

Matrix4x4 Add(Matrix4x4 matrix1, Matrix4x4 matrix2);

Matrix4x4 Subtract(Matrix4x4 matrix1, Matrix4x4 matrix2);

Matrix4x4 Multiply(Matrix4x4 matrix1, Matrix4x4 matrix2);

Matrix4x4 Inverse(Matrix4x4 matrix);

Matrix4x4 Transpose(Matrix4x4 matrix);

Matrix4x4 Scalar(double k, Matrix4x4 matrix);
void MatrixScreenPrintf(int x, int y, const Matrix4x4& matrix);
*/
/*template<typename MatrixSize>
int ColumnSize(MatrixSize m)
{
	return sizeof(m.m[0]) / sizeof(int);
}

template<typename MatrixSize>
int RowSize(MatrixSize m)
{
	return sizeof(m.m) / sizeof(m.m[0]);
}*/

Matrix4x4 Add(Matrix4x4 m1, Matrix4x4 m2);


Matrix4x4 Subtract(Matrix4x4 m1, Matrix4x4 m2);


Matrix4x4 Multiply(Matrix4x4 m1, Matrix4x4 m2);


Matrix4x4 Scalar(double k, Matrix4x4 m);
auto CofactorExpansion(Matrix2x2 m);
auto CofactorExpansion(Matrix3x3 m);


double CofactorExpansion(Matrix4x4 m);


Matrix4x4 Transpose(Matrix4x4 m);

Matrix3x3 Transpose(Matrix3x3 m);

Matrix2x2 Transpose(Matrix2x2 m);

Matrix4x4 Adjoint(Matrix4x4 m);
Matrix3x3 Adjoint(Matrix3x3 m);
Matrix2x2 Adjoint(Matrix2x2 m);


Matrix4x4 Inverse(Matrix4x4 m);

Matrix4x4 MakeIdentity4x4();

//アフィン変換関係

Matrix4x4 MakeTranslateMatrix(const Vector3& translate);

Matrix4x4 MakeScaleMatrix(const Vector3& scale);

Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);

Matrix4x4 MakeRotateXMatrix(float radian);

Matrix4x4 MakeRotateYMatrix(float radian);
Matrix4x4 MakeRotateZMatrix(float radian);

Matrix4x4 MakeRotateMatrix(const Vector3& rotate);

Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

//透視投影行列
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);

//正射影行列
Matrix4x4 MakeOrthographicMatrix(float l, float t, float r, float b, float zn, float zf);

//ビューポート変換行列
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minD, float maxD);
