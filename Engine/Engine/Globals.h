#pragma once

// Tipos básicos
typedef unsigned int uint;
typedef unsigned char uchar;
typedef unsigned __int32 uint32;
typedef unsigned __int64 uint64;

// Macros de configuración
#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f
#define PI 3.14159265358979323846f

// Macros de utilidad
#define CAP(n) ((n <= 0.0f) ? n=0.0f : (n >= 1.0f) ? n=1.0f : n=n)
#define MIN(a,b) ((a)<(b)) ? (a) : (b)
#define MAX(a,b) ((a)>(b)) ? (a) : (b)
#define CLAMP(value, min, max) (value < min)? min : (value > max)? max : value

// Definiciones de Release/Debug
#ifdef _DEBUG
#define LOG(format, ...) log(__FILE__, __LINE__, format, __VA_ARGS__);
#else
#define LOG(format, ...) 
#endif

// Tipos de LOG
enum class LogType
{
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR
};

// Función de logging (debes implementarla en algún lugar)
void log(const char file[], int line, const char* format, ...);