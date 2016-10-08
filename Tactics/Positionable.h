#pragma once


#ifndef __POSITIONABLE_H__
#define __POSITIONABLE_H__


// Interface for something that can be described in two dimensions

template <typename T>
struct Point2D {
	T x;
	T y;
};

template <typename T>
struct Point3D {
	T x;
	T y;
	T z;
};

template <typename T>
struct Positionable2D {
	void setX(T x) { _point2d.x = x; }
	void setY(T y) { _point2d.y = y; }
	T getX() const { return _point2d.x; }
	T getY() const { return _point2d.y; }
protected:
	Point2D<T> _point2d;
};

template <typename T>
struct Positionable3D {
	void setX(T x) { _point3d.x = x; }
	void setY(T y) { _point3d.y = y; }
	void setY(T z) { _point3d.z = z; }
	T getX() const { return _point3d.x; }
	T getY() const { return _point3d.y; }
	T getZ() const { return _point3d.z; }
protected:
	Point3D<T> _point3d;
};



#endif
