#pragma once

#include "libCFG/setup/parameterset/ParameterSet.h"

namespace libCFG {
	class ParameterSource;
}

namespace inpainting 
{
	class CostFunctionOptions : public libCFG::ParameterSet 
	{
	public:
		enum class CostFunctionType
		{
			L1,
			L2,
			L2WithWeight,
		};

		CostFunctionOptions();
		CostFunctionOptions(const libCFG::ParameterSource* parameterSource);
		virtual ~CostFunctionOptions();

		CostFunctionType getCostFunctionType();

	protected:
		CostFunctionType parseCostFunctionType( std::string value );

	public:
		CostFunctionType costFunctionType;
	};
}
