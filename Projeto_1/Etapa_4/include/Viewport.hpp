#ifndef VIEWPORT_HPP
#define VIEWPORT_HPP

#include <ctime>
#include "Window.hpp"
#include "Objects.hpp"
#include "World.hpp"
#include "Clipping.hpp"

#define PI 3.1415926535897932384626433832795

class Border : public Polygon {
public:
    Border(ClipWindow& window);
    void addNCoordinate(const Coordinates& coords){
        _nCoords.insert(_nCoords.end(), coords.begin(), coords.end());
    }
};

class Viewport
{
    public:
        Viewport(double width, double height, World *world):
            _width(width), _height(height), _world(world), _window(width,height),
            _clipping(_cWindow), _border(new Border(_cWindow))
            { transformAndClipAllObjs(); }
        virtual ~Viewport(){ delete _border; }

        Coordinate transformCoordinate(const Coordinate& c) const;
        Coordinates transformCoordinates(const Coordinates& coords) const;

        void transformAndClipObj(Object* obj);
        void changeLineClipAlg(LineClipAlgs alg){_clipping.setLineClipAlg(alg); transformAndClipAllObjs();}

        void gotoObj(std::string objName);
        void zoomWindow(double step){_window.zoom(step); transformAndClipAllObjs();}
        void moveWindow(double x, double y){_window.move(x,y); transformAndClipAllObjs();}
        void rotateWindow(double graus){_window.setAngulo(graus); transformAndClipAllObjs();}
        void drawObjs(cairo_t* cr);
    protected:
    private:
        double _width, _height;
        World* _world;
        Window _window;

        cairo_t* _cairo;

        ClipWindow _cWindow = {-0.95,0.95,-0.95,0.95};
        Clipping _clipping;
        Border *_border;

        void transformAndClipAllObjs();

        void drawObj(Object* obj);
        void drawPoint(Object* obj);
        void drawLine(Object* obj);
        void drawPolygon(Object* obj);

        void prepareContext(GdkRGBA& color);
};

// Cria a borda da Viewport
Border::Border(ClipWindow& window) :
    Polygon("_border_", GdkRGBA({0,0.9,0})) {

    addCoordinate(window.minX, window.minY);
    addCoordinate(window.maxX, window.minY);
    addCoordinate(window.maxX, window.maxY);
    addCoordinate(window.minX, window.maxX);

    addNCoordinate(_coords);
}

// Remover isso?
void Viewport::gotoObj(std::string objName){
    Object *obj = _world->getObj(objName);
    Coordinate c = obj->center();
    _window.moveTo(c);
    transformAndClipAllObjs();
}

void Viewport::transformAndClipObj(Object* obj){
    auto t = _window.getT();
    bool shouldDraw = true;
    obj->transformNormalized(t);

    switch(obj->getType()){
    case ObjType::OBJECT:
        shouldDraw = false;
        break;
    case ObjType::POINT:
        shouldDraw = _clipping.clipPoint((Point*)obj);
        break;
    case ObjType::LINE:
        shouldDraw = _clipping.clipLine((Line*)obj);
        break;
    case ObjType::POLYGON:
        break;
    }

    if(!shouldDraw)
        obj->getNCoords().clear();
}

void Viewport::transformAndClipAllObjs(){
    _window.updateMatrix();

    for(int i = 0; i < _world->numObjs(); i++){
        Object *obj = _world->getObj(i);
        transformAndClipObj(obj);
    }
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

    for(int i = 0; i < _world->numObjs(); i++){
        Object *obj = _world->getObj(i);
        this->drawObj(obj);
    }
    this->drawObj(_border);
}

void Viewport::drawObj(Object* obj){
    if(obj->getNCoordsSize() == 0) return;

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