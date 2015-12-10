#ifndef VECTOR3_H
#define VECTOR3_H


class Vector3
{
    public:
        Vector3(){}
        Vector3(double _x, double _y, double _z=0.0) :
            x(_x), y(_y), z(_z) {}
        virtual ~Vector3() {}

    public:
        double x = 0.0, y = 0.0, z = 0.0;
};

#endif // VECTOR3_H
