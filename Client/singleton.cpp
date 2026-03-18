#include "singleton.h"

// Определения статических переменных — строго в .cpp, не в .h
Singleton*         Singleton::p_instance = nullptr;
SingletonDestroyer Singleton::destroyer;
