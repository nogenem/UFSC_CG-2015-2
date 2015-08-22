#include "Objects.hpp"

Coordinate& Coordinate::operator+=(double step){
    this->x += step;
    this->y += step;
    return *this;
}

Coordinate& Coordinate::operator-=(double step){
    this->x -= step;
    this->y -= step;
    return *this;
}

Coordinate& Coordinate::operator*=(const Transformation& t){
    const auto &m = t.getM();
    double x = this->x, y = this->y, z = this->z;
    this->x = x*m[0][0] + y*m[1][0] + z*m[2][0];
    this->y = x*m[0][1] + y*m[1][1] + z*m[2][1];
    this->z = x*m[0][2] + y*m[1][2] + z*m[2][2];
    return *this;
}

Coordinate Object::center() const{
    Coordinate c(0, 0);
    int n = _coords.size();

    for(auto &p : _coords){
        c.x += p.x;
        c.y += p.y;
    }

    c.x /= n;
    c.y /= n;
    return c;
}

void Object::transform(const Transformation& t){
    //std::cout << t << "\n";
    for(auto &p : _coords)
        p *= (t);
}
