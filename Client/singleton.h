#ifndef SINGLETON_H
#define SINGLETON_H

class Singleton;

class SingletonDestroyer
{
private:
    Singleton* p_instance = nullptr;
public:
    ~SingletonDestroyer();
    void initialize(Singleton* p)
    {
        p_instance = p;
    }
};

class Singleton
{
private:
    static Singleton*         p_instance;
    static SingletonDestroyer destroyer;

protected:
    Singleton() {}
    Singleton(const Singleton&)            = delete;
    Singleton& operator=(const Singleton&) = delete;
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

#endif // SINGLETON_H
