#include "GradientFieldFlyweightFactory.h++"

#include <iostream>


GradientFieldPolicy * 
GradientFieldFlyweightFactory::neutral()	
{ 
	return &m_neutral_policy; 
}


GradientFieldPolicy * 
GradientFieldFlyweightFactory::constant(float value)	
{ 
	m_constant_policy.value = value; 
	return &m_constant_policy; 
}


GradientFieldPolicy * 
GradientFieldFlyweightFactory::strains11()
{ 
	return &m_strain11_policy; 
}

GradientFieldPolicy * 
GradientFieldFlyweightFactory::strains22()
{ 
	return &m_strain22_policy; 
}

GradientFieldPolicy * 
GradientFieldFlyweightFactory::strains33()
{ 
	return &m_strain33_policy; 
}


GradientFieldPolicy * 
GradientFieldFlyweightFactory::strains12()
{ 
	return &m_strain12_policy; 
}


GradientFieldPolicy * 
GradientFieldFlyweightFactory::strains23()
{ 
	return &m_strain23_policy; 
}


GradientFieldPolicy * 
GradientFieldFlyweightFactory::strains13()
{ 
	return &m_strain13_policy; 
}


void 
GradientFieldFlyweightFactory::setModel(fem::Model &model)
{
	m_neutral_policy.setModel(model);	// test pattern
	m_constant_policy.setModel(model);

	m_strain11_policy.setModel(model);
	m_strain22_policy.setModel(model);
	m_strain33_policy.setModel(model);
	m_strain12_policy.setModel(model);
	m_strain23_policy.setModel(model);
	m_strain13_policy.setModel(model);
}


void 
GradientFieldFlyweightFactory::setAnalysisResult(fem::AnalysisResult<double> &result)
{
	std::cout << "GradientFieldFlyweightFactory::setAnalysisResult(fem::AnalysisResult<double> &result)" << std::endl;
	m_neutral_policy.setAnalysisResult(result);	// test pattern
	m_constant_policy.setAnalysisResult(result);

	m_strain11_policy.setAnalysisResult(result);
	m_strain22_policy.setAnalysisResult(result);
	m_strain33_policy.setAnalysisResult(result);
	m_strain12_policy.setAnalysisResult(result);
	m_strain23_policy.setAnalysisResult(result);
	m_strain13_policy.setAnalysisResult(result);
}


