#ifndef OBJECTS_H
#define OBJECTS_H

#include <string>
#include <vector>

class Coordinate
{
    public:
        Coordinate() :
            x(0), y(0){}
        Coordinate(double cx, double cy) :
            x(cx), y(cy){}
        virtual ~Coordinate(){};

        Coordinate& operator+=(double step){
            this->x += step;
            this->y += step;
            return *this;
        }
        Coordinate& operator-=(double step){
            this->x -= step;
            this->y -= step;
            return *this;
        }
        bool operator==(const Coordinate& c){
            return (this->x==c.x && this->y==c.y);
        }

        double x,y;
    protected:
    private:
};

typedef std::vector<Coordinate> Coordinates;
enum ObjType { OBJECT, POINT, LINE, POLYGON };

class Object
{
    public:
        Object(const std::string name) :
            _name(name) {}
        virtual ~Object() {}

        std::string getName(){ return _name; }
        const std::string getName() const { return _name; }

        virtual ObjType type() const { return ObjType::OBJECT; }
		virtual std::string typeName() const { return "Object"; }

        Coordinates& getCoords() {return _coords;}
		const Coordinates& getCoords() const {return _coords;}
        Coordinate& getCoord(int index) { return _coords[index]; }
        const Coordinate& getCoord(int index) const { return _coords[index]; }

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
        virtual ~Point() {}

        virtual ObjType type() const { return ObjType::POINT; }
		virtual std::string typeName() const { return "Point"; }
};

class Line : public Object
{
    public:
        Line(std::string name) :
            Object(name) {}
		Line(std::string name, const Coordinates& coords) :
            Object(name) { addCoordinate(coords); }
        virtual ~Line() {}

        virtual ObjType type() const { return ObjType::LINE; }
		virtual std::string typeName() const { return "Line"; }
};

class Polygon : public Object
{
    public:
        Polygon(std::string name) :
            Object(name) { _filled = false; }
		Polygon(std::string name, const Coordinates& coords) :
            Object(name) { _filled = false; addCoordinate(coords); }
        virtual ~Polygon() {}

        virtual ObjType type() const { return ObjType::POLYGON; }
		virtual std::string typeName() const { return "Polygon"; }

        bool filled(){ return _filled; }
        bool filled() const { return _filled; }
        void setFilled(bool v){ _filled = v; }
    private:
        bool _filled;
};

#endif // OBJECTS_H
