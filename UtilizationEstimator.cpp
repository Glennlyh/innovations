#include "UtilizationEstimator.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <numeric>

using namespace std;

UtilizationEstimator::UtilizationEstimator() {
}

double UtilizationEstimator::calculateDestinationScore(
    const freight& f,
    const Cargo& c) {
    
    // Perfect match if destinations are the same
    if (f.getDestination() == c.getDestination()) {
        return 100.0;
    }
    return 0.0;
}

double UtilizationEstimator::calculateTimeScore(
    const freight& f,
    const Cargo& c) {
    
    int timeDiff = abs(f.getTime() - c.getTime());
    
    // Perfect score if times match exactly
    if (timeDiff == 0) {
        return 100.0;
    }
    
    // Freight should arrive at or after cargo time
    if (f.getTime() >= c.getTime()) {
        // Score decreases with time difference
        // 100 at 0 diff, 50 at 5 hours, 0 at 10+ hours
        return max(0.0, 100.0 - (timeDiff * 10.0));
    } else {
        // Freight arrives before cargo is ready - not ideal
        return max(0.0, 50.0 - (timeDiff * 10.0));
    }
}

double UtilizationEstimator::calculateCapacityScore(
    const freight& f,
    const Cargo& c) {
    
    int freightCapacity = f.getCapacity();
    int cargoCapacity = c.getCapacity();
    
    if (cargoCapacity > freightCapacity) {
        // Cargo won't fit - very bad score
        return 0.0;
    }
    
    // Best score when cargo uses 80-100% of freight capacity
    double utilizationRatio = (double)cargoCapacity / freightCapacity;
    
    if (utilizationRatio >= 0.8) {
        return 100.0;
    } else if (utilizationRatio >= 0.5) {
        // Good utilization
        return 50.0 + (utilizationRatio - 0.5) * 100.0;
    } else {
        // Poor utilization
        return utilizationRatio * 100.0;
    }
}

double UtilizationEstimator::calculateMatchScore(
    const freight& f,
    const Cargo& c) {
    
    // Multi-factor scoring with weights:
    // Destination: 40%, Time: 30%, Capacity: 30%
    double destScore = calculateDestinationScore(f, c);
    double timeScore = calculateTimeScore(f, c);
    double capacityScore = calculateCapacityScore(f, c);
    
    return (destScore * 0.4) + (timeScore * 0.3) + (capacityScore * 0.3);
}

vector<MatchingScore> UtilizationEstimator::predictBestMatches(
    const vector<freight>& freights,
    const vector<Cargo>& cargos,
    int topN) {
    
    cout << "\n🤖 AI-Based Matching Prediction\n";
    cout << "================================\n";
    
    vector<MatchingScore> allScores;
    
    // Calculate scores for all possible matches
    for (const auto& f : freights) {
        for (const auto& c : cargos) {
            MatchingScore score;
            score.freightIndex = f.getIndex();
            score.cargoIndex = c.getIndex();
            score.destinationScore = calculateDestinationScore(f, c);
            score.timeScore = calculateTimeScore(f, c);
            score.capacityScore = calculateCapacityScore(f, c);
            score.totalScore = calculateMatchScore(f, c);
            
            // Generate reason
            if (score.totalScore >= 80.0) {
                score.reason = "Excellent match - optimal on all factors";
            } else if (score.totalScore >= 60.0) {
                score.reason = "Good match - minor compromises";
            } else if (score.totalScore >= 40.0) {
                score.reason = "Acceptable match - significant compromises";
            } else {
                score.reason = "Poor match - consider alternatives";
            }
            
            allScores.push_back(score);
        }
    }
    
    // Sort by total score descending
    sort(allScores.begin(), allScores.end(),
        [](const MatchingScore& a, const MatchingScore& b) {
            return a.totalScore > b.totalScore;
        });
    
    // Return top N matches
    vector<MatchingScore> topMatches;
    int count = min(topN, (int)allScores.size());
    
    cout << "\nTop " << count << " Predicted Matches:\n";
    for (int i = 0; i < count; i++) {
        const auto& score = allScores[i];
        cout << "\n" << (i + 1) << ". Freight F" << score.freightIndex 
             << " ↔ Cargo C" << score.cargoIndex << "\n";
        cout << "   Overall Score: " << score.totalScore << "/100\n";
        cout << "   • Destination: " << score.destinationScore << "/100\n";
        cout << "   • Time Compatibility: " << score.timeScore << "/100\n";
        cout << "   • Capacity Fit: " << score.capacityScore << "/100\n";
        cout << "   Reason: " << score.reason << "\n";
        
        topMatches.push_back(score);
    }
    
    return topMatches;
}

UtilizationReport UtilizationEstimator::estimateUtilization(
    const vector<freight>& freights,
    const vector<Cargo>& cargos) {
    
    cout << "\n📊 Intelligent Utilization Estimation\n";
    cout << "=====================================\n";
    
    UtilizationReport report;
    
    if (freights.empty()) {
        cout << "⚠️  No freights available for estimation.\n";
        report.recommendations.push_back("Add freights to the system");
        return report;
    }
    
    if (cargos.empty()) {
        cout << "⚠️  No cargos available for estimation.\n";
        report.recommendations.push_back("Add cargos to schedule");
        return report;
    }
    
    // Calculate total capacities
    int totalFreightCapacity = 0;
    for (const auto& f : freights) {
        totalFreightCapacity += f.getCapacity();
    }
    
    int totalCargoCapacity = 0;
    for (const auto& c : cargos) {
        totalCargoCapacity += c.getCapacity();
    }
    
    cout << "\n📦 Capacity Analysis:\n";
    cout << "  Total Freight Capacity: " << totalFreightCapacity << " units\n";
    cout << "  Total Cargo Volume: " << totalCargoCapacity << " units\n";
    
    // Predict utilization
    if (totalFreightCapacity > 0) {
        report.predictedUtilization = 
            (double)totalCargoCapacity / totalFreightCapacity * 100.0;
    }
    
    cout << "  Predicted Utilization: " << report.predictedUtilization << "%\n";
    
    // Estimate recommended number of freights
    report.recommendedFreights = freights.size();
    if (totalFreightCapacity > 0) {
        // Try to achieve 85% utilization
        int idealCapacity = (int)(totalCargoCapacity / 0.85);
        
        // Estimate freights needed (rough approximation)
        int avgFreightCapacity = totalFreightCapacity / freights.size();
        if (avgFreightCapacity > 0) {
            report.recommendedFreights = 
                (idealCapacity + avgFreightCapacity - 1) / avgFreightCapacity;
        }
    }
    
    // Generate recommendations
    if (report.predictedUtilization < 60.0) {
        report.recommendations.push_back(
            "⚠️  Low utilization predicted. Consider using fewer or larger freights.");
    }
    
    if (totalCargoCapacity > totalFreightCapacity) {
        int shortage = totalCargoCapacity - totalFreightCapacity;
        report.recommendations.push_back(
            "❌ Insufficient capacity! Need " + to_string(shortage) + 
            " more units of freight capacity.");
        report.overloadedCargo.push_back(-1); // Indicator
    }
    
    if (report.predictedUtilization >= 85.0 && report.predictedUtilization <= 95.0) {
        report.recommendations.push_back(
            "✅ Excellent utilization predicted! Schedule is well-optimized.");
    }
    
    if (report.predictedUtilization > 95.0) {
        report.recommendations.push_back(
            "⚠️  Very high utilization. Consider adding buffer capacity for flexibility.");
    }
    
    // Group analysis by destination
    unordered_map<string, int> freightCapByDest;
    unordered_map<string, int> cargoCapByDest;
    
    for (const auto& f : freights) {
        freightCapByDest[f.getDestination()] += f.getCapacity();
    }
    
    for (const auto& c : cargos) {
        cargoCapByDest[c.getDestination()] += c.getCapacity();
    }
    
    cout << "\n🗺️  Destination Analysis:\n";
    for (const auto& pair : cargoCapByDest) {
        const string& dest = pair.first;
        int cargoNeed = pair.second;
        int freightAvail = freightCapByDest[dest];
        
        cout << "  " << dest << ": ";
        cout << "Cargo=" << cargoNeed << " units, ";
        cout << "Freight=" << freightAvail << " units";
        
        if (freightAvail < cargoNeed) {
            cout << " ❌ SHORTAGE!";
            report.recommendations.push_back(
                "Add more freight capacity to " + dest);
        } else if (freightAvail > cargoNeed * 1.5) {
            cout << " ⚠️  Excess capacity";
        } else {
            cout << " ✓";
        }
        cout << "\n";
    }
    
    cout << "\n💡 Recommendations:\n";
    if (report.recommendations.empty()) {
        cout << "  ✓ No major issues detected\n";
    } else {
        for (const auto& rec : report.recommendations) {
            cout << "  • " << rec << "\n";
        }
    }
    
    return report;
}

UtilizationReport UtilizationEstimator::analyzeSchedule(
    const vector<Shipment>& schedule) {
    
    cout << "\n📊 Schedule Utilization Analysis\n";
    cout << "=================================\n";
    
    UtilizationReport report;
    
    if (schedule.empty()) {
        cout << "⚠️  Empty schedule - nothing to analyze.\n";
        return report;
    }
    
    // Group shipments by freight
    unordered_map<int, int> capacityUsed;
    unordered_map<int, const freight*> freightMap;
    
    for (const auto& shipment : schedule) {
        int freightIdx = shipment.getFreightIndex();
        capacityUsed[freightIdx] += shipment.getCargoQuantity();
        freightMap[freightIdx] = &shipment.getFreight();
    }
    
    // Analyze each freight
    int totalCapacity = 0;
    int totalUsed = 0;
    int underutilizedCount = 0;
    
    cout << "\n🚚 Per-Freight Analysis:\n";
    for (const auto& pair : freightMap) {
        int freightIdx = pair.first;
        const freight* f = pair.second;
        
        FreightAnalysis analysis;
        analysis.freightIndex = freightIdx;
        analysis.freightType = f->getTypeName();
        analysis.capacity = f->getCapacity();
        analysis.capacityUsed = capacityUsed[freightIdx];
        analysis.utilization = 
            (double)analysis.capacityUsed / analysis.capacity * 100.0;
        
        // Classify status
        if (analysis.utilization >= 85.0) {
            analysis.status = "Optimal";
        } else if (analysis.utilization >= 50.0) {
            analysis.status = "Good";
        } else {
            analysis.status = "Underutilized";
            underutilizedCount++;
            report.underutilizedFreights.push_back(freightIdx);
        }
        
        cout << "\n  Freight F" << freightIdx << " (" << analysis.freightType << "):\n";
        cout << "    Capacity: " << analysis.capacity << " units\n";
        cout << "    Used: " << analysis.capacityUsed << " units\n";
        cout << "    Utilization: " << analysis.utilization << "%\n";
        cout << "    Status: " << analysis.status;
        
        if (analysis.status == "Optimal") {
            cout << " ✅\n";
        } else if (analysis.status == "Good") {
            cout << " ✓\n";
        } else {
            cout << " ⚠️\n";
        }
        
        report.freightDetails.push_back(analysis);
        totalCapacity += analysis.capacity;
        totalUsed += analysis.capacityUsed;
    }
    
    report.predictedUtilization = (double)totalUsed / totalCapacity * 100.0;
    report.recommendedFreights = freightMap.size();
    
    cout << "\n📈 Overall Statistics:\n";
    cout << "  Total Freights: " << freightMap.size() << "\n";
    cout << "  Total Capacity: " << totalCapacity << " units\n";
    cout << "  Total Used: " << totalUsed << " units\n";
    cout << "  Average Utilization: " << report.predictedUtilization << "%\n";
    
    // Generate recommendations
    if (underutilizedCount > 0) {
        double underutilizedPercent = 
            (double)underutilizedCount / freightMap.size() * 100.0;
        
        if (underutilizedPercent > 50.0) {
            report.recommendations.push_back(
                "⚠️  Over 50% of freights are underutilized. " 
                "Consider consolidating to fewer, larger freights.");
        } else {
            report.recommendations.push_back(
                "ℹ️  Some freights are underutilized. Review scheduling strategy.");
        }
    }
    
    if (report.predictedUtilization >= 85.0 && report.predictedUtilization <= 95.0) {
        report.recommendations.push_back(
            "✅ Excellent utilization! Schedule is well-optimized.");
    } else if (report.predictedUtilization > 95.0) {
        report.recommendations.push_back(
            "⚠️  Very high utilization. Consider adding buffer capacity.");
    }
    
    cout << "\n💡 Recommendations:\n";
    if (report.recommendations.empty()) {
        cout << "  ✓ No major issues detected\n";
    } else {
        for (const auto& rec : report.recommendations) {
            cout << "  • " << rec << "\n";
        }
    }
    
    return report;
}

vector<string> UtilizationEstimator::suggestOptimizations(
    const UtilizationReport& report) {
    
    vector<string> suggestions;
    
    if (report.predictedUtilization < 60.0) {
        suggestions.push_back("Use 'Minimize Freights' strategy to consolidate cargo");
    }
    
    if (!report.underutilizedFreights.empty()) {
        suggestions.push_back("Remove or reallocate underutilized freights");
    }
    
    if (!report.overloadedCargo.empty()) {
        suggestions.push_back("Add more freight capacity to handle excess cargo");
    }
    
    if (report.predictedUtilization > 95.0) {
        suggestions.push_back("Add 10-15% buffer capacity for operational flexibility");
    }
    
    return suggestions;
}

void UtilizationEstimator::learnFromSchedule(
    const vector<Shipment>& schedule) {
    
    if (schedule.empty()) return;
    
    // Calculate utilization for this schedule
    unordered_map<int, int> capacityUsed;
    unordered_map<int, int> capacityTotal;
    
    for (const auto& shipment : schedule) {
        int freightIdx = shipment.getFreightIndex();
        capacityUsed[freightIdx] += shipment.getCargoQuantity();
        capacityTotal[freightIdx] = shipment.getFreight().getCapacity();
    }
    
    int totalUsed = 0;
    int totalCapacity = 0;
    
    for (const auto& pair : capacityTotal) {
        totalCapacity += pair.second;
        totalUsed += capacityUsed[pair.first];
    }
    
    if (totalCapacity > 0) {
        double utilization = (double)totalUsed / totalCapacity * 100.0;
        historicalData_.utilizationHistory.push_back(utilization);
        
        // Keep only last 50 schedules
        if (historicalData_.utilizationHistory.size() > 50) {
            historicalData_.utilizationHistory.erase(
                historicalData_.utilizationHistory.begin());
        }
    }
    
    // Track destination frequency
    for (const auto& shipment : schedule) {
        string dest = shipment.getFreightDes();
        historicalData_.destinationFrequency[dest]++;
    }
    
    historicalData_.totalSchedulesAnalyzed++;
}

EfficiencyTrend UtilizationEstimator::getEfficiencyTrend() const {
    EfficiencyTrend trend;
    
    if (historicalData_.utilizationHistory.empty()) {
        trend.assessment = "Insufficient data";
        return trend;
    }
    
    // Calculate recent average (last 10)
    int recentCount = min(10, (int)historicalData_.utilizationHistory.size());
    double recentSum = 0.0;
    
    for (int i = historicalData_.utilizationHistory.size() - recentCount; 
         i < (int)historicalData_.utilizationHistory.size(); i++) {
        recentSum += historicalData_.utilizationHistory[i];
    }
    trend.recentAvgUtilization = recentSum / recentCount;
    
    // Calculate historical average (all data)
    double historicalSum = accumulate(
        historicalData_.utilizationHistory.begin(),
        historicalData_.utilizationHistory.end(),
        0.0);
    trend.historicalAvgUtilization = 
        historicalSum / historicalData_.utilizationHistory.size();
    
    // Calculate trend
    trend.trend = trend.recentAvgUtilization - trend.historicalAvgUtilization;
    
    if (trend.trend > 5.0) {
        trend.assessment = "Improving";
    } else if (trend.trend < -5.0) {
        trend.assessment = "Declining";
    } else {
        trend.assessment = "Stable";
    }
    
    return trend;
}
