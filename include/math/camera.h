#pragma once
#include "math/mat4.h"
#include "math/vec.h"

static Mat4 lookAt(Vec3 eye, Vec3 target, Vec3 up) {
	Vec3 f = (eye - target).normalize();
	Vec3 r = (up.cross(f)).normalize();
	Vec3 u = f.cross(r);
	Mat4 view;
	view.m[0] = r.x;   view.m[4] = r.y;    view.m[8] = r.z;      view.m[12] = -r.dot(eye);
	view.m[1] = u.x;   view.m[5] = u.y;    view.m[9] = u.z;      view.m[13] = -u.dot(eye);
	view.m[2] = f.x;   view.m[6] = f.y;    view.m[10] = f.z;     view.m[14] = -f.dot(eye);
	view.m[3] = 0.0f;  view.m[7] = 0.0f;   view.m[11] = 0.0f;    view.m[15] = 1.0f;

	return view;
}