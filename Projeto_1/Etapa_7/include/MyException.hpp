#ifndef MYEXCEPTION_HPP
#define MYEXCEPTION_HPP

#include <exception>

class MyException : public std::runtime_error
{
    public:
        MyException(const std::string& message) : runtime_error(message) {}
    protected:
    private:
};

#endif // MYEXCEPTION_HPP
