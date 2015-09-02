#ifndef OBJECTS_H
#define OBJECTS_H

#include <gtk/gtk.h>
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
            _name(name), _color({0}) {}
        Object(const std::string name, GdkRGBA color) :
            _name(name), _color(color) {}
        virtual ~Object() {}

        std::string getName(){ return _name; }
        const std::string getName() const { return _name; }

        GdkRGBA& getColor(){ return _color; }

        virtual ObjType getType() const { return ObjType::OBJECT; }
		virtual std::string getTypeName() const { return "Object"; }

        Coordinates& getCoords() {return _coords;}
        Coordinate& getCoord(int index) { return _coords[index]; }
        int getCoordsSize(){ return _coords.size(); }

        Coordinates& getNCoords() {return _nCoords;}
		Coordinate& getNCoord(int index) { return _nCoords[index]; }
		int getNCoordsSize(){ return _nCoords.size(); }

        Coordinate center() const;
        Coordinate nCenter() const;
        void transform(const Transformation& t);
        void transformNormalized(const Transformation& t);

        bool operator==(const Object& other){
            return this->getName() == other.getName();
        }
        Object& operator*(){ return *this; }

		void addCoordinate(double x, double y) {_coords.emplace_back(x,y);}
		void addCoordinate(const Coordinate& p) {_coords.push_back(p);}
    protected:
        std::string _name;
        GdkRGBA _color;
        Coordinates _coords;
        Coordinates _nCoords; // Coordenadas normalizadadas

        void addCoordinate(const Coordinates& coords){
            _coords.insert(_coords.end(), coords.begin(), coords.end());
        }
};

class Point : public Object
{
    public:
        Point(std::string name) :
            Object(name) {}
        Point(std::string name, GdkRGBA color) :
            Object(name,color) {}
		Point(std::string name, GdkRGBA color, double x, double y) :
            Object(name,color) { addCoordinate(x,y); }
        Point(std::string name, GdkRGBA color, const Coordinate& p) :
            Object(name,color) { addCoordinate(p); }

        virtual ObjType getType() const { return ObjType::POINT; }
		virtual std::string getTypeName() const { return "Point"; }
};

class Line : public Object
{
    public:
        Line(std::string name) :
            Object(name) {}
        Line(std::string name, GdkRGBA color) :
            Object(name,color) {}
		Line(std::string name, GdkRGBA color, const Coordinates& coords) :
            Object(name,color) { addCoordinate(coords); }

        virtual ObjType getType() const { return ObjType::LINE; }
		virtual std::string getTypeName() const { return "Line"; }
};

class Polygon : public Object
{
    public:
        Polygon(std::string name) :
            Object(name) { _filled = false; }
        Polygon(std::string name, GdkRGBA color) :
            Object(name,color) { _filled = false; }
		Polygon(std::string name, GdkRGBA color, const Coordinates& coords) :
            Object(name,color) { _filled = false; addCoordinate(coords); }
        Polygon(std::string name, GdkRGBA color, bool filled, const Coordinates& coords) :
            Object(name,color) { _filled = filled; addCoordinate(coords); }

        virtual ObjType getType() const { return ObjType::POLYGON; }
		virtual std::string getTypeName() const { return "Polygon"; }

        bool filled() const { return _filled; }
        void setFilled(bool v){ _filled = v; }
    private:
        bool _filled;
};
#endif // OBJECTS_H
