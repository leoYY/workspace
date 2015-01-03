// Copyright (c) 2015 Baidu.com , Inc. All Rights Reserved
// @author: yuanyi (yuan.yi.coding@gmail.com)
// @brief : 

#ifndef _CLOSURE_H_
#define _CLOSURE_H_

namespace common {
namespace util {
    

class Closure : public noncopyable {
public:
    Closure() {}
    virtual ~Closure() {}

    virtual void Run() = 0;
};

// closure function with zero args
class FunctionClosure0 : public Closure {

public:
    typedef void (*FunctionType)();

    FunctionClosure0(FunctionType func, bool self_del)
        : _function(func), _self_del(self_del) {}

    ~FunctionClosure0(); 

    void Run() {
        bool need_delete = _self_del; 
        _function();
        if (need_delete) {
            delete this;    
        }
    }
private:
    FunctionType _function;
    bool _self_del;
};

template <typename Arg1>
class FunctionClosure1 : public Closure {
    
public:
    typedef void (*FunctionType)(Arg1 arg);

    FunctionClosure1(FunctionType func, Arg1 arg, bool self_del)
        : _function(func), _arg(arg), _self_del(self_del) {}

    ~FunctionClosure1();

    void Run() {
        bool need_del = _self_del;
        _function(arg);
        if (need_del) {
            delete this; 
        }
    }
private:
    FunctionType _function;
    Arg1 _arg;
    bool _self_del;
};

template <typename Arg1, typename Arg2>
class FunctionClosure2 : public Closure {

public:
    typedef void (*FunctionType)(Arg1 arg1, Arg2 arg2);

    FunctionClosure2(FunctionType func, Arg1 arg1, Arg2 arg2, bool self_del)
        : _function(func), _arg1(arg1), _arg2(arg2), _self_del(self_del) {}

    ~FunctionClosure2();

    void Run() {
        bool need_del = _self_del; 
        _function(_arg1, _arg2);
        if (need_del) {
            delete this; 
        }
    }

private:
    FunctionType _function;
    Arg1 _arg1;
    Arg2 _arg2;
    bool _self_del;
};

template <typename Arg1, typename Arg2, typename Arg3>
class FunctionClosure3 : public Closure {

public:
    typedef void (*FunctionType)(Arg1, Arg2, Arg3);

    FunctionClosure3(FunctionType func, Arg1 arg1, Arg2 arg2, Arg3 arg3, bool self_del)
        : _function(func), _arg1(arg1), _arg2(arg2), _arg3(arg3), _self_del(self_del) {}

    ~FunctionClosure3();

    void Run() {
        bool need_del = _self_del; 
        _function(_arg1, _arg2, _arg3);
        if (need_del) {
            delete this; 
        }
    }
private:
    FunctionType _function;
    Arg1 _arg1;
    Arg2 _arg2;
    Arg3 _arg3;
    bool _self_del;
};


// closure class method with zero args
template <typename Class>
class MethodClosure0 : public Closure {

public:
    typedef void (Class::*MethodType)();

    MethodClosure0(Class* obj, MethodType method, bool self_del)
        : _object(obj), _method(method), _self_del(self_del) {}

    ~MethodClosure0(); 

    void Run() {
        if (_object != NULL) {
            bool need_del = _self_del; 
            (_object->_method)();
            if (need_del){
                delete this;
            }
        }
    }
private:
    Class* _object;
    MethodType _method;
    bool _self_del;
};

template <typename Class, typename Arg1>
class MethodClosure1 : public Closure {

public:
    typedef void (Class::*MethodType) (Arg1);

    MethodClosure1(Class* obj, MethodType method, Arg1 arg1, bool self_del)
        : _object(obj), _method(method), _arg1(arg1), _self_del(self_del) {}

    ~MethodClosure1();

    void Run() {
        bool need_del = _self_del; 
        (_object->_method)(_arg1);
        if (need_del) {
            delete this; 
        }
    }
private:
    Class* _object;
    MethodType _method;
    Arg1 _arg1;
    bool _self_del;
};  

template <typename Class, typename Arg1, typename Arg2>
class MethodClosure2 : public Closure {

public: 
    typedef void (Class::*MethodType) (Arg1, Arg2);

    MethodClosure2(Class* obj, MethodType method, Arg1 arg1, Arg2 arg2, bool self_del)
        : _object(obj), _method(method), _arg1(arg1), _arg2(arg2), _self_del(self_del) {}

    ~MethodClosure2(); 

    void Run() {
        bool need_del = _self_del; 
        (_object->_method) (_arg1, _arg2);
        if (need_del) {
            delete this; 
        }
    }
private:
    Class* _object;
    MethodType _method;
    Arg1 _arg1;
    Arg2 _arg2;
    bool _self_del;
};

template <typename Class, typename Arg1, typename Arg2, typename Arg3>
class MethodClosure3 : public Closure {

public:
    typedef void (Class::*MethodType) (Arg1, Arg2, Arg3);

    MethodClosure3(Class* obj, MethodType method, Arg1 arg1, Arg2 arg2, Arg3 arg3, bool self_del)
        : _object(obj), _method(method), _arg1(arg1), _arg2(arg2), _arg3(arg3), _self_del(self_del) {}
     
    ~MethodClosure3();

    void Run() {
        bool need_del = _self_del; 
        (_object->_method) (_arg1, _arg2, _arg3);
        if (need_del) {
            delete this; 
        } 
    }
private:
    Class* _object;
    MethodType _method;
    Arg1 _arg1;
    Arg2 _arg2;
    Arg3 _arg3;
    bool _self_del;
};  

// user interface for Closure
inline Closure* NewClosure(void(*func)()) {
    return new FunctionClosure0(func, true);
}

inline Closure* NewPermentClosure(void(*func)()) {
    return new FunctionClosure0(func, false);
}

template <typename Arg1>
inline Closure* NewClosure(void(*func)(Arg1), Arg1 arg) {
    return new FunctionClosure1(func, arg, true);
}

template <typename Arg1>
inline Closure* NewPermentClosure(void(*func)(Arg1), Arg1 arg) {
    return new FunctionClosure1(func, arg, false);
}

template <typename Arg1, typename Arg2>
inline Closure* NewClosure(void (*func) (Arg1, Arg2), Arg1 arg1, Arg2 arg2) {
    return new FunctionClosure2(func, arg1, arg2, true);
}

template <typename Arg1, typename Arg2>
inline Closure* NewPermentClosure(void (*func) (Arg1, Arg2), Arg1 arg1, Arg2 arg2) {
    return new FunctionClosure2(func, arg1, arg2, false);
}

template <typename Arg1, typename Arg2, typename Arg3>
inline Closure* NewClosure(void (*func) (Arg1, Arg2, Arg3), Arg1 arg1, Arg2 arg2, Arg3 arg3) {
    return new FunctionClosure3(func, arg1, arg2, arg3, true);
}

template <typename Arg1, typename Arg2, typename Arg3>
inline Closure* NewPermentClosure(void (*func) (Arg1, Arg2, Arg3), Arg1 arg1, Arg2 arg2, Arg3 arg3) {
    return new FunctionClosure3(func, arg1, arg2, arg3, false);
}

template <typename Class>
inline Closure* NewClosure(Class* object, void (Class::*method)()) {
    return new MethodClosure0(object, method, true);
}

template <typename Class>
inline Closure* NewClosure(Class* object, void (Class::*method)()) {
    return new MethodClosure0(object, method, false);
}

template <typename Class, typename Arg1>
inline Closure* NewClosure(Class* object, void (Class::*method)(Arg1), Arg1 arg1) {
    return new MethodClosure1(object, method, arg1, true);
}

template <typename Class, typename Arg1>
inline Closure* NewPermentClosure(Class* object, void (Class::*method)(Arg1), Arg1 arg1) {
    return new MethodClosure1(object, method, arg1, false);
}

template <typename Class, typename Arg1, typename Arg2>
inline Closure* NewClosure(Class* object, void (Class::*method)(Arg1, Arg2), Arg1 arg1, Arg2 arg2) {
    return new MethodClosure2(object, method, arg1, arg2, true);
}

template <typename Class, typename Arg1, typename Arg2>
inline Closure* NewPermentClosure(Class* object, void (Class::*method)(Arg1, Arg2), Arg1 arg1, Arg2 arg2) {
    return new MethodClosure2(object, method, arg1, arg2, false);
}

template <typename Class, typename Arg1, typename Arg2, typename Arg3>
inline Closure* NewClosure(Class* object, void (Class::*method)(Arg1, Arg2, Arg3), Arg1 arg1, Arg2 arg2, Arg3 arg3) {
    return new MethodClosure3(object, method, arg1, arg2, arg3, true);
}

template <typename Class, typename Arg1, typename Arg2, typename Arg3>
inline Closure* NewPermentClosure(Class* object, void (Class::*method)(Arg1, Arg2, Arg3), Arg1 arg1, Arg2 arg2, Arg3 arg3) {
    return new MethodClosure3(object, method, arg1, arg2, arg3, false);
}

}   // ending namespace util
}   // ending namespace common

#endif

