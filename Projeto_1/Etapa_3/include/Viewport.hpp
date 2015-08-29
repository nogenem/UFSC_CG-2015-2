#ifndef VIEWPORT_HPP
#define VIEWPORT_HPP

#include <ctime>
#include "Window.hpp"
#include "Objects.hpp"
#include "World.hpp"

#define PI 3.1415926535897932384626433832795

class Viewport
{
    public:
        Viewport(double width, double height, World *world):
            _width(width), _height(height), _world(world), _window(width,height),
            _border(new Border(width,height)) {}
        virtual ~Viewport(){ delete _border; }

        Coordinate transformCoordinate(const Coordinate& c) const;
        Coordinates transformCoordinates(const Coordinates& coords) const;

        void transformAllObjs();

        void gotoObj(std::string objName);
        void zoom(double step){_window.zoom(step);}
        void moveX(double value){_window.moveX(value);}
        void moveY(double value){_window.moveY(value);}
        void rotateWindow(double graus){_window.setAngulo(graus);}
        void drawObjs(cairo_t* cr);

        Coordinate center();
    protected:
    private:
        double _width, _height;
        World *_world;
        Window _window;

        cairo_t* _cairo;

        class Border : public Polygon {
            public:
                Border(int width, int height);
        };
        Border *_border;

        void drawObj(Object* obj);
        void drawPoint(Object* obj);
        void drawLine(Object* obj);
        void drawPolygon(Object* obj);

        void prepareContext(GdkRGBA& color);
};

// Cria a borda da Viewport [mudar cor/tamanho da linha?]
Viewport::Border::Border(int width, int height) :
    Polygon("_Border") {
    addCoordinate(0,0);
    addCoordinate(width, 0);
    addCoordinate(width, height);
    addCoordinate(0, height);
}

Coordinate Viewport::center(){
    return Coordinate(_width/2, _height/2);
}

void Viewport::gotoObj(std::string objName){
    Object *obj = _world->getObj(objName);
    _window.moveTo(obj->nCenter());
}

void Viewport::transformAllObjs(){
    _window.updateMatrix();
    auto t = _window.getT();

    for(int i = 0; i < _world->numObjs(); i++){
        Object *obj = _world->getObj(i);
        obj->transformNormalized(t);
    }
    _border->transformNormalized(t);
}

Coordinate Viewport::transformCoordinate(const Coordinate& c) const {
    const Coordinate wmin = _window.wMin();
    const Coordinate wmax = _window.wMax();

    double x = ((c.x-wmin.x)/(wmax.x-wmin.x))*_width;
    double y = (1-((c.y-wmin.y)/(wmax.y-wmin.y)))*_height;

    return Coordinate(x,y);
}

Coordinates Viewport::transformCoordinates(const Coordinates& coords) const{
    Coordinates vCoords;
    for(const auto &c : coords)
        vCoords.push_back(transformCoordinate(c));
    return vCoords;
}

void Viewport::drawObjs(cairo_t* cr){
    _cairo = cr;
    transformAllObjs();
    for(int i = 0; i < _world->numObjs(); i++){
        Object *obj = _world->getObj(i);
        this->drawObj(obj);
    }
    this->drawObj(_border);
}

void Viewport::drawObj(Object* obj){
    switch(obj->getType()){
    case ObjType::OBJECT:
        break;
    case ObjType::POINT:
        this->drawPoint(obj);
        break;
    case ObjType::LINE:
        this->drawLine(obj);
        break;
    case ObjType::POLYGON:
        this->drawPolygon(obj);
        break;
    }
}

void Viewport::drawPoint(Object* obj){
    Coordinate coord = transformCoordinate(obj->getNCoord(0));
    prepareContext(obj->getColor());
    cairo_move_to(_cairo, coord.x, coord.y);
    cairo_arc(_cairo, coord.x, coord.y, 1.0, 0.0, (2*PI) );//pnt deveria ir diminuindo, nao?
    cairo_fill(_cairo);
}

void Viewport::drawLine(Object* obj){
    auto coords = obj->getNCoords();
    if(coords[0] == coords[1]){// Usuario quer um ponto?
        drawPoint(obj);
        return;
    }
    coords = transformCoordinates(coords);
    prepareContext(obj->getColor());

    cairo_move_to(_cairo, coords[0].x, coords[0].y);
    cairo_line_to(_cairo, coords[1].x, coords[1].y);
    cairo_stroke(_cairo);
}

void Viewport::drawPolygon(Object* obj){
    auto coords = obj->getNCoords();
    if(coords.size() == 1){// Usuario quer um ponto?
        drawPoint(obj);
        return;
    }else if(coords.size() == 2){// Usuario quer uma linha?
        drawLine(obj);
        return;
    }

    coords = transformCoordinates(coords);
    prepareContext(obj->getColor());

    cairo_move_to(_cairo, coords[0].x, coords[0].y);
    for(unsigned int i = 0; i<coords.size(); i++)
        cairo_line_to(_cairo, coords[i].x, coords[i].y);

    cairo_close_path(_cairo);

    Polygon* p = (Polygon*) obj;
    if(p->filled()){
        cairo_stroke_preserve(_cairo);
        cairo_fill(_cairo);
    }else
        cairo_stroke(_cairo);
}

void Viewport::prepareContext(GdkRGBA& color){
    cairo_set_source_rgb(_cairo, color.red, color.green, color.blue);
    cairo_set_line_width(_cairo, 1);
}

#endif // VIEWPORT_HPP
