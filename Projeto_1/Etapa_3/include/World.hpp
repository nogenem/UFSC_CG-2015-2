#ifndef WORLD_HPP
#define WORLD_HPP

#include "DisplayFile.hpp"
#include "Dialogs.hpp"

class World
{
    public:
        World() {}
        virtual ~World() {}

        Object* addPoint(std::string name, GdkRGBA color, const Coordinate& p);
        Object* addLine(std::string name, GdkRGBA color, const Coordinates& c);
        Object* addPolygon(std::string name, GdkRGBA color, bool filled, const Coordinates& c);
        void addObj(Object *obj){ _objs.addObj(obj); }

        void removeObj(std::string name);
        int numObjs(){ return _objs.size(); }
        Object* getObj(int pos){ return _objs.getObj(pos); }
        Object* getObj(std::string name);

        Object* translateObj(std::string objName, double dx, double dy);
        Object* scaleObj(std::string objName, double sx, double sy);
        // p = ponto central para rodar;
        Object* rotateObj(std::string objName, double angle, const Coordinate& p,
                       rotateType type);
    protected:
    private:
        DisplayFile _objs;
};

Object* World::addPoint(std::string name, GdkRGBA color, const Coordinate& p){
    Point *obj = new Point(name, color, p);
    _objs.addObj(obj);
    return obj;
}

Object* World::addLine(std::string name, GdkRGBA color, const Coordinates& c){
    Line *obj = new Line(name, color, c);
    _objs.addObj(obj);
    return obj;
}

Object* World::addPolygon(std::string name, GdkRGBA color, bool filled, const Coordinates& c){
    Polygon *obj = new Polygon(name, color, filled, c);
    _objs.addObj(obj);
    return obj;
}

void World::removeObj(std::string name){
    Object tmp(name);
    _objs.removeObj(&tmp);
}

Object* World::getObj(std::string name){
    Object tmp(name);
    return _objs.getObj(&tmp);
}

Object* World::translateObj(std::string objName, double dx, double dy){
    Object tmp(objName);
    Object *obj = _objs.getObj(&tmp);
    obj->transform(Transformation::newTranslation(dx,dy));
    return obj;
}

Object* World::scaleObj(std::string objName, double sx, double sy){
    Object tmp(objName);
    Object *obj = _objs.getObj(&tmp);
    obj->transform(Transformation::newScalingAroundObjCenter(sx,sy,obj->center()));
    return obj;
}

Object* World::rotateObj(std::string objName, double angle, const Coordinate& p,
                       rotateType type){
    Object tmp(objName);
    Object *obj = _objs.getObj(&tmp);

    switch(type){
    case rotateType::OBJECT:
        obj->transform(Transformation::newRotationAroundPoint(angle, obj->center()));
        break;
    case rotateType::WORLD:
        obj->transform(Transformation::newRotationAroundPoint(angle, Coordinate(0,0)));
        break;
    case rotateType::POINT:
        obj->transform(Transformation::newRotationAroundPoint(angle, p));
        break;
    }
    return obj;
}

#endif // WORLD_HPP
