#pragma once

#include "setup/parameterset/ParameterSet.h"

namespace ettention
{
	class ParameterSource;

	namespace inpainting 
	{
		class CostFunctionOptions : public ParameterSet 
		{
		public:
			enum class CostFunctionType
			{
				L1,
				L2,
			};

			CostFunctionOptions();
			CostFunctionOptions(const ParameterSource* parameterSource);
			virtual ~CostFunctionOptions();

			CostFunctionType getCostFunctionType();

		protected:
			CostFunctionType parseCostFunctionType( std::string value );

		public:
			CostFunctionType costFunctionType;
		};
    }
}