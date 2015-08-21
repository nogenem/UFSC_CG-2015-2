#ifndef WORLD_HPP
#define WORLD_HPP

#include "DisplayFile.hpp"

class World
{
    public:
        World() {}
        virtual ~World() {}

        void addPoint(std::string name, const Coordinate& p);
        void addLine(std::string name, const Coordinates& c);
        void addPolygon(std::string name, bool filled, const Coordinates& c);

        void removeObj(std::string name);
        int numObjs(){ return _objs.size(); }
        Object* getObj(int pos){ return _objs.getObj(pos); }

        void translateObj(std::string name, double dx, double dy);
        void scaleObj(std::string name, double sx, double sy);
        void rotateObj(std::string name, double angle, Coordinate *center);
    protected:
    private:
        DisplayFile _objs;
};

void World::addPoint(std::string name, const Coordinate& p){
    Point *obj = new Point(name, p);
    _objs.addObj(obj);
}

void World::addLine(std::string name, const Coordinates& c){
    Line *obj = new Line(name, c);
    _objs.addObj(obj);
}

void World::addPolygon(std::string name, bool filled, const Coordinates& c){
    Polygon *obj = new Polygon(name, c);
    obj->setFilled(filled);
    _objs.addObj(obj);
}

void World::removeObj(std::string name){
    Object obj(name);
    _objs.removeObj(&obj);
}

void World::translateObj(std::string name, double dx, double dy){
    Object tmp(name);
    Object *obj = _objs.getObj(&tmp);
    obj->transform(Transformation::newTranslation(dx,dy));
}

void World::scaleObj(std::string name, double sx, double sy){
    Object tmp(name);
    Object *obj = _objs.getObj(&tmp);
    obj->transform(Transformation::newScalingAroundObjCenter(sx,sy,obj->center()));
}

void World::rotateObj(std::string name, double angle, Coordinate *center){
    Object tmp(name);
    Object *obj = _objs.getObj(&tmp);
    if(center == NULL){
        Coordinate tmp2 = obj->center();
        center = &tmp2;
    }
    obj->transform(Transformation::newRotationAroundPoint(angle,*center));
}

#endif // WORLD_HPP
