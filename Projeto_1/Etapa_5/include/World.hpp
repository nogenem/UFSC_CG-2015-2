#ifndef WORLD_HPP
#define WORLD_HPP

#include "DisplayFile.hpp"
#include "Dialogs.hpp"

class World
{
    public:
        World() {}
        virtual ~World() {}

        Object* addPoint(const std::string& name, const GdkRGBA& color, const Coordinate& p);
        Object* addLine(const std::string& name, const GdkRGBA& color, const Coordinates& c);
        Object* addPolygon(const std::string& name, const GdkRGBA& color, bool filled, const Coordinates& c);
        Object* addBezierCurve(const std::string& name, const GdkRGBA& color, const Coordinates& c);
        void addObj(Object *obj){ validateName(obj->getName()); m_objs.addObj(obj); }

        void removeObj(const std::string& name);
        int numObjs() const { return m_objs.size(); }
        Object* getObj(int pos){ return m_objs.getObj(pos); }
        Object* getObj(const std::string& name);
        Elemento<Object*>* getFirstObject(){ return m_objs.getFirstElement(); }

        Object* translateObj(const std::string& objName, double dx, double dy);
        Object* scaleObj(const std::string& objName, double sx, double sy);
        // p = ponto central para rodar;
        Object* rotateObj(const std::string& objName, double angle, const Coordinate& p,
                       rotateType type);

    private:
        DisplayFile m_objs;

        void validateName(const std::string& name);
};

void World::validateName(const std::string& name){
    if(name == "")
        throw MyException("Adicione um nome para este objeto.\n");

    Object tmp(name);
    if(m_objs.contains(&tmp))
        throw MyException("Ja existe um objeto com o nome '"+ name +"'.\n");
}

Object* World::addPoint(const std::string& name, const GdkRGBA& color, const Coordinate& p){
    validateName(name);

    Point *obj = new Point(name, color, p);
    m_objs.addObj(obj);
    return obj;
}

Object* World::addLine(const std::string& name, const GdkRGBA& color, const Coordinates& c){
    validateName(name);

    Line *obj = new Line(name, color, c);
    m_objs.addObj(obj);
    return obj;
}

Object* World::addPolygon(const std::string& name, const GdkRGBA& color,
                            bool filled, const Coordinates& c){
    validateName(name);

    Polygon *obj = new Polygon(name, color, filled, c);
    m_objs.addObj(obj);
    return obj;
}

Object* World::addBezierCurve(const std::string& name, const GdkRGBA& color,
                              const Coordinates& c){
    validateName(name);

    if(c.size() < 4 || (c.size()-4)%3 != 0)
        throw MyException("Uma curva de Bezier deve ter 4, 7, 10, 13... coordenadas.");

    Curve *obj = new Curve(name, color, c);
    m_objs.addObj(obj);
    return obj;
}

void World::removeObj(const std::string& name){
    Object tmp(name);
    m_objs.removeObj(&tmp);
}

Object* World::getObj(const std::string& name){
    Object tmp(name);
    return m_objs.getObj(&tmp);
}

Object* World::translateObj(const std::string& objName, double dx, double dy){
    Object tmp(objName);
    Object *obj = m_objs.getObj(&tmp);
    obj->transform(Transformation::newTranslation(dx,dy));
    return obj;
}

Object* World::scaleObj(const std::string& objName, double sx, double sy){
    Object tmp(objName);
    Object *obj = m_objs.getObj(&tmp);
    obj->transform(Transformation::newScalingAroundObjCenter(sx,sy,obj->center()));
    return obj;
}

Object* World::rotateObj(const std::string& objName, double angle, const Coordinate& p,
                       rotateType type){
    Object tmp(objName);
    Object *obj = m_objs.getObj(&tmp);

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
