#ifndef FEMP_SURFACELOADOPERATORS_CONSTANTLOAD_HPP
#define FEMP_SURFACELOADOPERATORS_CONSTANTLOAD_HPP


#include <libfemp/SurfaceLoadOperators/SurfaceLoadOperator.h++>
#include <libfemp/Point3D.h++>

namespace fem
{

/**
Applies a constant load in relation to the global coordinate system
**/
class ConstantLoad
    : public SurfaceLoadOperator
{
protected:
    fem::Point3D m_force;

public:
    ConstantLoad();

    void setLoad(fem::Point3D const &force);

    void operator() (fem::SurfaceLoad &surface_load, fem::Model &model) override;
};

}	// namespace fem

#endif
