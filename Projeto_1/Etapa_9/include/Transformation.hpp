#ifndef TRANSFORMATION_HPP
#define TRANSFORMATION_HPP

#include <cmath>
#include <array>
#include "Objects.hpp"

#define PI 3.1415926535897932384626433832795

#define M_SIZE 4

template<class type, int n, int m>
using Matrix = std::array<std::array<type, m>, n>;

// Transformation Matrix
typedef Matrix<double, M_SIZE, M_SIZE> tMatrix4x4;

class Coordinate;

class Transformation
{
    public:
        Transformation(const tMatrix4x4 m):
            m_matrix(std::move(m)) {}
        Transformation();
        virtual ~Transformation() {}

        const tMatrix4x4& getM() const {return m_matrix;}
        tMatrix4x4& getM() {return m_matrix;}

		Transformation& operator*=(const Transformation& t2);

        static Transformation newTranslation(double dx, double dy, double dz);

        static Transformation newRx(double angleX, bool isRad=false);
        static Transformation newRy(double angleY, bool isRad=false);
        static Transformation newRz(double angleZ, bool isRad=false);
        // Rotação sobre eixo arbitrario
        static Transformation newRa(double angleA, const Coordinate& p);

        static Transformation newRotation(double angleX, double angleY, double angleZ);
		static Transformation newFullRotation(double angleX, double angleY,
                                    double angleZ, double angleA, const Coordinate& p);

        static Transformation newScaling(double sx, double sy, double sz);
		static Transformation newScalingAroundObjCenter(double sx, double sy, double sz,
                                                  const Coordinate& center);

		static double toRadians(double degrees) { return (PI/180) * degrees; }

    private:
        tMatrix4x4 m_matrix;
};

Transformation operator*(Transformation t1, const Transformation& t2);
std::ostream& operator<<(std::ostream& os, const Transformation& t);

#endif // TRANSFORMATION_HPP
