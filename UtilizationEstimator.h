#pragma once

#include "freight.h"
#include "Cargo.h"
#include "Shipment.h"
#include <vector>
#include <string>
#include <unordered_map>

// Per-freight detailed analysis
struct FreightAnalysis {
    int freightIndex;
    std::string freightType;
    int capacity;
    int capacityUsed;
    double utilization;
    std::string status; // "Optimal", "Good", "Underutilized"
    
    FreightAnalysis()
        : freightIndex(0), capacity(0), capacityUsed(0), 
          utilization(0.0), status("Unknown") {}
};

// Utilization report with predictions and recommendations
struct UtilizationReport {
    double predictedUtilization;
    int recommendedFreights;
    std::vector<int> underutilizedFreights;
    std::vector<int> overloadedCargo;
    std::vector<std::string> recommendations;
    std::vector<FreightAnalysis> freightDetails;
    
    UtilizationReport()
        : predictedUtilization(0.0), recommendedFreights(0) {}
};

// Matching score for cargo-freight pairing
struct MatchingScore {
    int freightIndex;
    int cargoIndex;
    double totalScore;
    double destinationScore;
    double timeScore;
    double capacityScore;
    std::string reason;
    
    MatchingScore()
        : freightIndex(0), cargoIndex(0), totalScore(0.0),
          destinationScore(0.0), timeScore(0.0), capacityScore(0.0) {}
};

// Efficiency trend analysis
struct EfficiencyTrend {
    double recentAvgUtilization;    // Last 10 schedules
    double historicalAvgUtilization; // Last 50 schedules
    double trend;                    // Positive = improving, negative = declining
    std::string assessment;          // "Improving", "Stable", "Declining"
    
    EfficiencyTrend()
        : recentAvgUtilization(0.0), historicalAvgUtilization(0.0),
          trend(0.0), assessment("Unknown") {}
};

// Intelligent freight utilization estimator
class UtilizationEstimator {
public:
    UtilizationEstimator();
    
    // Main estimation function
    UtilizationReport estimateUtilization(
        const std::vector<freight>& freights,
        const std::vector<Cargo>& cargos);
    
    // AI-based matching prediction
    std::vector<MatchingScore> predictBestMatches(
        const std::vector<freight>& freights,
        const std::vector<Cargo>& cargos,
        int topN = 5);
    
    // Analyze existing schedule
    UtilizationReport analyzeSchedule(
        const std::vector<Shipment>& schedule);
    
    // Generate optimization suggestions
    std::vector<std::string> suggestOptimizations(
        const UtilizationReport& report);
    
    // Machine learning from historical data
    void learnFromSchedule(
        const std::vector<Shipment>& schedule);
    
    // Get efficiency trend
    EfficiencyTrend getEfficiencyTrend() const;
    
private:
    // Historical data for learning
    struct HistoricalData {
        std::vector<double> utilizationHistory;
        std::unordered_map<std::string, int> destinationFrequency;
        int totalSchedulesAnalyzed;
        
        HistoricalData() : totalSchedulesAnalyzed(0) {}
    };
    
    HistoricalData historicalData_;
    
    // Scoring algorithms
    double calculateMatchScore(
        const freight& f,
        const Cargo& c);
    
    double calculateTimeScore(
        const freight& f,
        const Cargo& c);
    
    double calculateCapacityScore(
        const freight& f,
        const Cargo& c);
    
    double calculateDestinationScore(
        const freight& f,
        const Cargo& c);
};
