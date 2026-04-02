#include "singleton.h"

SingletonDestroyer::~SingletonDestroyer()
{
    delete p_instance;
}

// Определения статических переменных — строго в .cpp, не в .h
Singleton*         Singleton::p_instance = nullptr;
SingletonDestroyer Singleton::destroyer;
