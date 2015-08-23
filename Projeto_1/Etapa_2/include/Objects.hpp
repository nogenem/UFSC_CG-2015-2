#ifndef OBJECTS_H
#define OBJECTS_H

#include <iostream>
#include <string>
#include <vector>
#include "Transformation.hpp"

class Transformation;

class Coordinate
{
    public:
        Coordinate() :
            x(0), y(0), z(1){}
        Coordinate(double cx, double cy) :
            x(cx), y(cy), z(1){}
        virtual ~Coordinate(){};

        Coordinate& operator+=(double step);
        Coordinate& operator-=(double step);
        Coordinate& operator+=(const Coordinate& c);
        Coordinate& operator-=(const Coordinate& c);
        Coordinate& operator*=(const Transformation& t);
        bool operator==(const Coordinate& c){
            return (this->x==c.x && this->y==c.y);
        }

        double x,y,z;
    protected:
    private:
};

typedef std::vector<Coordinate> Coordinates;
enum class ObjType { OBJECT, POINT, LINE, POLYGON };

class Object
{
    public:
        Object(const std::string name) :
            _name(name) {}
        virtual ~Object() {}

        std::string getName(){ return _name; }
        const std::string getName() const { return _name; }

        virtual ObjType getType() const { return ObjType::OBJECT; }
		virtual std::string getTypeName() const { return "Object"; }

        Coordinates& getCoords() {return _coords;}
		const Coordinates& getCoords() const {return _coords;}
        Coordinate& getCoord(int index) { return _coords[index]; }
        const Coordinate& getCoord(int index) const { return _coords[index]; }

        Coordinate center() const;
        void transform(const Transformation& t);

        bool operator==(const Object& other){
            return this->getName() == other.getName();
        }
        Object& operator*(){ return *this; }

		void addCoordinate(double x, double y) {_coords.emplace_back(x,y);}
    protected:
		void addCoordinate(const Coordinate& p) {_coords.push_back(p);}
        void addCoordinate(const Coordinates& coords){
            _coords.insert(_coords.end(), coords.begin(), coords.end());
        }
    private:
        std::string _name;
        Coordinates _coords;
};

class Point : public Object
{
    public:
        Point(std::string name) :
            Object(name) {}
		Point(std::string name, double x, double y) :
            Object(name) { addCoordinate(x,y); }
        Point(std::string name, const Coordinate& p) :
            Object(name) { addCoordinate(p); }

        virtual ObjType getType() const { return ObjType::POINT; }
		virtual std::string getTypeName() const { return "Point"; }
};

class Line : public Object
{
    public:
        Line(std::string name) :
            Object(name) {}
		Line(std::string name, const Coordinates& coords) :
            Object(name) { addCoordinate(coords); }

        virtual ObjType getType() const { return ObjType::LINE; }
		virtual std::string getTypeName() const { return "Line"; }
};

class Polygon : public Object
{
    public:
        Polygon(std::string name) :
            Object(name) { _filled = false; }
		Polygon(std::string name, const Coordinates& coords) :
            Object(name) { _filled = false; addCoordinate(coords); }

        virtual ObjType getType() const { return ObjType::POLYGON; }
		virtual std::string getTypeName() const { return "Polygon"; }

        bool filled(){ return _filled; }
        bool filled() const { return _filled; }
        void setFilled(bool v){ _filled = v; }
    private:
        bool _filled;
};
#endif // OBJECTS_H
