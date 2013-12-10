/* === S Y N F I G ========================================================= */
/*!	\file transformation.h
**	\brief Affine Transformation
**
**	$Id$
**
**	\legal
**	......... ... 2013 Ivan Mahonin
**
**	This package is free software; you can redistribute it and/or
**	modify it under the terms of the GNU General Public License as
**	published by the Free Software Foundation; either version 2 of
**	the License, or (at your option) any later version.
**
**	This package is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
**	General Public License for more details.
**	\endlegal
*/
/* ========================================================================= */

/* === S T A R T =========================================================== */

#ifndef __SYNFIG_TRANSFORMATION_H
#define __SYNFIG_TRANSFORMATION_H

/* === H E A D E R S ======================================================= */

#include "vector.h"
#include "matrix.h"
#include "rect.h"

/* === M A C R O S ========================================================= */

/* === T Y P E D E F S ===================================================== */

/* === C L A S S E S & S T R U C T S ======================================= */

namespace synfig {

/*!	\class Transformation
**	\todo writeme
*/
class Transformation
{
public:
	Vector offset;
	Angle angle;
	Angle skew_angle;
	Vector scale;

	Transformation():
		offset(0.0, 0.0),
		angle(Angle::rad(0.0)),
		skew_angle(Angle::rad(0.0)),
		scale(1.0, 1.0)
	{ }

	bool is_valid()const
	{
		return offset.is_valid()
		    && !isnan(Angle::rad(angle).get())
		    && !isnan(Angle::rad(skew_angle).get())
		    && scale.is_valid();
	}

	bool
	operator==(const Transformation &rhs)const
	{
		return offset==rhs.offset
			&& angle==rhs.angle
			&& skew_angle==rhs.skew_angle
			&& scale==rhs.scale;
	}

	bool
	operator!=(const Transformation &rhs)const
	{
		return offset!=rhs.offset
			|| angle!=rhs.angle
			|| skew_angle!=rhs.skew_angle
			|| scale!=rhs.scale;
	}

	bool is_equal_to(const Transformation& rhs)const
	{
		static const Angle::rad epsilon_angle(0.0000000000001);
		Angle::rad a = angle - rhs.angle;
		Angle::rad sa = skew_angle - rhs.skew_angle;
		return offset.is_equal_to(rhs.offset)
		    && a < epsilon_angle && a > -epsilon_angle
		    && sa < epsilon_angle && sa > -epsilon_angle
		    && scale.is_equal_to(rhs.scale);
	}

	Matrix get_matrix() const
	{
		Vector axis_x(scale[0], angle);
		Vector axis_y(scale[1], angle + skew_angle + Angle::deg(90.0));
		return Matrix(
			axis_x[0], axis_x[1], 0.0,
			axis_y[0], axis_y[1], 0.0,
			offset[0], offset[1], 1.0 );
	}

	void set_matrix(const Matrix &matrix)
	{
		Vector axis_x(matrix.m00, matrix.m01);
		Vector axis_y(matrix.m10, matrix.m11);
		angle = axis_x.angle();
		skew_angle = axis_y.angle() - angle - Angle::deg(90.0);
		scale[0] = axis_x.mag();
		scale[1] = axis_y.mag();
		offset[0] = matrix.m20;
		offset[1] = matrix.m21;
	}

	explicit Transformation(const Matrix &matrix)
		{ set_matrix(matrix); }

	Matrix get_inverted_matrix() const
		{ return get_matrix().invert(); }

	Transformation get_back_transformation() const
		{ return Transformation(get_inverted_matrix()); }

	static Rect transform_bounds(const Matrix &matrix, const Rect &bounds)
	{
		Rect transformed_bounds(
			matrix.get_transformed(
				Vector(bounds.minx, bounds.miny) ));
		transformed_bounds.expand(
			matrix.get_transformed(
				Vector(bounds.minx, bounds.maxy) ));
		transformed_bounds.expand(
			matrix.get_transformed(
				Vector(bounds.maxx, bounds.miny) ));
		transformed_bounds.expand(
			matrix.get_transformed(
				Vector(bounds.maxx, bounds.maxy) ));
		return transformed_bounds;
	}

	Vector transform(const Vector &v, bool translate = true) const
		{ return get_matrix().get_transformed(v, translate); }
	Transformation transform(const Transformation &transformation) const
		{ return Transformation( transformation.get_matrix()*get_matrix() ); }
	Rect transform_bounds(const Rect &bounds) const
		{ return transform_bounds(get_matrix(), bounds); }

	Vector back_transform(const Vector &v, bool translate = true) const
		{ return get_inverted_matrix().get_transformed(v, translate); }
	Transformation back_transform(const Transformation &transformation) const
		{ return Transformation( transformation.get_matrix()*get_inverted_matrix() ); }
	Rect back_transform_bounds(const Rect &bounds) const
		{ return transform_bounds(get_inverted_matrix(), bounds); }

	static const Transformation identity() { return Transformation(); }
};

}; // END of namespace synfig

/* === E N D =============================================================== */

#endif
