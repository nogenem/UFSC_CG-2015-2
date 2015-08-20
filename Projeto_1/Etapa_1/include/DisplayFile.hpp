#ifndef DISPLAYFILE_HPP
#define DISPLAYFILE_HPP

#include "ListaEnc.hpp"
#include "Objects.hpp"

class DisplayFile : public ListaEnc<Object*>
{
    public:
        DisplayFile(){}
        virtual ~DisplayFile() {}

        void addObj(Object* obj);
        Object* getObj(int pos){ return getFromPos(pos); }
        int size(){ return getSize(); }
        void removeObj(Object* obj){ delete retiraEspecifico(obj); }
    protected:
    private:
};

void DisplayFile::addObj(Object* obj){
    if(obj->getName() == "")
        throw "Adicione um nome para este objeto.\n";

    if(!contem(obj)){
        adiciona(obj);
    }else
        throw "Ja existe um objeto com este mesmo nome.\n";
}

#endif // DISPLAYFILE_HPP
