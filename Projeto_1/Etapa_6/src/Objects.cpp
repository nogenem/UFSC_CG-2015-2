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

Coordinate& Coordinate::operator+=(const Coordinate& c){
    this->x += c.x;
    this->y += c.y;
    return *this;
}

Coordinate& Coordinate::operator-=(const Coordinate& c){
    this->x -= c.x;
    this->y -= c.y;
    return *this;
}

Coordinate operator-(const Coordinate& c1, const Coordinate& c2){
    return Coordinate(c1.x - c2.x, c1.y - c2.y);
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
    Coordinate c;
    int n = m_coords.size();

    for(auto &p : m_coords){
        c.x += p.x;
        c.y += p.y;
    }

    c.x /= n;
    c.y /= n;
    return c;
}

Coordinate Object::nCenter() const{
    Coordinate c;
    int n = m_nCoords.size();

    for(auto &p : m_nCoords){
        c.x += p.x;
        c.y += p.y;
    }

    c.x /= n;
    c.y /= n;
    return c;
}

void Object::transform(const Transformation& t){
    //std::cout << t << "\n";
    for(auto &p : m_coords)
        p *= (t);
}

void Object::transformNormalized(const Transformation& t){
    if(m_nCoords.size() > 0)
        m_nCoords.clear();
    for(auto p : m_coords)
        m_nCoords.push_back( (p *= t) );
}

void Object::setNCoord(const Coordinates& c){
    m_nCoords.clear();
    m_nCoords.insert(m_nCoords.end(), c.begin(), c.end());
}

void Curve::generateCurve(){
    if(m_controlPoints.size() != 0)
        return;

    setControlPoints(m_coords);
    const auto& coords = m_controlPoints;

    int numCurves = ((coords.size()-4)/3) + 1;
    m_coords.clear();

    for(int i = 0; i<numCurves; i++){

        // Bleding Function
        for(float t = 0; t<1; t += m_step){
            double t2 = t * t;
            double t3 = t2 * t;

            double x,y;
            x = (-t3 +3*t2 -3*t +1)*coords[i*3+0].x + (3*t3 -6*t2 +3*t)*coords[i*3+1].x +
                (-3*t3 +3*t2)*coords[i*3+2].x + (t3)*coords[i*3+3].x;
            y = (-t3 +3*t2 -3*t +1)*coords[i*3+0].y + (3*t3 -6*t2 +3*t)*coords[i*3+1].y +
                (-3*t3 +3*t2)*coords[i*3+2].y + (t3)*coords[i*3+3].y;

            /*x = (2*t3 -3*t2 +1)*coords[i*3+0].x + (-2*t3 +3*t2)*coords[i*3+3].x +
                (t3 -2*t2 +t)*coords[i*3+1].x + (t3 -t2)*coords[i*3+2].x;
            y = (2*t3 -3*t2 +1)*coords[i*3+0].y + (-2*t3 +3*t2)*coords[i*3+3].y +
                (t3 -2*t2 +t)*coords[i*3+1].y + (t3 -t2)*coords[i*3+2].y;*/

            m_coords.emplace_back(x,y);
        }
    }
}
