#ifndef FILEHANDLERS_HPP
#define FILEHANDLERS_HPP

#include <fstream>
#include <sstream>
#include <vector>
#include <regex>
#include <map>
#include <string>

/*
    Possiveis Diretivas:
        v, o, p, l, f, curv2,
        cstype, deg, end
        usemtl, newmtl, Kd

    Obs:
        Index dos vertices podem ser positivos
         ou negativos.

        Os arquivos .mtl devem estar na mesma
         pasta que o arquivo .obj.
*/

class ColorReader
{
    public:
        ColorReader(){ m_colors["Padrao"] = GdkRGBA{}; }
        bool loadFile(const std::string& filename);
        const GdkRGBA& getColor(const std::string& colorName) const;

    private:
        void loadColors();
        void addColor(std::string name, std::stringstream& line);

    private:
        //Ex: <"Color0", GdkRGBA(0,0.6,0)>
        std::map<std::string, GdkRGBA> m_colors;
        std::ifstream m_ifs;
};

class ColorWriter
{
    public:
        ColorWriter(){}
        void loadFile(const std::string& filename);
        std::string getColorName(const GdkRGBA& color);

    private:
        void writeColor(const std::string& colorName,
                            const GdkRGBA& color);

    private:
        //Ex: <"(0,0.6,0)", "Color0">
        std::map<std::string, std::string> m_colors;
        std::ofstream m_ofs;
        //Contador de cores ja escritas, usado no nome da cor
        int m_numColors = 0;
};

class ObjStream
{
    public:
        ObjStream(std::string& filename);

    protected:
        std::string m_name;// Nome do arquivo, sem a extensao
        std::string m_path;// Path para o arquivo
        std::fstream m_objsFile;
        std::fstream m_colorsFile;
        GdkRGBA m_color{};// Cor atual [inicializada como preta]
};

class ObjReader : public ObjStream
{
    public:
        ObjReader(std::string& filename);
        std::vector<Object*>& getObjs(){ return m_objs; }

    private:
        void loadObjs();
        void loadColorsFile(std::stringstream& line);
        void changeColor(std::stringstream& line);
        void addCoord(std::stringstream& line);
        void addPoint(std::stringstream& line);
        void addPoly(std::stringstream& line, bool filled);
        // Usado para destruir os objs caso de algum erro
        void destroyObjs();

        void setFreeFormType(std::stringstream& line);
        void setDegree(std::stringstream& line);//Soh le mas nao faz nada...
        void addCurve(std::stringstream& line);

    private:
        std::vector<Object*> m_objs;
        Coordinates m_coords;// Todas as coordenadas lidas do arquivo
        ColorReader m_cReader;
        bool m_usingColorsFile = false;// Existe uma chamada 'mtllib' no .obj?
        int m_numSubObjs = 0; // Numero de objetos em seguida com o mesmo nome

        unsigned int m_degree = 3;//Curvas de Bezier sao de grau 3
        ObjType m_freeFormType = ObjType::OBJECT;
};

class ObjWriter : public ObjStream
{
    public:
        ObjWriter(std::string& filename);
        void writeObjs(World *world);

    private:
        void printObj(Object* obj);

    private:
        // Numero de coordenadas ja escritas
        int m_numVertex = 0;
        ColorWriter m_cWriter;
};

ObjStream::ObjStream(std::string& filename) {
    if(!filename.find("\\.obj"))
        filename += ".obj";

    std::size_t found = filename.find_last_of("/\\");
    m_path = filename.substr(0,found+1);
    m_name = filename.substr(found+1, filename.size()-found-5);// Nome sem o '.obj'
}

ObjReader::ObjReader(std::string& filename):
    ObjStream(filename){

    m_objsFile.open(filename.c_str());
    if(!m_objsFile.is_open())
        throw MyException("Erro tentando abrir o arquivo:\n\t" + filename + ".\n");
    else
        loadObjs();
}

void ObjReader::destroyObjs(){
    for(auto o : m_objs)
        delete o;
}

void ObjReader::loadObjs(){
    std::string tmp, keyWord;
    while(std::getline(m_objsFile, tmp)){
        if(tmp.size() <= 1) continue;
        std::stringstream line(tmp);
        line >> keyWord;

        if(keyWord == "#"){ /* Nao faz nada... */ }
        else if(keyWord == "mtllib"){ loadColorsFile(line); }
        else if(keyWord == "usemtl"){ if(m_usingColorsFile){changeColor(line);} }
        else if(keyWord == "v"){ addCoord(line); }
        else if(keyWord == "o"){ line >> m_name; m_numSubObjs = 0; }
        else if(keyWord == "w"){ /* TODO(?) */ }
        else if(keyWord == "p"){ addPoint(line); }
        else if(keyWord == "l"){ addPoly(line, false); m_numSubObjs++; }
        else if(keyWord == "f"){ addPoly(line, true); m_numSubObjs++; }
        else if(keyWord == "cstype"){ setFreeFormType(line); } //cstype type
        else if(keyWord == "deg"){ setDegree(line); }//deg degu
        else if(keyWord == "curv2"){ addCurve(line); }//curv2 v1 v2 v3 v4 ...
        else if(keyWord == "end"){ }
    }
}

void ObjReader::loadColorsFile(std::stringstream& line){
    std::string file;
    line >> file;
    m_usingColorsFile = m_cReader.loadFile(m_path+file);
}

void ObjReader::changeColor(std::stringstream& line){
    std::string colorName;
    line >> colorName;
    m_color = m_cReader.getColor(colorName);
}

void ObjReader::addCoord(std::stringstream& line){
    double x,y;
    line >> x >> y;
    m_coords.emplace_back(x,y);
}

void ObjReader::addPoint(std::stringstream& line){
    int index = 0;
    int size = m_coords.size();
    Point *p;

    while(line >> index){

        if(index < 0)
            index = size + (index+1);
        else
            index--;

        if(index < 0 || index >= size){
            destroyObjs();
            throw MyException("Indice de vertice invalido na linha: "+ line.str() + ".\n");
        }

        std::string name = m_numSubObjs == 0 ? m_name :
            m_name+"_sub"+std::to_string(m_numSubObjs);

        p = new Point(name, m_color);
        p->addCoordinate(m_coords[index]);
        m_objs.push_back(p);
        m_numSubObjs++;
    }
}

void ObjReader::addPoly(std::stringstream& line, bool filled){
    int index = 0;
    int size = m_coords.size();
    Coordinates objCoords;

    while(line >> index){
        if(index < 0)
            index = size + index;
        else
            index--;

        if(index < 0 || index >= size){
            destroyObjs();
            throw MyException("Indice de vertice invalido na linha: "+ line.str() + ".\n");
        }

        objCoords.push_back(m_coords[index]);
    }

    Object *obj;
    std::string name = m_numSubObjs == 0 ? m_name :
        m_name+"_sub"+std::to_string(m_numSubObjs);

    if(objCoords.size() == 2)
        obj = new Line(name, m_color, objCoords);
    else
        obj = new Polygon(name, m_color, filled, objCoords);

    m_objs.push_back(obj);
}

void ObjReader::addCurve(std::stringstream& line){
    if(m_freeFormType == ObjType::OBJECT)
        throw MyException("Tentativa de criar uma curva sem 'cstype' na linha: "+
                          line.str() +".\n");

    int index = 0;
    int size = m_coords.size();
    Coordinates objCoords;

    while(line >> index){
        if(index < 0)
            index = size + index;
        else
            index--;

        if(index < 0 || index >= size){
            destroyObjs();
            throw MyException("Indice de vertice invalido na linha: "+ line.str() + ".\n");
        }

        objCoords.push_back(m_coords[index]);
    }

    Object *obj = nullptr;
    std::string name = m_numSubObjs == 0 ? m_name :
        m_name+"_sub"+std::to_string(m_numSubObjs);

    switch(m_freeFormType){
    case ObjType::BEZIER_CURVE:
        obj = new Curve(name, m_color, objCoords);
        break;
    /*case ObjType::BSPLINE_CURVE:
        obj = new BSplineCurve(name, m_color, objCoords);
        break;*/
    default:
        break;
    }

    m_objs.push_back(obj);
}

void ObjReader::setFreeFormType(std::stringstream& line){
    std::string type;
    line >> type;

    if(type == "rat"){ line >> type; }//Nao intendi bem o que isto quer dizer...

    if(type == "bezier"){ m_freeFormType = ObjType::BEZIER_CURVE; }
    else if(type == "bspline"){ /* m_freeFormType = ObjType::BSPLINE_CURVE; */ }//TODO...
    else{ throw MyException("cstype igual a: '"+
                            type+"', nao eh suportado por esta aplicacao.\n"); }
}

void ObjReader::setDegree(std::stringstream& line){
    int degree;
    line >> degree;

    if(degree > 0)
        m_degree = degree;
}

ObjWriter::ObjWriter(std::string& filename):
    ObjStream(filename) {

    m_objsFile.open(filename.c_str(), std::fstream::out);

    if(!m_objsFile.is_open())
        throw MyException("Erro tentando abrir arquivo:\n\t" + filename + ".\n");
}

void ObjWriter::writeObjs(World *world){
    m_cWriter.loadFile(m_path+m_name+".mtl");
    m_objsFile << "mtllib " << m_name << ".mtl\n\n";

    Object *obj;
    int size = world->numObjs();
    for(int i = 0; i<size; i++){
        obj = world->getObj(i);
        printObj(obj);
    }
}

void ObjWriter::printObj(Object* obj){
    auto coords = obj->getCoords();

    if(obj->getType() == ObjType::BEZIER_CURVE)
        coords = ((Curve*)obj)->getControlPoints();

    for(const auto &c : coords)
        m_objsFile << "v " << c.x << " " << c.y << "\n";

    m_objsFile << "\no " << obj->getName() << "\n";

    const std::string colorName = m_cWriter.getColorName(obj->getColor());

    if(colorName != "none")
        m_objsFile << "usemtl " << colorName << "\n";

    std::string keyWord;
    switch(obj->getType()){
    case ObjType::POINT:
        keyWord = "p";
        break;
    case ObjType::LINE:
        keyWord = "l";
        break;
    case ObjType::POLYGON:
        keyWord = ((Polygon*)obj)->filled() ? "f" : "l";
        break;
    case ObjType::BEZIER_CURVE:
        m_objsFile << "cstype bezier\n";
        m_objsFile << "deg 3\n";
        keyWord = "curv2";
    default:
        break;
    }

    int size = coords.size();
    m_objsFile << keyWord;
    for(int i = 0; i<size; i++)
        m_objsFile << " " << m_numVertex+(i+1);


    if(obj->getType() == ObjType::BEZIER_CURVE)
        m_objsFile << "\nend\n" << std::endl;
    else
        m_objsFile << "\n" << std::endl;
    m_numVertex += size;
}

bool ColorReader::loadFile(const std::string& filename){
    m_ifs.open(filename);

    if(!m_ifs.is_open()){ //dar throw?
        std::cout << "Erro tentando abrir arquivo:\n\t" + filename + ".\n";
        return false;
    }else{
        loadColors();
        return true;
    }
}

void ColorReader::loadColors(){
    std::string tmp, keyWord, colorName;
    while(std::getline(m_ifs, tmp)){
        if(tmp.size() <= 1) continue;
        std::stringstream line(tmp);
        line >> keyWord;

        if(keyWord == "newmtl"){ line >> colorName; }
        if(keyWord == "Kd"){ addColor(colorName, line); }
    }
}

void ColorReader::addColor(std::string name, std::stringstream& line){
    double r,g,b;
    line >> r >> g >> b;
    m_colors[name] = GdkRGBA{r,g,b};
}

const GdkRGBA& ColorReader::getColor(const std::string& colorName) const{
    auto iter = m_colors.find(colorName);
    if(iter != m_colors.end())
        return iter->second;
    else{
        auto iter2 = m_colors.find("Padrao");
        return iter2->second;
    }
}

void ColorWriter::loadFile(const std::string& filename){
    m_ofs.open(filename);

    if(!m_ofs.is_open()) //dar throw?
        std::cout << "Erro tentando abrir arquivo:\n\t" + filename + ".\n";
}

std::string ColorWriter::getColorName(const GdkRGBA& color){
    if(!m_ofs.is_open())
        return "none";

    std::string colorString = gdk_rgba_to_string(&color);

    auto iter = m_colors.find(colorString);
    if(iter != m_colors.end())
        return iter->second;
    else{ //Se nao existir, cria um novo nome para a cor, e retorna ele...
        std::string colorName = "Color" + std::to_string(m_numColors++);

        m_colors[colorString] = colorName;
        writeColor(colorName, color);
        return colorName;
    }
}

void ColorWriter::writeColor(const std::string& colorName, const GdkRGBA& color){
    m_ofs << "newmtl " << colorName << "\n";
    m_ofs << "Kd " << color.red << " " << color.green << " " << color.blue << "\n\n";
}

#endif // FILEHANDLERS_HPP
