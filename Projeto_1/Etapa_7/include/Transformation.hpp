#ifndef TRANSFORMATION_HPP
#define TRANSFORMATION_HPP

#include <cmath>
#include <array>
#include "Objects.hpp"

#define PI 3.1415926535897932384626433832795

#define M_SIZE 4
typedef std::array<std::array<double, M_SIZE>, M_SIZE> Matrix;

class Coordinate;

class Transformation
{
    public:
        Transformation(const Matrix m):
            m_matrix(m) {}
        Transformation();
        virtual ~Transformation() {}

        const Matrix& getM() const {return m_matrix;}
        Matrix& getM() {return m_matrix;}

		Transformation& operator*=(const Transformation& t2);

        static Transformation newTranslation(double dx, double dy, double dz);

        static Transformation newRx(double angleX);
        static Transformation newRy(double angleY);
        static Transformation newRz(double angleZ);
        static Transformation newRotation(double angleX, double angleY, double angleZ);
		static Transformation newRotationAroundPoint(double angleX, double angleY,
                                               double angleZ, const Coordinate& p);

        static Transformation newScaling(double sx, double sy, double sz);
		static Transformation newScalingAroundObjCenter(double sx, double sy, double sz,
                                                  const Coordinate& center);

		static double toRadians(double degrees) { return (PI/180) * degrees; }

    private:
        Matrix m_matrix;
};

Transformation operator*(Transformation t1, const Transformation& t2);
std::ostream& operator<<(std::ostream& os, const Transformation& t);

#endif // TRANSFORMATION_HPP
