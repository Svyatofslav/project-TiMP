class Singleton;

// Для удаления данных
class SingletonDestroyer
{
private:
    Singleton * p_instance;
public:
    ~SingletonDestroyer()
    {
        delete p_instance;
    }
    void initialize(Singleton * p)
    {
        p_instance = p;
    }
};

class Singleton
{
private:
    static Singleton * p_instance;
    static SingletonDestroyer destroyer;
protected:
    Singleton() {}
    Singleton(const Singleton&) = delete;
    Singleton& operator = (Singleton &) = delete;
    ~Singleton() {}
    friend class SingletonDestroyer;
public:
    static Singleton* getInstance()
    {
        if (!p_instance)
        {
            p_instance = new Singleton();
            destroyer.initialize(p_instance);
        }
        return p_instance;
    }
};

// Выделение памяти под статичные переменные
Singleton::Singleton * p_instance;
Singleton::SingletonDestroyer destroyer;

