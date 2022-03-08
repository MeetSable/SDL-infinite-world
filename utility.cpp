#include <SDL.h>

#define MOD(a,b) (((a%b)+b)%b)
#define MIN(a,b) (a<b?a:b)
#define MAX(a,b) (a>b?a:b)
#define CLAMP(a,lo,hi) MIN(hi,MAX(lo,a))

template <typename T>
inline void swap(T& a, T& b)
{
	T temp;
	temp = a;
	a = b;
	b = temp;
}

inline bool operator==(SDL_Point const& a, SDL_Point const& b)
{
	return a.x == b.x && a.y == b.y;
}

inline bool operator!=(SDL_Point const& a, SDL_Point const& b)
{
	return !(a == b);
}
inline bool operator<(SDL_Point const& a, SDL_Point const& b)
{
	return a.x < b.x && a.y < b.y;
}

inline bool operator>(SDL_Point const& a, SDL_Point const& b)
{
	return !(a < b);
}