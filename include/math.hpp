#pragma once
#include <concepts>
#ifndef SDLGAME_MATH_
#define SDLGAME_MATH_
#include <SDL2/SDL_rect.h>
#include <string>
/**
 * namespace for most use math functionality in game dev
 */
namespace sdlgame::math {

constexpr double degree_to_radian(double deg) { return deg * M_PI / 180.0; }
constexpr double radian_to_degree(double rad) { return rad * 180.0 / M_PI; }

template <typename T>
concept Arithmetic = std::floating_point<T> || std::integral<T>;

Arithmetic auto clamp(Arithmetic auto val, Arithmetic auto left,
                      Arithmetic auto right) {
  if (left > right) {
    std::swap(left, right);
  }
  return (val < left ? left : (val > right ? right : val));
}

/**
 *  a class for 2D vector, also can represent a point on a 2d surface
 * since the simplicity of 2d vector, we dont need get and set function
 *
 * also for some dumb reason, you can do Vector2 * number, but not number *
 * Vector2
 *
 */
struct Vector2 {
  double x = 0;
  double y = 0;

  Vector2() = default;
  explicit Vector2(const SDL_Point &p);
  Vector2(Arithmetic auto _x, Arithmetic auto _y) : x(_x), y(_y) {}
  Vector2(const Vector2 &) = default;
  Vector2 &operator=(const Vector2 &) = default;
  Vector2 &operator+=(const Vector2 &oth);
  Vector2 &operator-=(const Vector2 &oth);
  Vector2 &operator*=(Arithmetic auto scalar) {
    x *= scalar;
    y *= scalar;
    return *this;
  }
  Vector2 &operator/=(Arithmetic auto scalar) {
    x /= scalar;
    y /= scalar;
    return *this;
  }
  Vector2 operator+(const Vector2 &oth) const;
  Vector2 operator-() const;
  Vector2 operator-(const Vector2 &oth) const;
  Vector2 operator*(Arithmetic auto scalar) const {
    return Vector2(scalar * x, scalar * y);
  }

  Vector2 operator/(Arithmetic auto scalar) const {
    return {x / scalar, y / scalar};
  }

  bool operator==(const Vector2 &oth) const;
  /**
   * @return length of the vector
   */
  double magnitude() const;
  /**
   * @return the squared value of the length of the vector
   */
  double sqr_magnitude() const;
  /**
   * @return a normalized vector (a vector with length 1 unit) that have the
   * same direction with the original
   */
  Vector2 normalize() const;
  /**
   *  normalize the vector
   */
  void normalize_ip();
  /**
   * @return dot product between 2 vector
   */
  double dot(const Vector2 &oth) const;
  /**
   * @return angle in degree to another vector in range [0,180] degrees, which
   * is the smallest of 2 angle
   * */
  double angle_to(const Vector2 &oth) const;

  /**
   * @return a vector that rotated deg degrees counter clockwise
   * */
  Vector2 rotate(Arithmetic auto deg) const {
    double angleInRadians = degree_to_radian(deg);
    return Vector2(x * std::cos(angleInRadians) - y * std::sin(angleInRadians),
                   x * std::sin(angleInRadians) + y * std::cos(angleInRadians));
  }

  /**
   *  make the vector rotate deg degrees counter-clockwise
   */
  void rotate_ip(Arithmetic auto deg) {
    double _x = x, _y = y;
    double angleInRadians = degree_to_radian(deg);
    x = _x * std::cos(angleInRadians) - _y * std::sin(angleInRadians);
    y = _x * std::sin(angleInRadians) + _y * std::cos(angleInRadians);
  }
  /**
   * @return distance between 2 point
   */
  double distance_to(const Vector2 &oth) const;
  /**
   * @return the vector that is the reflection of the current vector to a normal
   * vector
   */
  Vector2 reflect(const Vector2 &normal) const;
  /**
   * reflect the vector through a normal vector
   */
  void reflect_ip(const Vector2 &normal);
  /**
   * @return a projected vector from this vector to a normal vector
   */
  Vector2 project(const Vector2 &normal) const;
  /**
   * project the vector onto a normal vector
   */
  void project_ip(const Vector2 &normal);
  std::string toString() const;
  SDL_FPoint to_SDL_FPoint() const;
};

Vector2 operator*(Arithmetic auto scalar, const Vector2 &v) {
  return v * scalar;
}

} // namespace sdlgame::math

#endif