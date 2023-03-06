#ifndef UTILITYFUNCTIONS_H
#define UTILITYFUNCTIONS_H
#include <cassert>
#include <random>
#include <commdlg.h>
#include <string>
//NO ELRIC, BAD ELRIC
#define maybe Catbox::GetRandom(0,2)

namespace Catbox
{
	static std::wstring ToWString(std::string aString)
	{
		return std::wstring(aString.begin(), aString.end());
	}

	static std::string ToLowerString(std::string aString) 
	{
		std::string returnString = aString;
		for (size_t i = 0; i < returnString.size(); i++)
		{
			returnString[i] = std::tolower(returnString[i]);
		}
		return returnString;
	}

	static int StringToInt(std::string aString) 
	{
		int aReturnInt = 0;
		for (int i = 0; i < aString.size(); i++)
		{
			aReturnInt += (int)aString[i];
			aReturnInt += (aReturnInt) * (10 * (i + 1));
		}
		return aReturnInt;
	}

	static bool CheckText(const char* aText, UINT aSize)
	{
		if (aText[aSize] != '\0')
		{
			return false;
		}
		return true;
	}

	template<class T> 
	static bool LowerThan(T& firstThing, T& anotherThing) 
	{
		return (firstThing < anotherThing)
	}


	static std::string OpenFile(const char* aFilter) 
	{
		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = GetActiveWindow();
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = aFilter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		if (GetOpenFileNameA(&ofn))
		{
			std::string fullPath = ofn.lpstrFile;
			std::size_t pos = fullPath.find("Bin");
			std::string relativePath = fullPath.substr(pos + 4);

			return relativePath;
		} 
		return std::string(); //operation canceled
	}

	static std::string SaveFile(const char* aFilter)
	{
		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = GetActiveWindow();
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = aFilter;
		ofn.nFilterIndex = 1;
		ofn.Flags =  OFN_NOCHANGEDIR;

		ofn.lpstrDefExt = strchr(aFilter, '\0') + 1;
		
		if (GetSaveFileNameA(&ofn))
		{
			return ofn.lpstrFile;
		} 
		return std::string(); //operation canceled
	}

	static std::string GetNameFromPath(const std::string& aPath)
	{
		size_t indexOfPeriod = aPath.find_last_of('.');
		if (indexOfPeriod != std::string::npos)
		{
			size_t indexOfSlash = aPath.find_last_of('/') + 1;
			if (indexOfSlash == 0)
			{
				indexOfSlash = aPath.find_last_of('\\') + 1;
			}
			return aPath.substr(indexOfSlash, indexOfPeriod - indexOfSlash);
		}
		return "";
	}

	static std::string GetExtensionFromPath(const std::string& aPath)
	{
		size_t indexOfPeriod = aPath.find_last_of('.');
		if (indexOfPeriod != std::string::npos)
		{
			return aPath.substr(indexOfPeriod, aPath.size() - indexOfPeriod);
		}
		return "";
	}

	static void CreateFileK(std::string& aPath)
	{
		DWORD attributes = GetFileAttributesA((LPCSTR)aPath.c_str());
		if (attributes & FILE_ATTRIBUTE_READONLY)
		{
			SetFileAttributesA((LPCSTR)aPath.c_str(), attributes & ~FILE_ATTRIBUTE_READONLY);
		}

		std::ifstream  src(aPath, std::ios::binary);
		std::ofstream  dst(aPath, std::ios::binary);
		dst << src.rdbuf();
	}

	static void CopyFileK(const std::string& aSource, std::string& aDestination)
	{
		std::string fileName = Catbox::GetNameFromPath(aDestination);
		std::string fileExtension = Catbox::GetExtensionFromPath(aDestination);
		std::string directory = aDestination;
		directory.erase(directory.end() - fileName.size() - fileExtension.size(), directory.end());

		bool success = !std::filesystem::exists(aDestination);
		int num = 2;
		while (!success)
		{
			std::string newName = directory + fileName + std::to_string(num) + fileExtension;

			if (!std::filesystem::exists(newName))
			{
				success = true;
				aDestination = newName;
			}
			++num;
		}

		DWORD attributes = GetFileAttributesA((LPCSTR)aDestination.c_str());
		if (attributes & FILE_ATTRIBUTE_READONLY)
		{
			bool success = SetFileAttributesA((LPCSTR)aDestination.c_str(), attributes & ~FILE_ATTRIBUTE_READONLY);
			if (!success)
			{
				auto err = GetLastError();
			}
		}

		std::ifstream  src(aSource, std::ios::binary);
		std::ofstream  dst(aDestination, std::ios::binary);
		dst << src.rdbuf();
	}

	template <typename T>
	T Max(const T aFirst, const T aSecond)
	{
		if (aFirst >= aSecond)
		{
			return(aFirst);
		}
		if (aFirst < aSecond)
		{
			return(aSecond);
		}
	}

	template <typename T>
	T Min(const T aFirst, const T aSecond)
	{
		if (aFirst <= aSecond)
		{
			return(aFirst);
		}
		if (aFirst > aSecond)
		{
			return(aSecond);
		}
	}

	template <typename T>
	T Abs(const T aValue)
	{
		if (aValue < 0)
		{
			return(-aValue);
		}
		return(aValue);
	}

	template <typename T>
	T Clamp(const T aValue, const T aMinValue, const T aMaxValue)
	{
		//printerror(aMinValue <= aMaxValue && "Entered min value was larger than max value!");

		if (aValue < aMinValue)
		{
			return(aMinValue);
		}
		if (aValue > aMaxValue)
		{
			return(aMaxValue);
		}
		return(aValue);
	}

	template <typename T>
	T Lerp(const T aStartValue, const T anEndValue, float aPercentage)
	{
		return(static_cast<T>(aStartValue + aPercentage * (anEndValue - aStartValue)));
	}

	template <typename T>
	T SmoothStep(const T aMin, const T aMax, T aValue)
	{
		aValue = Clamp((aValue - aMin) / (aMax - aMin), 0.0f, 1.0f);

		return aValue * aValue * (3 - 2 * aValue);
	}

	template <typename T>
	void Swap(T& aFirst, T& aSecond)
	{
		T temp = aFirst;
		aFirst = aSecond;
		aSecond = temp;
	}

	template <typename T>
	T GetRandom(T aLowerBound, T aUpperBound)
	{
		float random = (rand()) / (float)RAND_MAX;
		T diff = aUpperBound - aLowerBound;
		T r = static_cast<T>(random * diff);
		return aLowerBound + r;
	}

	static float Pi()
	{
		return 3.14159265359f;
	}

	static float Rad2Deg(float aRadians)
	{
		return static_cast<float>(aRadians * 180.0f / Pi());
	}

	static float Deg2Rad(float aDegrees)
	{
		return aDegrees * Pi() / 180.0f;
	}

	static bool SameSide(Vector3f aPoint, Vector3f anotherPoint, Vector3f A, Vector3f B, float aOffset)
	{
		Vector3f crossA = (B - A).Cross(aPoint - A);
		Vector3f crossB = (B - A).Cross(anotherPoint - A);
		return crossA.Dot(crossB) >= -aOffset;
	}
	
	static bool PointInTriangle(Vector3f aPoint, Vector3f A, Vector3f B, Vector3f C, float aOffset = 2.0f) 
	{
		return (SameSide(aPoint, A, B, C, aOffset) && SameSide(aPoint, B, A, C, aOffset) && SameSide(aPoint, C, A, B, aOffset));
	}

	static float WrapPi(float theta) 
	{
		if (std::fabs(theta) <= Pi()) 
		{
			const float TWOPPI = 2.0f * Pi();
			float revolutions = std::floor((theta + Pi()) * (1.0f / TWOPPI));

			theta -= revolutions * TWOPPI;
		}
	}

	static bool PointInTriangleBarycentric(Vector3f aPoint, Vector3f A, Vector3f B, Vector3f C, float aOffset = 0.0f)
	{
		// Compute vectors        
		Vector3f v0 = C - A;
		Vector3f v1 = B - A;
		Vector3f v2 = aPoint - A;

		// Compute dot products
		float dot00 = v0.Dot(v0);
		float dot01 = v0.Dot(v1);
		float dot02 = v0.Dot(v2);
		float dot11 = v1.Dot(v1);
		float dot12 = v1.Dot(v2);

		// Compute barycentric coordinates
		float invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
		float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
		float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

		// Check if point is in triangle
		//return (u >= 0 ) && (v >= 0) && (u + v < 1);
		// More Lenient Version
		return (u >= 0 - aOffset) && (v >= 0 - aOffset) && (u + v <= 1 + aOffset);
	}


	struct Quaternion
	{
		Quaternion() = default;
		Quaternion(Vector4f aRot) 
		{
			x = aRot.x;
			y = aRot.y;
			z = aRot.z;
			w = aRot.w;
		}
		double w = 0.0f, x = 0, y = 0, z = 0;
		void Normalize() 
		{
			float d = std::sqrt(std::powf(w, 2) + std::powf(x, 2) + std::powf(y, 2) + std::powf(z, 2));
			w = w / d;
			x = x / d;
			y = y / d;
			z = z / d;
		}
		Vector4f GetVector4f() { return Vector4f(x, y, z, w); }
	};

	//static Vector3f ToEulerAngles(Quaternion q) 
	//{
	//	Vector3f angles;

	//	// roll (x-axis rotation)
	//	double sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
	//	double cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
	//	angles.x = std::atan2(sinr_cosp, cosr_cosp);

	//	// pitch (y-axis rotation)
	//	double sinp = std::sqrt(1 + 2 * (q.w * q.y - q.x * q.z));
	//	double cosp = std::sqrt(1 - 2 * (q.w * q.y - q.x * q.z));
	//	angles.y = 2 * std::atan2(sinp, cosp) - 3.14159265359f * 0.5f;

	//	// yaw (z-axis rotation)
	//	double siny_cosp = 2 * (q.w * q.z + q.x * q.y);
	//	double cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
	//	angles.z = std::atan2(siny_cosp, cosy_cosp);

	//	angles.x = Rad2Deg(angles.x);
	//	angles.y = Rad2Deg(angles.y);
	//	angles.z = Rad2Deg(angles.z);

	//	return angles;
	//}

	static Vector3f ToEulerAngles(Quaternion q)
	{
		float sinpitch = -2.0f * (q.y * q.z - q.w * q.x);

		//out angles radians
		//heading, pitch, bank
		//y, x, z
		float h, p, b;

		//assuming object to world quaternion
		if (std::fabsf(sinpitch) > 1 - 0.00001) //check for gimbal lock
		{
			p = 3.14 * 0.5f * sinpitch;
			h = atan2f(-q.x * q.z + q.w * q.y, 0.5f - q.y * q.y - q.z * q.z);
			b = 0.0f;
		}
		else
		{
			p = asinf(sinpitch);
			h = atan2f(q.x * q.z + q.w * q.y, 0.5f - q.x * q.x - q.y * q.y);
			b = atan2f(q.x * q.y + q.w * q.z, 0.5f - q.x * q.x - q.z * q.z);
		}

		return { Rad2Deg(p), Rad2Deg(h), Rad2Deg(b) };
	}

	static Quaternion ToQuaternion(Vector3f aRotation)
	{
		double c1 = cos(Catbox::Deg2Rad(aRotation.x) * 0.5);
		double c2 = cos(Catbox::Deg2Rad(aRotation.y) * 0.5);
		//if (aRotation.y == 90) 
		//{
		//	c2 = cos(Catbox::Deg2Rad(45));
		//}
		//else if (aRotation.y == -90) 
		//{
		//	c2 = cos(Catbox::Deg2Rad(-45));
		//}
		double c3 = cos(Catbox::Deg2Rad(aRotation.z) * 0.5);
		double s1 = sin(Catbox::Deg2Rad(aRotation.x) * 0.5);
		double s2 = sin(Catbox::Deg2Rad(aRotation.y) * 0.5);
		//if (aRotation.y == 90)
		//{
		//	s2 = cos(Catbox::Deg2Rad(45));
		//}
		//else if (aRotation.y == -90)
		//{
		//	s2 = cos(Catbox::Deg2Rad(-45));
		//}
		double s3 = sin(Catbox::Deg2Rad(aRotation.z) * 0.5);



		Quaternion q;
		q.x = s1 * c2 * c3 - c1 * s2 * s3;
		q.y = c1 * s2 * c3 + s1 * c2 * s3;
		q.z = c1 * c2 * s3 - s1 * s2 * c3;
		q.w = c1 * c2 * c3 + s1 * s2 * s3;

		return q;
	}

	static Vector3f SlerpToEuler(Quaternion q1, Quaternion q2, double t)
	{
		Quaternion qr;
		 //Calculate angle between them.
		double cosHalfTheta = q1.w * q2.w + q1.x * q2.x + q1.y * q2.y + q1.z * q2.z;
		if (abs(cosHalfTheta) >= 1.0)
		{
			qr.w = q1.w; qr.x = q1.x; qr.y = q1.y; qr.z = q1.z;
		}
		else 
		{
			if (cosHalfTheta < 0)
			{
				q2.w = -q2.w; q2.x = -q2.x; q2.y = -q2.y; q2.z = q2.z;
				cosHalfTheta = -cosHalfTheta;
			}
			// Calculate temporary values.
			double halfTheta = acos(cosHalfTheta);
			double sinHalfTheta = sqrt(1.0 - cosHalfTheta * cosHalfTheta);
			// if theta = 180 degrees then result is not fully defined
			// we could rotate around any axis normal to qa or qb
			if (fabs(sinHalfTheta) < 0.001) 
			{ // fabs is floating point absolute
				qr.w = (q1.w * 0.5 + q2.w * 0.5);
				qr.x = (q1.x * 0.5 + q2.x * 0.5);
				qr.y = (q1.y * 0.5 + q2.y * 0.5);
				qr.z = (q1.z * 0.5 + q2.z * 0.5);
			}
			else 
			{
				double ratioA = sin((1 - t) * halfTheta) / sinHalfTheta;
				double ratioB = sin(t * halfTheta) / sinHalfTheta;
				//calculate Quaternion.
				qr.w = (q1.w * ratioA + q2.w * ratioB);
				qr.x = (q1.x * ratioA + q2.x * ratioB);
				qr.y = (q1.y * ratioA + q2.y * ratioB);
				qr.z = (q1.z * ratioA + q2.z * ratioB);
			}
		}
		// Compute the "cosine of the angle" between the
		// quaternions, using the dot product


		//float cosOmega = q1.w * q2.w + q1.x * q2.x + q1.y * q2.y + q1.z * q2.z;

		//// If negative dot, negate one of the input
		//// quaternions, to take the shorter 4D "arc"
		//if (cosOmega < 0.0f) {
		//	q2.w = -q2.w;
		//	q2.x = -q2.x;
		//	q2.y = -q2.y;
		//	q2.z = -q2.z;
		//	cosOmega = -cosOmega;
		//}

		//// Check if they are very close together, to protect
		//// against divide-by-zero
		//float k0, k1;
		//if (cosOmega > 0.9999f) {

		//	// Very close - just use linear interpolation
		//	k0 = 1.0f - t;
		//	k1 = t;

		//}
		//else {

		//	// Compute the sin of the angle using the
		//	// trig identity sin^2(omega) + cos^2(omega) = 1
		//	float sinOmega = sqrt(1.0f - cosOmega * cosOmega);

		//	// Compute the angle from its sine and cosine
		//	float omega = atan2(sinOmega, cosOmega);

		//	// Compute inverse of denominator, so we only have
		//	// to divide once
		//	float oneOverSinOmega = 1.0f / sinOmega;

		//	// Compute interpolation parameters
		//	k0 = sin((1.0f - t) * omega) * oneOverSinOmega;
		//	k1 = sin(t * omega) * oneOverSinOmega;
		//}

		//// Interpolate
		//qr.w = q1.w * k0 + q2.w * k1;
		//qr.x = q1.x * k0 + q2.x * k1;
		//qr.y = q1.y * k0 + q2.y * k1;
		//qr.z = q1.z * k0 + q2.z * k1;

		//float dotproduct = q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;
		//float theta = 0, st = 0, sut = 0, sout = 0, coeff1 = 0, coeff2 = 0;

		//// algorithm adapted from Shoemake's paper
		//t = t / 2.0;
		//if (q1.x != 0 || q1.y != 0 || q1.z != 0 || q2.x != 0 || q2.y != 0 || q2.z != 0) 
		//{
		//	theta = (float)acos(dotproduct);
		//	if (theta < 0.0) theta = -theta;

		//	st = (float)sin(theta);
		//	sut = (float)sin(t * theta);
		//	sout = (float)sin((1 - t) * theta);
		//	coeff1 = sout / st;
		//	coeff2 = sut / st;

		//	qr.x = coeff1 * q1.x + coeff2 * q2.x;
		//	qr.y = coeff1 * q1.y + coeff2 * q2.y;
		//	qr.z = coeff1 * q1.z + coeff2 * q2.z;
		//	qr.w = coeff1 * q1.w + coeff2 * q2.w;
		//}


		//// roll (x-axis rotation)
		Vector3f returnVector;
		double sinr_cosp = 2 * (qr.w * qr.x + qr.y * qr.z);
		double cosr_cosp = 1 - 2 * (qr.x * qr.x + qr.y * qr.y);
		returnVector.x = std::atan2(sinr_cosp, cosr_cosp);

		// pitch (y-axis rotation)

		//double sinp = 2.0 * (qr.w * qr.y - qr.z * qr.x);
		//int aSign = 0;
		//aSign = (int(0) < sinp) - (sinp < int(0));
		//if (std::abs(sinp) >= 1) {
		//	returnVector.y = (Catbox::Pi() / 2 * aSign) * 2;
		//}
		//else {
		//returnVector.y = std::asin(sinp);
		//}

		//// yaw (z-axis rotation)
		double siny_cosp = 2 * (qr.w * qr.z + qr.x * qr.y);
		double cosy_cosp = 1 - 2 * (qr.y * qr.y + qr.z * qr.z);
		returnVector.z = std::atan2(siny_cosp, cosy_cosp);

		double sinp = 2.0 * (qr.w * qr.y - qr.z * qr.x);
		int aSign = 0;
		aSign = (int(0) < sinp) - (sinp < int(0));
		if (std::abs(sinp) >= 1) {
			returnVector.y = (Catbox::Pi() / 2 * aSign) * 2;
		}
		else 
		{
			returnVector.y = std::asin(sinp);
		}

		//double sp = -2.0f * (qr.y * qr.z - qr.w * qr.x);

		//if (std::fabs(sp) > 0.9999f) 
		//{
		//	returnVector.y = 1.5570796f * sp;
		//	returnVector.x = std::atan2(-qr.x * qr.z, 0.5f - qr.y * qr.y - qr.z * qr.z);
		//	returnVector.z = 0.0f;
		//}
		//else 
		//{
		//	returnVector.y = std::asin(sp);
		//	returnVector.x = std::atan2(qr.x * qr.z + qr.w * qr.y, 0.5f - qr.x * qr.x - qr.y * qr.y);
		//	returnVector.z = std::atan2(qr.x * qr.y + qr.w * qr.z, 0.5f - qr.x * qr.x - qr.z * qr.z);
		//}

		returnVector.x = Rad2Deg(returnVector.x);
		returnVector.y = Rad2Deg(returnVector.y);
		returnVector.z = Rad2Deg(returnVector.z);


		//Matrix4x4<float> aRotationMatrix;
		//aRotationMatrix(1, 1) = 2 * (qr.w * qr.w + qr.x * qr.x) - 1;
		//aRotationMatrix(1, 2) = 2 * (qr.x * qr.x - qr.w * qr.z);
		//aRotationMatrix(1, 3) = 2 * (qr.y * qr.z + qr.x * qr.y);

		//aRotationMatrix(2, 1) = 2 * (qr.x * qr.y + qr.w * qr.z);
		//aRotationMatrix(2, 2) = 2 * (qr.w * qr.w + qr.y * qr.x) - 1;
		//aRotationMatrix(2, 3) = 2 * (qr.y * qr.z - qr.w * qr.x);

		//aRotationMatrix(3, 1) = 2 * (qr.x * qr.z - qr.w * qr.y);
		//aRotationMatrix(3, 2) = 2 * (qr.y * qr.z + qr.w * qr.x);
		//aRotationMatrix(3, 3) = 2 * (qr.w * qr.w + qr.z * qr.z) - 1;

		return returnVector;
	}
}

#endif