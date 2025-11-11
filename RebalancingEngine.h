#pragma once

#include "freight.h"
#include "Cargo.h"
#include "Shipment.h"
#include "ScheduleBuilder.h"
#include <vector>
#include <string>
#include <unordered_map>

// Metrics for measuring rebalancing performance
struct RebalanceMetrics {
    int freightsUsed;
    int totalCargoReassigned;
    double avgUtilization;
    int timeImpact;
    int totalShipments;
    
    RebalanceMetrics() 
        : freightsUsed(0), totalCargoReassigned(0), avgUtilization(0.0), 
          timeImpact(0), totalShipments(0) {}
};

// Automatic rebalancing engine for cargo redistribution
class RebalancingEngine {
public:
    enum class RebalancingStrategy {
        MINIMIZE_FREIGHTS,      // Use fewer freights
        MINIMIZE_TIME,          // Prioritize earliest arrival
        MAXIMIZE_UTILIZATION    // Fill freights to capacity
    };

    RebalancingEngine();
    
    // Main rebalancing function
    std::vector<Shipment> rebalanceAfterRemoval(
        int removedFreightIndex,
        const std::vector<Shipment>& currentSchedule,
        const std::vector<freight>& allFreights,
        const std::vector<Cargo>& allCargos);
    
    // Get cargos affected by freight removal
    std::vector<Cargo> getAffectedCargos(
        int removedFreightIndex,
        const std::vector<Shipment>& currentSchedule,
        const std::vector<Cargo>& allCargos);
    
    // Find alternative freights for cargo
    std::vector<freight> findAlternativeFreights(
        const Cargo& cargo,
        const std::vector<freight>& allFreights,
        const std::vector<Shipment>& currentSchedule);
    
    // Calculate performance metrics
    RebalanceMetrics calculateMetrics(
        const std::vector<Shipment>& newSchedule,
        const std::vector<Shipment>& originalSchedule,
        int removedFreightIndex);
    
    // Strategy setters and getters
    void setStrategy(RebalancingStrategy strategy);
    RebalancingStrategy getStrategy() const;
    
private:
    RebalancingStrategy strategy_;
    
    // Helper methods
    bool isFreightAvailable(
        const freight& f,
        int removedFreightIndex,
        const std::vector<Shipment>& currentSchedule);
    
    int calculateRemainingCapacity(
        const freight& f,
        const std::vector<Shipment>& currentSchedule);
    
    void sortFreightsByStrategy(
        std::vector<freight>& freights,
        const Cargo& cargo);
};
