#include "math.hpp"
#include <algorithm>
#include <cmath>

#define M_PI 3.14159265358979323846

namespace sdlgame::math {

Vector2::Vector2(const SDL_Point &p)
    : x(static_cast<double>(p.x)), y(static_cast<double>(p.y)) {}

Vector2 &Vector2::operator+=(const Vector2 &oth) {
  x += oth.x;
  y += oth.y;
  return *this;
}
Vector2 &Vector2::operator-=(const Vector2 &oth) {
  x -= oth.x;
  y -= oth.y;
  return *this;
}


Vector2 Vector2::operator+(const Vector2 &oth) const {
  return {x + oth.x, y + oth.y};
}
Vector2 Vector2::operator-() const { return Vector2(-x, -y); }
Vector2 Vector2::operator-(const Vector2 &oth) const {
  return {x - oth.x, y - oth.y};
}


bool Vector2::operator==(const Vector2 &oth) const {
  return x == oth.x and y == oth.y;
}

double Vector2::magnitude() const { return std::sqrt(x * x + y * y); }
double Vector2::sqr_magnitude() const { return x * x + y * y; }

Vector2 Vector2::normalize() const {
  double mag = magnitude();
  if (mag == 0) {
    return {0, 0};
  }
  return Vector2(x / mag, y / mag);
}

void Vector2::normalize_ip() {
  double mag = magnitude();
  if (mag != 0) {
    *this *= 1.0 / mag;
  }
}

double Vector2::dot(const Vector2 &oth) const { return x * oth.x + y * oth.y; }

double Vector2::angle_to(const Vector2 &oth) const {
  double p = dot(oth);
  double det = x * oth.y - y * oth.x;
  double deg = radian_to_degree(std::atan2(det, p));
  return std::min(abs(deg), 360 - abs(deg));
}



double Vector2::distance_to(const Vector2 &oth) const {
  return std::sqrt((x - oth.x) * (x - oth.x) + (y - oth.y) * (y - oth.y));
}

Vector2 Vector2::reflect(const Vector2 &normal) const {
  return (*this) - normal * 2 * (dot(normal));
}

void Vector2::reflect_ip(const Vector2 &normal) { *this = reflect(normal); }

Vector2 Vector2::project(const Vector2 &normal) const {
  double sqr_mag = normal.sqr_magnitude();
  if (sqr_mag == 0) {
    return {0, 0};
  }
  return normal * (dot(normal) / sqr_mag);
}

void Vector2::project_ip(const Vector2 &normal) { *this = project(normal); }

std::string Vector2::toString() const {
  return "Vector2<" + std::to_string(x) + " , " + std::to_string(y) + ">";
}

SDL_FPoint Vector2::to_SDL_FPoint() const {
  return {static_cast<float>(x), static_cast<float>(y)};
}

} // namespace sdlgame::math
