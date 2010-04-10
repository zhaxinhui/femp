#include "DisplayOptions.h++"


DisplayOptions::DisplayOptions()
{
	setDefaultOptions();
}


DisplayOptions::~DisplayOptions()
{
}


void DisplayOptions::setDefaultOptions()
{
	load_pattern = NULL;
	nodes = 1;
	surfaces = 1;
	wireframe = 1;

	nodal_forces = 0;
	surface_forces = 0;
	domain_forces =  0;
	nodal_displacements = 0;
}