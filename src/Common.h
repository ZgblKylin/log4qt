#pragma once
#include <QtCore/QtCore>
#include <functional>

class Defer
{
public:
    Defer() = default;
    Defer(std::function<void(void)> onDelete) : onDel(onDelete) {}
    ~Defer() { if (onDel) { onDel(); } }

    Defer(Defer&& other) : onDel(other.onDel) {}
    Defer& operator=(Defer&& other) { onDel = other.onDel; return *this; }
    Defer& operator=(std::function<void(void)> onDelete) { onDel = onDelete; return *this; }
    void reset() { onDel = {}; }

private:
    Defer(const Defer&) = delete;
    Defer& operator=(const Defer&) = delete;
    std::function<void(void)> onDel;
};
#define DEFER_STRCAT_2(A, B) A ## B
#define DEFER_STRCAT(A, B) DEFER_STRCAT_2(A, B)
#define DEFER_TEMPNAME DEFER_STRCAT(__defer, __LINE__)
#define defer Defer DEFER_TEMPNAME; DEFER_TEMPNAME =
