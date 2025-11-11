#pragma once

#include "FreightPlugin.h"
#include <iostream>
#include <unordered_set>

// HeavyHauler freight type plugin for industrial heavy cargo
class HeavyHaulerPlugin : public FreightPlugin {
public:
    std::string getPluginName() const override {
        return "HeavyHauler Plugin";
    }
    
    std::string getPluginVersion() const override {
        return "1.0.0";
    }
    
    std::string getFreightTypeName() const override {
        return "HeavyHauler";
    }
    
    int getBaseCapacity() const override {
        return 500; // Base capacity: 500 units (much larger)
    }
    
    double getCostPerUnit() const override {
        return 5.00; // $5.00 per unit (premium service)
    }
    
    double getSpeedFactor() const override {
        return 0.7; // Slower but much larger capacity
    }
    
    bool validateCargo(const Cargo& cargo) const override {
        // Only heavy industrial cargo (200-10000 weight)
        int capacity = cargo.getCapacity();
        return capacity >= 200 && capacity <= 10000;
    }
    
    bool canTransport(const std::string& destination) const override {
        // Only major routes (New York, Los Angeles, Chicago, Houston)
        static const std::unordered_set<std::string> majorRoutes = {
            "New York", "Los Angeles", "Chicago", "Houston"
        };
        
        return majorRoutes.find(destination) != majorRoutes.end();
    }
    
    void onLoad(const freight& f, const Cargo& c) const override {
        std::cout << "🏗️  HeavyHauler F" << f.getIndex() 
                  << " loading heavy industrial Cargo C" << c.getIndex() 
                  << " (" << c.getCapacity() << " units)\n";
    }
    
    void onUnload(const freight& f, const Cargo& c) const override {
        std::cout << "✓ HeavyHauler F" << f.getIndex() 
                  << " delivered heavy Cargo C" << c.getIndex() 
                  << " to major hub\n";
    }
    
    std::string getCustomInfo() const override {
        return "Industrial heavy cargo specialist - Major routes only";
    }
};

// Auto-register the plugin
REGISTER_FREIGHT_PLUGIN(HeavyHaulerPlugin, "HeavyHauler")
