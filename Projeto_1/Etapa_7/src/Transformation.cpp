#include "Transformation.hpp"

Transformation::Transformation(){
    for(int i=0; i<M_SIZE; i++)
        for(int j=0; j<M_SIZE; j++)
            m_matrix[i][j] = 0;
    m_matrix[0][0] = m_matrix[1][1] =
    m_matrix[2][2] = m_matrix[3][3] = 1;
}

Transformation Transformation::newTranslation(double dx, double dy, double dz){
    Matrix m = {{  {1,  0,  0,  0},
                   {0,  1,  0,  0},
                   {0,  0,  1,  0},
                   {dx, dy, dz, 1}  }};
    return Transformation(m);
}

Transformation Transformation::newScaling(double sx, double sy, double sz){
    Matrix m = {{  {sx,  0,  0,  0},
                   {0,  sy,  0,  0},
                   {0,   0,  sz, 0},
                   {0,   0,  0,  1}  }};
    return Transformation(m);
}

Transformation Transformation::newScalingAroundObjCenter(double sx, double sy, double sz,
                                                        const Coordinate& center){
    return newTranslation(-center.x, -center.y, -center.z) *
        newScaling(sx, sy, sz) *
        newTranslation(center.x, center.y, center.z);
}

Transformation Transformation::newRx(double angleX, bool isRad){
    double rad = isRad ? angleX : toRadians(angleX);
    Matrix m = {{   {1, 0,         0,        0},
                    {0, cos(rad),  sin(rad), 0},
                    {0, -sin(rad), cos(rad), 0},
                    {0, 0,         0,        1}    }};
    return Transformation(m);
}

Transformation Transformation::newRy(double angleY, bool isRad){
    double rad = isRad ? angleY : toRadians(angleY);
    Matrix m = {{   {cos(rad), 0, -sin(rad), 0},
                    {0,        1, 0,         0},
                    {sin(rad), 0, cos(rad),  0},
                    {0,        0, 0,         1}    }};
    return Transformation(m);
}

Transformation Transformation::newRz(double angleZ, bool isRad){
    double rad = isRad ? angleZ : toRadians(angleZ);
    Matrix m = {{   {cos(rad),  sin(rad), 0, 0},
                    {-sin(rad), cos(rad), 0, 0},
                    {0,         0,        1, 0},
                    {0,         0,        0, 1}    }};
    return Transformation(m);
}

Transformation Transformation::newRa(double angleA, const Coordinate& p){
    // Poupar algum processamento...
    if(angleA == 0 || (p.x == 0 && p.y == 0 && p.z == 0))
        return Transformation();

    double d = sqrt(p.y*p.y + p.z*p.z);
    double alfa = acos(p.z/d);
    double beta = acos(d);

    return Transformation::newTranslation(-p.x, -p.y, -p.z) * Transformation::newRx(alfa,true) *
        Transformation::newRy(beta,true) * Transformation::newRz(angleA) *
        Transformation::newRy(-beta,true) * Transformation::newRx(-alfa,true) *
        Transformation::newTranslation(p.x, p.y, p.z);
}

Transformation Transformation::newRotation(double angleX, double angleY, double angleZ){
    return newRx(angleX) * newRy(angleY) * newRz(angleZ);
}

Transformation Transformation::newFullRotation(double angleX, double angleY,
                                    double angleZ, double angleA, const Coordinate& p){
    return newRx(angleX) * newRy(angleY) *
        newRz(angleZ) * newRa(angleA, p);
}

Transformation& Transformation::operator*=(const Transformation& t2){
    auto m1 = this->m_matrix;
    const auto &m2 = t2.getM();
    for(int i=0; i<M_SIZE; i++)
        for(int j=0; j<M_SIZE; j++){
            m_matrix[i][j]=0;
            for(int k=0; k<M_SIZE; k++)
                m_matrix[i][j] += m1[i][k] * m2[k][j];
        }

    return *this;
}

Transformation operator*(Transformation t1, const Transformation& t2){
    t1 *= t2;
    return t1;
}

std::ostream& operator<<(std::ostream& os, const Transformation& t){
    const auto &m = t.getM();
    for(int i=0; i<M_SIZE; i++){
        for(int j=0; j<M_SIZE; j++){
            os.width(10);
            os << m[i][j] << " ";
        }
        os << std::endl;
    }
    return os;
}
