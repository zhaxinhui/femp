#ifndef MDI_WINDOW_PROPERTIES_HPP
#define MDI_WINDOW_PROPERTIES_HPP


/*
This class is used by the UI routines in order to provide information regarding the features supported by each MDI window
*/
class MdiWindowProperties
{
	//TODO implement method to deal with program options changes, such as foreground color

	enum Type { MWP_Model, MWP_Displacements } type;
	public:
		MdiWindowProperties();
		~MdiWindowProperties();
};

#endif
