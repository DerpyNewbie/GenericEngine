#pragma once
#include <map>
#include <functional>
#include <string>
#include <iostream>

template<class B>
class Factory {
    std::map<std::string, std::function<B*()>> m_factories_;

public:
    static Factory<B>& Instance() {
        static Factory<B> s_instance;
        return s_instance;
    }
    
    template<class T>
    void RegisterClass(const std::string& name) {
        m_factories_.insert({name, []() -> B* { return new T(); }});
    }

    B* Create(const std::string& name) {
        const auto it = m_factories_.find(name);
        if (it == m_factories_.end()) return nullptr; // not a derived class
        return (it->second)();
    }

    void PrintRegisteredClasses() {
        for (const auto &creator : m_factories_) {
            std::cout << creator.first << '\n';
        }
    }
};
#define FACTORY(Class) Factory<Class>::Instance()

template<class B, class T>
class Creator {
public:
    explicit Creator(const std::string& name) {
        FACTORY(B).RegisterClass<T>(name);
    }
};

#define REGISTER(base_class, derived_class) \
Creator<base_class, derived_class> s_##derived_class##Creator(#derived_class);