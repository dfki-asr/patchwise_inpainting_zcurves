#pragma once

#include "setup/parameterset/ParameterSet.h"

namespace cfg {
	class ParameterSource;
}

namespace inpainting 
{
	class CostFunctionOptions : public cfg::ParameterSet 
	{
	public:
		enum class CostFunctionType
		{
			L1,
			L2,
			L2WithWeight,
		};

		CostFunctionOptions();
		CostFunctionOptions(const cfg::ParameterSource* parameterSource);
		virtual ~CostFunctionOptions();

		CostFunctionType getCostFunctionType();

	protected:
		CostFunctionType parseCostFunctionType( std::string value );

	public:
		CostFunctionType costFunctionType;
	};
}
