#include "RebalancingEngine.h"
#include <algorithm>
#include <iostream>
#include <unordered_set>
#include <map>

using namespace std;

RebalancingEngine::RebalancingEngine()
    : strategy_(RebalancingStrategy::MINIMIZE_FREIGHTS) {
}

void RebalancingEngine::setStrategy(RebalancingStrategy strategy) {
    strategy_ = strategy;
}

RebalancingEngine::RebalancingStrategy RebalancingEngine::getStrategy() const {
    return strategy_;
}

vector<Cargo> RebalancingEngine::getAffectedCargos(
    int removedFreightIndex,
    const vector<Shipment>& currentSchedule,
    const vector<Cargo>& allCargos) {
    
    cout << "\n🔍 Identifying affected cargos from freight F" << removedFreightIndex << "...\n";
    
    unordered_set<int> affectedCargoIds;
    
    // Find all cargos that were on the removed freight
    for (const auto& shipment : currentSchedule) {
        if (shipment.getFreightIndex() == removedFreightIndex) {
            affectedCargoIds.insert(shipment.getCargoIndex());
        }
    }
    
    // Build list of affected cargo objects
    vector<Cargo> affectedCargos;
    for (const auto& cargo : allCargos) {
        if (affectedCargoIds.find(cargo.getIndex()) != affectedCargoIds.end()) {
            affectedCargos.push_back(cargo);
            cout << "  ⚠️  Cargo C" << cargo.getIndex() << " needs reassignment\n";
        }
    }
    
    cout << "  Total affected cargos: " << affectedCargos.size() << "\n";
    return affectedCargos;
}

bool RebalancingEngine::isFreightAvailable(
    const freight& f,
    int removedFreightIndex,
    const vector<Shipment>& currentSchedule) {
    
    // Can't use the removed freight
    if (f.getIndex() == removedFreightIndex) {
        return false;
    }
    
    return true;
}

int RebalancingEngine::calculateRemainingCapacity(
    const freight& f,
    const vector<Shipment>& currentSchedule) {
    
    int usedCapacity = 0;
    for (const auto& shipment : currentSchedule) {
        if (shipment.getFreightIndex() == f.getIndex()) {
            usedCapacity += shipment.getCargoQuantity();
        }
    }
    
    return f.getCapacity() - usedCapacity;
}

void RebalancingEngine::sortFreightsByStrategy(
    vector<freight>& freights,
    const Cargo& cargo) {
    
    switch (strategy_) {
        case RebalancingStrategy::MINIMIZE_FREIGHTS:
            // Sort by capacity descending (use bigger freights first)
            sort(freights.begin(), freights.end(),
                [](const freight& a, const freight& b) {
                    if (a.getCapacity() != b.getCapacity())
                        return a.getCapacity() > b.getCapacity();
                    return a.getTime() < b.getTime();
                });
            break;
            
        case RebalancingStrategy::MINIMIZE_TIME:
            // Sort by time ascending (earliest arrival first)
            sort(freights.begin(), freights.end(),
                [](const freight& a, const freight& b) {
                    if (a.getTime() != b.getTime())
                        return a.getTime() < b.getTime();
                    return a.getCapacity() > b.getCapacity();
                });
            break;
            
        case RebalancingStrategy::MAXIMIZE_UTILIZATION:
            // Sort by capacity ascending (fill smaller freights first)
            sort(freights.begin(), freights.end(),
                [](const freight& a, const freight& b) {
                    if (a.getCapacity() != b.getCapacity())
                        return a.getCapacity() < b.getCapacity();
                    return a.getTime() < b.getTime();
                });
            break;
    }
}

vector<freight> RebalancingEngine::findAlternativeFreights(
    const Cargo& cargo,
    const vector<freight>& allFreights,
    const vector<Shipment>& currentSchedule) {
    
    vector<freight> alternatives;
    
    // Find freights going to same destination
    for (const auto& f : allFreights) {
        if (f.getDestination() == cargo.getDestination() &&
            isFreightAvailable(f, -1, currentSchedule)) {
            alternatives.push_back(f);
        }
    }
    
    // Sort by current strategy
    sortFreightsByStrategy(alternatives, cargo);
    
    return alternatives;
}

vector<Shipment> RebalancingEngine::rebalanceAfterRemoval(
    int removedFreightIndex,
    const vector<Shipment>& currentSchedule,
    const vector<freight>& allFreights,
    const vector<Cargo>& allCargos) {
    
    cout << "\n🔄 ========== REBALANCING ENGINE ACTIVATED ==========\n";
    cout << "Strategy: ";
    switch (strategy_) {
        case RebalancingStrategy::MINIMIZE_FREIGHTS:
            cout << "MINIMIZE_FREIGHTS (use fewer freights)\n";
            break;
        case RebalancingStrategy::MINIMIZE_TIME:
            cout << "MINIMIZE_TIME (prioritize earliest arrival)\n";
            break;
        case RebalancingStrategy::MAXIMIZE_UTILIZATION:
            cout << "MAXIMIZE_UTILIZATION (fill to capacity)\n";
            break;
    }
    
    // Start with existing schedule minus the removed freight
    vector<Shipment> newSchedule;
    for (const auto& shipment : currentSchedule) {
        if (shipment.getFreightIndex() != removedFreightIndex) {
            newSchedule.push_back(shipment);
        }
    }
    
    // Get affected cargos
    vector<Cargo> affectedCargos = getAffectedCargos(
        removedFreightIndex, currentSchedule, allCargos);
    
    if (affectedCargos.empty()) {
        cout << "✓ No cargos were affected. No rebalancing needed.\n";
        return newSchedule;
    }
    
    cout << "\n📦 Reassigning " << affectedCargos.size() << " affected cargo(s)...\n";
    
    // Track cargo quantities to reassign
    unordered_map<int, int> cargoRemaining;
    for (const auto& cargo : affectedCargos) {
        cargoRemaining[cargo.getIndex()] = cargo.getCapacity();
    }
    
    // Group freights by destination for efficiency
    map<string, vector<freight>> freightsByDestination;
    for (const auto& f : allFreights) {
        if (f.getIndex() != removedFreightIndex) {
            freightsByDestination[f.getDestination()].push_back(f);
        }
    }
    
    // Sort freights in each destination group
    for (auto& pair : freightsByDestination) {
        sortFreightsByStrategy(pair.second, affectedCargos[0]);
    }
    
    int reassignedCount = 0;
    int newShipmentsCreated = 0;
    
    // Try to reassign each affected cargo
    for (const auto& cargo : affectedCargos) {
        cout << "\n  🔄 Processing Cargo C" << cargo.getIndex() 
             << " (Destination: " << cargo.getDestination() << ", Quantity: " 
             << cargo.getCapacity() << ")\n";
        
        auto it = freightsByDestination.find(cargo.getDestination());
        if (it == freightsByDestination.end() || it->second.empty()) {
            cout << "    ❌ No alternative freights available for this destination!\n";
            continue;
        }
        
        vector<freight>& candidateFreights = it->second;
        int remainingQuantity = cargoRemaining[cargo.getIndex()];
        
        for (auto& f : candidateFreights) {
            if (remainingQuantity <= 0) break;
            
            int availableCapacity = calculateRemainingCapacity(f, newSchedule);
            
            if (availableCapacity > 0) {
                int quantityToAssign = min(availableCapacity, remainingQuantity);
                
                // Find the highest schedule number and increment
                int maxScheduleNo = 0;
                for (const auto& s : newSchedule) {
                    if (s.getScheduleNo() > maxScheduleNo) {
                        maxScheduleNo = s.getScheduleNo();
                    }
                }
                
                Shipment newShipment(f, cargo, maxScheduleNo + 1, quantityToAssign);
                newSchedule.push_back(newShipment);
                
                remainingQuantity -= quantityToAssign;
                reassignedCount += quantityToAssign;
                newShipmentsCreated++;
                
                cout << "    ✅ Assigned " << quantityToAssign << " units to Freight F" 
                     << f.getIndex() << " (" << f.getTypeName() << ")\n";
                cout << "       Remaining capacity on F" << f.getIndex() << ": " 
                     << (availableCapacity - quantityToAssign) << "\n";
            }
        }
        
        if (remainingQuantity > 0) {
            cout << "    ⚠️  WARNING: " << remainingQuantity 
                 << " units could not be reassigned (insufficient capacity)\n";
        } else {
            cout << "    ✓ Cargo fully reassigned\n";
        }
    }
    
    cout << "\n📊 Rebalancing Summary:\n";
    cout << "  • Cargos reassigned: " << affectedCargos.size() << "\n";
    cout << "  • Total units redistributed: " << reassignedCount << "\n";
    cout << "  • New shipments created: " << newShipmentsCreated << "\n";
    cout << "  • Total shipments in new schedule: " << newSchedule.size() << "\n";
    
    cout << "\n✓ Rebalancing complete!\n";
    cout << "====================================================\n\n";
    
    return newSchedule;
}

RebalanceMetrics RebalancingEngine::calculateMetrics(
    const vector<Shipment>& newSchedule,
    const vector<Shipment>& originalSchedule,
    int removedFreightIndex) {
    
    RebalanceMetrics metrics;
    
    // Count unique freights used in new schedule
    unordered_set<int> freightIds;
    int totalCapacityUsed = 0;
    int totalCapacityAvailable = 0;
    
    for (const auto& shipment : newSchedule) {
        freightIds.insert(shipment.getFreightIndex());
        totalCapacityUsed += shipment.getCargoQuantity();
        totalCapacityAvailable += shipment.getFreight().getCapacity();
    }
    
    metrics.freightsUsed = freightIds.size();
    metrics.totalShipments = newSchedule.size();
    
    // Calculate average utilization
    if (totalCapacityAvailable > 0) {
        metrics.avgUtilization = (double)totalCapacityUsed / totalCapacityAvailable * 100.0;
    }
    
    // Count cargos that were reassigned
    unordered_set<int> affectedCargoIds;
    for (const auto& shipment : originalSchedule) {
        if (shipment.getFreightIndex() == removedFreightIndex) {
            affectedCargoIds.insert(shipment.getCargoIndex());
        }
    }
    metrics.totalCargoReassigned = affectedCargoIds.size();
    
    // Calculate time impact (difference in average arrival time)
    int oldTotalTime = 0, oldCount = 0;
    int newTotalTime = 0, newCount = 0;
    
    for (const auto& shipment : originalSchedule) {
        if (affectedCargoIds.find(shipment.getCargoIndex()) != affectedCargoIds.end()) {
            oldTotalTime += shipment.getFreightTime();
            oldCount++;
        }
    }
    
    for (const auto& shipment : newSchedule) {
        if (affectedCargoIds.find(shipment.getCargoIndex()) != affectedCargoIds.end()) {
            newTotalTime += shipment.getFreightTime();
            newCount++;
        }
    }
    
    if (oldCount > 0 && newCount > 0) {
        int oldAvgTime = oldTotalTime / oldCount;
        int newAvgTime = newTotalTime / newCount;
        metrics.timeImpact = newAvgTime - oldAvgTime;
    }
    
    return metrics;
}
