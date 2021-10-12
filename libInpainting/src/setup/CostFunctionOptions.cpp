#include "stdafx.h"

#include "setup/CostFunctionOptions.h"
#include "libCFG/setup/parametersource/ParameterSource.h"

namespace inpainting
{

	CostFunctionOptions::CostFunctionOptions()
	{
		costFunctionType = CostFunctionType::L2;
	}

	CostFunctionOptions::CostFunctionOptions(const libCFG::ParameterSource* parameterSource)
	{
		costFunctionType = CostFunctionType::L2;
		if (parameterSource->parameterExists("costFunction"))
		{
			costFunctionType = parseCostFunctionType( parameterSource->getStringParameter("costFunction") );
		}
	}

	CostFunctionOptions::~CostFunctionOptions()
	{

	}

	CostFunctionOptions::CostFunctionType CostFunctionOptions::getCostFunctionType()
	{
		return costFunctionType;
	}

	CostFunctionOptions::CostFunctionType CostFunctionOptions::parseCostFunctionType( std::string value )
	{
		if (value == "L1")
			return CostFunctionOptions::CostFunctionType::L1;
		if (value == "L2")
			return CostFunctionOptions::CostFunctionType::L2;
		if (value == "L2_with_weight")
			return CostFunctionOptions::CostFunctionType::L2WithWeight;
		throw std::invalid_argument("illegal CostFunctionType " + value );
	}

} // namespace

