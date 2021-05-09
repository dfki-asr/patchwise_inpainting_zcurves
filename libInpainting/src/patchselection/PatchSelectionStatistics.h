#pragma once

#include "libmmv/math/Vec3.h"
#include "IndexInterval.h"

namespace inpainting {

	class StepReport
	{
	public:
		enum Decision
		{
			EMPTY, TEST_INSIDE, VISIT_LEFT, VISIT_RIGHT, VISIT_LEFT_RIGHT, VISIT_RIGHT_LEFT
		};

		StepReport();
		~StepReport();

		void outputTextReport(std::ofstream& ofs);

		int depth;
		Decision decision;
		IndexInterval datasetInterval;
		float distanceToFarBox;
		float bestDistanceAtDecision;
		int samplesExcluded;
		int searchSteps;
		std::vector<StepReport*> child;
	};

	class QueryReport
	{
	public:
		QueryReport();
		~QueryReport();

		void addStep( StepReport* step );

		int totalStepsTaken();
		int maxDepthReached();
		int totalPointsExcluded();
		int totalPointsTested();

		void outputTextReport(std::ofstream& ofs, bool logStepWise );

		std::vector<StepReport*> steps;

		libmmv::Vec3ui targetPatchUpperLeft;
		libmmv::Vec3ui bestPatchUpperLeft;
		float normOfSelectedPatch;

		int numberOfSamplesExcluded;
		int numberOfSamplesTested;
		int numberOfTraversalSteps;

		std::mutex stepMutex;
	};

	class TraversalReport
	{
	public:
		TraversalReport( std::string filename, bool logStepWise );
		~TraversalReport();

		void startQuery();
		QueryReport& currentQuery();

		void outputTextReport(std::ofstream& ofs);
		void outputTextReport();

		bool shoudLogStepWise();

		std::vector<QueryReport*> queries;

	protected:
		std::string filename;
		bool logStepWise;
	};

}
