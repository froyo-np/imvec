#pragma once
#include "imvec.hpp"
#ifndef PI
#define PI 3.14159
#endif
namespace imvec {
	template <typename T, unsigned int N>
	class mat {
	protected:
		vec<T, N> cols[N];
	public:
		mat() {
			// a lot of work for a templated identity matrix...
			for (unsigned int i = 0; i < N; i++) {
				T vals[N];
				for (unsigned int j = 0; j < N; j++) {
					vals[j] = i == j ? (T)1 : (T)0;
				}
				cols[i] = vec<T, N>(vals);
			}
		}
		mat(vec<T, N> columns[N]) {
			for (unsigned int i = 0; i < N; i++) {
				cols[i] = columns[i];
			}
		}
		mat(T colmajorArray[N * N]) {
			T tmp[N];
			for (unsigned int c = 0; c < N; c++) {
				for (unsigned int r = 0; r < N; r++) {
					unsigned int i = (c * N) + r;
					tmp[r] = colmajorArray[i];
				}
				cols[c] = vec<T, N>(tmp);
			}
		}
		// translation!
		mat(const vec<T, N - 1> translation) : mat() {
			T tmp[N];
			for (unsigned int i = 0; i < N - 1; i++) {
				tmp[i] = translation[i];
			}
			tmp[N - 1] = (T)1;
			cols[N - 1] = vec<T, N>(tmp);
		}
		// scale!
		mat(T sX, T sY) {
			for (unsigned int i = 0; i < N; i++) {
				T vals[N];
				T s = i == 0 ? sX : (i == 1 ? sY : 1);
				for (unsigned int j = 0; j < N; j++) {
					vals[j] = s * (i == j ? (T)1 : (T)0);
				}
				cols[i] = vec<T, N>(vals);
			}
		}
		vec<T, N> row(unsigned int r) const {
			T vals[N];
			for (unsigned int c = 0; c < N; c++) {
				vals[c] = cols[c][r];
			}
			return vec<T, N>(vals);
		}
		mat<T, N> operator*(const mat<T, N>& rhs)const {
			vec<T, N> columns[N];
			T vals[N];
			for (unsigned int col = 0; col < N; col++) {
				vec<T, N> C = this->cols[col];
				for (unsigned int row = 0; row < N; row++) {
					vals[row] = C.dot(rhs.row(row));
				}
				columns[col] = vec<T, N>(vals); // uh maybe?
			}
			return mat<T, N>(columns);
		}
		mat<T, N> operator*(const T scalar)const {
			vec<T, N> columns[N];
			T vals[N];
			for (unsigned int col = 0; col < N; col++) {
				columns[col] = this->cols[col] * scalar;
			}
			return mat<T, N>(columns);
		}
		mat<T, N> operator+(const mat<T, N>& rhs)const {
			vec<T, N> columns[N];
			for (unsigned int col = 0; col < N; col++) {
				vec<T, N> C = this->cols[col];
				vec<T, N> O = rhs.cols[col];
				columns[col] = C + O;
			}
			return mat<T, N>(columns);
		}
		vec<T, N> operator[](unsigned int i) {
			return cols[i];
		}
		// vector mult!
		vec<T, N> operator*(const vec<T, N>& rhs)const {
			vec<T, N> columns[N];
			T vals[N];
			for (unsigned int col = 0; col < N; col++) {
				vec<T, N> C = this->row(col);
				vals[col] = C.dot(rhs);
			}
			return vec<T, N>(vals);
		}
		
		bool operator==(const mat<T, N> rhs) const {
			for (unsigned int i = 0; i < N; i++) {
				if (cols[i] != rhs.cols[i]) {
					return false;
				}
			}
			return true;
		}
		void toColumnMajorArray(T* t) const {
			T* ptr = t;
			for (unsigned int col = 0; col < N; col++) {
				for (unsigned int row = 0; row < N; row++) {
					*ptr = cols[col][row];
					++ptr;
				}
			}
		}
	};

	template <typename T>
	mat<T, 4> invert(const mat<T, 4>& M, bool* success) {
		T inv[16], det;
		T m[16];
		M.toColumnMajorArray(m);
		int i;

		inv[0] = m[5] * m[10] * m[15] -
			m[5] * m[11] * m[14] -
			m[9] * m[6] * m[15] +
			m[9] * m[7] * m[14] +
			m[13] * m[6] * m[11] -
			m[13] * m[7] * m[10];

		inv[4] = -m[4] * m[10] * m[15] +
			m[4] * m[11] * m[14] +
			m[8] * m[6] * m[15] -
			m[8] * m[7] * m[14] -
			m[12] * m[6] * m[11] +
			m[12] * m[7] * m[10];

		inv[8] = m[4] * m[9] * m[15] -
			m[4] * m[11] * m[13] -
			m[8] * m[5] * m[15] +
			m[8] * m[7] * m[13] +
			m[12] * m[5] * m[11] -
			m[12] * m[7] * m[9];

		inv[12] = -m[4] * m[9] * m[14] +
			m[4] * m[10] * m[13] +
			m[8] * m[5] * m[14] -
			m[8] * m[6] * m[13] -
			m[12] * m[5] * m[10] +
			m[12] * m[6] * m[9];

		inv[1] = -m[1] * m[10] * m[15] +
			m[1] * m[11] * m[14] +
			m[9] * m[2] * m[15] -
			m[9] * m[3] * m[14] -
			m[13] * m[2] * m[11] +
			m[13] * m[3] * m[10];

		inv[5] = m[0] * m[10] * m[15] -
			m[0] * m[11] * m[14] -
			m[8] * m[2] * m[15] +
			m[8] * m[3] * m[14] +
			m[12] * m[2] * m[11] -
			m[12] * m[3] * m[10];

		inv[9] = -m[0] * m[9] * m[15] +
			m[0] * m[11] * m[13] +
			m[8] * m[1] * m[15] -
			m[8] * m[3] * m[13] -
			m[12] * m[1] * m[11] +
			m[12] * m[3] * m[9];

		inv[13] = m[0] * m[9] * m[14] -
			m[0] * m[10] * m[13] -
			m[8] * m[1] * m[14] +
			m[8] * m[2] * m[13] +
			m[12] * m[1] * m[10] -
			m[12] * m[2] * m[9];

		inv[2] = m[1] * m[6] * m[15] -
			m[1] * m[7] * m[14] -
			m[5] * m[2] * m[15] +
			m[5] * m[3] * m[14] +
			m[13] * m[2] * m[7] -
			m[13] * m[3] * m[6];

		inv[6] = -m[0] * m[6] * m[15] +
			m[0] * m[7] * m[14] +
			m[4] * m[2] * m[15] -
			m[4] * m[3] * m[14] -
			m[12] * m[2] * m[7] +
			m[12] * m[3] * m[6];

		inv[10] = m[0] * m[5] * m[15] -
			m[0] * m[7] * m[13] -
			m[4] * m[1] * m[15] +
			m[4] * m[3] * m[13] +
			m[12] * m[1] * m[7] -
			m[12] * m[3] * m[5];

		inv[14] = -m[0] * m[5] * m[14] +
			m[0] * m[6] * m[13] +
			m[4] * m[1] * m[14] -
			m[4] * m[2] * m[13] -
			m[12] * m[1] * m[6] +
			m[12] * m[2] * m[5];

		inv[3] = -m[1] * m[6] * m[11] +
			m[1] * m[7] * m[10] +
			m[5] * m[2] * m[11] -
			m[5] * m[3] * m[10] -
			m[9] * m[2] * m[7] +
			m[9] * m[3] * m[6];

		inv[7] = m[0] * m[6] * m[11] -
			m[0] * m[7] * m[10] -
			m[4] * m[2] * m[11] +
			m[4] * m[3] * m[10] +
			m[8] * m[2] * m[7] -
			m[8] * m[3] * m[6];

		inv[11] = -m[0] * m[5] * m[11] +
			m[0] * m[7] * m[9] +
			m[4] * m[1] * m[11] -
			m[4] * m[3] * m[9] -
			m[8] * m[1] * m[7] +
			m[8] * m[3] * m[5];

		inv[15] = m[0] * m[5] * m[10] -
			m[0] * m[6] * m[9] -
			m[4] * m[1] * m[10] +
			m[4] * m[2] * m[9] +
			m[8] * m[1] * m[6] -
			m[8] * m[2] * m[5];

		det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

		if (det == 0) {
			return mat<T, 4>();
			*success = false;
		}
		*success = true;
		det = 1.0 / det;
		return mat<T, 4>(inv) * det;
	}
	template <typename T>
	mat<T, 3> rotateAboutPointZ(vec<T, 2> center, float radians) {
		using fvec3 = vec<float, 3>;
		mat<T, 3> move(center * -1);
		mat<T, 3> back(center);
		vec<T, 2> xDir(cos(radians), sin(radians));
		vec<T, 2> yDir(cos(radians + PI / 2), sin(radians + PI / 2));
		fvec3 rtmp[3] = { fvec3(xDir.x(),xDir.y(),0.0f), fvec3(yDir.x(),yDir.y(),0.0f), fvec3(0.0f, 0.0f, 1.0f) };
		mat<T, 3> rot(rtmp);
		return move * rot * back;
	}
	template <typename T>
	mat<T, 3> rotateAboutAxis(vec<T, 3> axis, T radians) {
		// thanks wikipedia:https://en.wikipedia.org/wiki/Transformation_matrix#Rotation_2
		T sinTheta = sin(radians);
		T cosTheta = cos(radians);
		T x = axis.x();
		T y = axis.y();
		T z = axis.z();
		T ll = x * x;
		T lm = x * y;
		T ln = x * z;
		T ml = lm;
		T nl = ln;
		T mn = y * z;
		T mm = y * y;
		T nn = z * z;
		T oneCosTheta = 1 - cosTheta;
		vec<T, 3> X(
			ll * (oneCosTheta)+cosTheta,
			lm * (oneCosTheta)+(z * sinTheta),
			ln * (oneCosTheta)-(y * sinTheta));
		vec<T, 3> Y(
			ml * oneCosTheta - (z * sinTheta),
			mm * oneCosTheta + cosTheta,
			mn * oneCosTheta + (x * sinTheta));
		vec<T, 3> Z(
			nl * oneCosTheta + (y * sinTheta),
			mn * oneCosTheta - (x * sinTheta),
			nn * oneCosTheta + cosTheta
		);
		vec<T, 3> cols[3] = { X,Y,Z };
		return mat<T, 3>(cols);
	}
	template <typename T>
	mat<T, 4> rotateAboutAxis4(vec<T, 3> axis, T radians) {
		// thanks wikipedia:https://en.wikipedia.org/wiki/Transformation_matrix#Rotation_2
		T sinTheta = sin(radians);
		T cosTheta = cos(radians);
		T x = axis.x();
		T y = axis.y();
		T z = axis.z();
		T ll = x * x;
		T lm = x * y;
		T ln = x * z;
		T mn = y * z;
		T mm = y * y;
		T nn = z * z;
		T oneCosTheta = 1 - cosTheta;
		vec<T, 4> X(
			ll * (oneCosTheta)+cosTheta,
			lm * (oneCosTheta)+(z * sinTheta),
			ln * (oneCosTheta)-(y * sinTheta), 0);
		vec<T, 4> Y(
			lm * oneCosTheta - (z * sinTheta),
			mm * oneCosTheta + cosTheta,
			mn * oneCosTheta + (x * sinTheta), 0);
		vec<T, 4> Z(
			ln * oneCosTheta + (y * sinTheta),
			mn * oneCosTheta - (x * sinTheta),
			nn * oneCosTheta + cosTheta, 0
		);
		vec<T, 4> W(0, 0, 0, 1);
		vec<T, 4> cols[4] = { X,Y,Z, W };
		return mat<T, 4>(cols);
	}
	template <typename T>
	mat<T, 3> rotateAndScaleAboutPointZ(vec<T, 2> center, float radians, vec<T, 3> scale) {
		using fvec3 = vec<float, 3>;
		mat<T, 3> move(center * -1);
		mat<T, 3> back(center);
		mat<T, 3> S(scale.x(), scale.y());
		vec<T, 2> xDir(cos(radians), sin(radians));
		vec<T, 2> yDir(cos(radians + PI / 2), sin(radians + PI / 2));

		fvec3 rtmp[3] = { fvec3(xDir.x(),xDir.y(),0.0f), fvec3(yDir.x(),yDir.y(),0.0f), fvec3(0.0f, 0.0f, 1.0f) };
		mat<T, 3> rot(rtmp);
		return move * S * rot * back;
	}

	// TODO: not usre this is right... 
	template <typename T>
	mat<T, 4> frustum(vec<T, 2> leftBottom, vec<T, 2> rightTop, T zNear, T zFar) {
		using vec4 = vec<T, 4>;
		using vec3 = vec<T, 3>;
		float tmp;
		tmp = 2 * zNear;
		vec3 diff = vec3(rightTop - leftBottom, zFar - zNear);
		vec4 xc = vec4(tmp / diff.x(), 0, 0, 0);
		vec4 yc = vec4(0, tmp / diff.y(), 0, 0);
		vec4 zc = vec4((vec3(leftBottom, -zNear) + vec3(rightTop, -zFar)) / diff, -1);
		vec4 wc = vec4(0, 0, (-tmp * zFar) / diff.z(), 0.0);
		vec4 clms[4] = { xc,yc,zc,wc };
		mat<T, 4> yay(clms);
		return yay;
	}
	template <typename T>
	mat<T, 4> frustum(T w, T h, T zNear, T zFar) {
		// symetric frustum :=> r+l= 0, t+b=0,
		// dX = 2w,
		// dY = 2h
		using vec4 = vec<T, 4>;
		using vec3 = vec<T, 3>;
		float zDiff = zFar - zNear;
		vec4 xc = vec4(-zNear / w, 0, 0, 0);
		vec4 yc = vec4(0, -zNear / h, 0, 0);
		vec4 zc = vec4(0, 0, (-1.0 * (zFar + zNear)) / zDiff, -1);
		vec4 wc = vec4(0, 0, (-2.0 * zFar * zNear) / zDiff, 0.0);
		vec4 clms[4] = { xc,yc,zc,wc };
		mat<T, 4> yay(clms);
		return yay;
	}
	template <typename T>
	mat<T, 4> ortho(vec<T, 3> minCorner, vec<T, 3> maxCorner) {
		using vec4 = vec<T, 4>;
		using vec3 = vec<T, 3>;
		vec3 diff = maxCorner - minCorner;
		vec4 xc(2 / diff.x(), 0, 0, 0);
		vec4 yc(0, 2 / diff.y(), 0, 0);
		vec4 zc(0, 0, 2 / diff.z(), 0);
		vec4 wc((minCorner + maxCorner) / diff, 1.0);
		vec4 clms[4] = { xc,yc,zc,wc };
		mat<T, 4> yay(clms);
		return yay;
	}

};