#ifndef TRANSFORMATION_HPP
#define TRANSFORMATION_HPP

#include <cmath>
#include <array>
#include "Objects.hpp"

#define PI 3.1415926535897932384626433832795

#define M_SIZE 3
typedef std::array<std::array<double, M_SIZE>, M_SIZE> Matrix;

class Coordinate;

class Transformation
{
    public:
        Transformation(const double x[M_SIZE][M_SIZE]);
        Transformation(const Matrix m):
            _m(m) {}
        virtual ~Transformation() {}

        const Matrix& getM() const {return _m;}
        Matrix& getM() {return _m;}

        static Transformation newTranslation(double dx, double dy);
		static Transformation newRotationAroundPoint(double theta, const Coordinate& p);
		static Transformation newScalingAroundObjCenter(double sx, double sy, const Coordinate& center);

        static double toRadians(double degrees){ return (PI/180) * degrees;  }

		Transformation& operator*=(const Transformation& t2);
    protected:
    private:
        Matrix _m;

        static Transformation newScaling(double sx, double sy);
        static Transformation newRotation(double theta);
};

Transformation operator*(Transformation t1, const Transformation& t2);
std::ostream& operator<<(std::ostream& os, const Transformation& t);

#endif // TRANSFORMATION_HPP
