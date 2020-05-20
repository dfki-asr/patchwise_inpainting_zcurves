#include "stdafx.h"

#include "PatchSelectionStatistics.h"

namespace ettention
{
    namespace inpainting
    {

		StepReport::StepReport()
			: depth(0), decision(EMPTY), samplesExcluded(0), searchSteps(0), distanceToFarBox(0), bestDistanceAtDecision(0)
		{
			child.reserve(2);
		}

		StepReport::~StepReport()
		{

		}

		void StepReport::outputTextReport(std::ofstream& ofs)
		{
			ofs << "<step depth=\""<< depth  << "\" interval_size=\"" << datasetInterval << "\">" << std::endl;
			ofs << "<operation value=\"";
			if (decision == EMPTY)
				ofs << "empty\" />" << std::endl;
			else if (decision == TEST_INSIDE)
				ofs << "test_inside\" />" << std::endl;
			else if (decision == VISIT_LEFT)
				ofs << "left_only\" />" << std::endl;
			else if (decision == VISIT_RIGHT)
				ofs << "right_only\" />" << std::endl;
			else if (decision == VISIT_LEFT_RIGHT)
				ofs << "left_then_right\" />" << std::endl;
			else if (decision == VISIT_RIGHT_LEFT)
				ofs << "right_then_left\" />" << std::endl;
			else
				ofs << "unknown\" />" << std::endl;

			if (decision == VISIT_LEFT || decision == VISIT_RIGHT || decision == VISIT_LEFT_RIGHT || decision == VISIT_RIGHT_LEFT )
			{
				ofs << "<excluded_points value=\"" << samplesExcluded << "\" />" << std::endl;
				ofs << "<far_distance value=\"" << distanceToFarBox << "\" />" << std::endl;
				ofs << "<distance_to_best_datapoint value=\"" << bestDistanceAtDecision << "\" />" << std::endl;
			}

			for (size_t i = 0; i < child.size(); i++)
				if (child[i] != nullptr)
					child[i]->outputTextReport(ofs);
			ofs << "</step>" << std::endl;
		}

		QueryReport::QueryReport()
		{
			numberOfTraversalSteps = 0;
			numberOfSamplesExcluded = 0;
			numberOfSamplesTested = 0;
		}

		QueryReport::~QueryReport()
		{
			for ( auto it : steps )
				delete it;
		}

		void QueryReport::addStep(StepReport* step )
		{
			numberOfTraversalSteps++;
			if (step == nullptr)
				return;

			std::unique_lock<std::mutex> lock(stepMutex);
			steps.push_back(step);
		}

		int QueryReport::totalStepsTaken()
		{
			return (int) steps.size();
		}

		int QueryReport::maxDepthReached()
		{
			int maxDepth = 0;
			for (size_t i = 0; i < steps.size(); i++)
				if (steps[i]->depth > maxDepth)
					maxDepth = steps[i]->depth;
			return maxDepth;
		}

		int QueryReport::totalPointsExcluded()
		{
			int excluded = 0;
			for (size_t i = 0; i < steps.size(); i++)
				excluded += steps[i]->samplesExcluded;
			return excluded;
		}

		int QueryReport::totalPointsTested()
		{
			int tested = 0;
			for (size_t i = 0; i < steps.size(); i++)
				if (steps[i]->decision == StepReport::TEST_INSIDE)
					tested += steps[i]->datasetInterval.length();
			return tested;
		}

		void QueryReport::outputTextReport(std::ofstream& ofs, bool logStepWise )
		{
			ofs << "<nearestneighborquery>" << std::endl;
			if (logStepWise)
			{
				ofs << "<steps>" << std::endl;
				if (steps.size() > 0)
					steps[0]->outputTextReport(ofs);
				ofs << "</steps>" << std::endl;
			}

			ofs << "<target_patch value=\"" << targetPatchUpperLeft << "\"/>" << std::endl;
			ofs << "<selected_patch value=\"" << bestPatchUpperLeft << "\"/>" << std::endl;
			std::cout.precision(2);
			ofs << "<norm value=\"" << normOfSelectedPatch << "\"/>" << std::endl;

			ofs << "<steps_taken value=\"" << numberOfTraversalSteps << "\"/>" << std::endl;
			ofs << "<data_points_excluded value=\"" << numberOfSamplesExcluded << "\"/>" << std::endl;
			ofs << "<data_points_tested value=\"" << numberOfSamplesTested << "\"/>" << std::endl;

			ofs << "</nearestneighborquery>" << std::endl;
		}

		TraversalReport::TraversalReport( std::string filename, bool logStepWise)
			: filename(filename)
			, logStepWise(logStepWise)
		{
			std::ofstream ofs;
			ofs.open(filename, std::ofstream::trunc);
			ofs << "<?xml version=\"1.0\" ?>" << std::endl;
			ofs.close();
		}

		TraversalReport::~TraversalReport()
		{
			for ( auto it : queries )
				delete it;
		}

		void TraversalReport::startQuery()
		{
			queries.push_back( new QueryReport() );
		}

		ettention::inpainting::QueryReport& TraversalReport::currentQuery()
		{
			return *queries.back();
		}

		void TraversalReport::outputTextReport(std::ofstream& ofs)
		{
			for (auto it : queries)
			{
				it->outputTextReport( ofs, logStepWise );
			}
		}

		void TraversalReport::outputTextReport()
		{
			std::ofstream ofs;
			ofs.open(filename, std::ofstream::app );
			outputTextReport(ofs);
			ofs.close();

			for (auto it : queries)
				delete it;
			queries.clear();
		}

		bool TraversalReport::shoudLogStepWise()
		{
			return logStepWise;
		}

	} // namespace
} // namespace
