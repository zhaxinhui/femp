#include "TensorFieldViewport.h++"

#include "SceneGraph.h++"


TensorFieldViewport::TensorFieldViewport(fem::Project &project, fem::AnalysisResult<double> &result, fem::ResultsRanges<double> &ranges, QWidget *parent)
	: BaseViewport(project, parent)
{
	//TODO let the user choose which result to represent
	this->project = &project;
	this->m_analysis_result = NULL;
	this->setAnalysisResult(result);
	this->setResultsRanges(ranges);

	//TODO let the user select which analysis case to visualize

	this->showTensionField();

	// set this widget's load pattern pointer
	if(project.model.load_pattern_list.empty())
	{
		display_options.load_pattern = NULL;
	}
	else
	{
		// set the first load pattern
		display_options.load_pattern = &project.model.load_pattern_list.front();
	}
	this->setFocusPolicy(Qt::StrongFocus);
}


TensorFieldViewport::~TensorFieldViewport()
{
}


void
TensorFieldViewport::setAnalysisResult(fem::AnalysisResult<double> &result)
{
	this->m_analysis_result = &result;
	this->m_vp_state_tension_fields.setAnalysisResult(result);
}


void
TensorFieldViewport::setResultsRanges(fem::ResultsRanges<double> &ranges)
{
	this->m_vp_state_tension_fields.setResultsRanges(ranges);
}


void 
TensorFieldViewport::showTensionField()
{
	this->setState(&this->m_vp_state_tension_fields);
}


void 
TensorFieldViewport::showNegativePrincipalStressesVisibility(bool state)
{
	m_vp_state_tension_fields.showNegativePrincipalStressesVisibility(state);
}


void 
TensorFieldViewport::showPositivePrincipalStressesVisibility(bool state)
{
	m_vp_state_tension_fields.showPositivePrincipalStressesVisibility(state);
}


