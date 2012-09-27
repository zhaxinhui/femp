#ifndef STRESS22_GRADIENT_FIELD_POLICY_H
#define STRESS22_GRADIENT_FIELD_POLICY_H

#include "GradientFieldPolicy.h++"


/**
Stress22 gradient field policy
**/
class Stress22GradientFieldPolicy
	: public GradientFieldPolicy
{
public:
	Stress22GradientFieldPolicy();

protected:
	float val(fem::element_ref_t const &ref, gradient_index_t const &p) const;
	float maxVal(fem::ResultsRanges<double> const &) const;
	float minVal(fem::ResultsRanges<double> const &) const;

};


#endif

